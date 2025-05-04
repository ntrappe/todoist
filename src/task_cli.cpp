#include "task_cli.hpp"
#include <algorithm>
#include <chrono>
#include <optional>
#include <string>
#include <string_view>

using namespace std;
using namespace std::chrono;

Priority TaskCLI::parsePriority(string_view txt) {
  auto lower = [](char c) { return char(tolower(c)); };
  string s{txt};
  transform(s.begin(), s.end(), s.begin(), lower);

  if (s == "low")
    return Priority::Low;
  if (s == "med" || s == "medium")
    return Priority::Medium;
  if (s == "high")
    return Priority::High;
  if (s == "critical" || s == "crit")
    return Priority::Critical;
  return Priority::Medium;
}

optional<ymd> TaskCLI::parseDate(const string &in) {
  int y, m, d;
  auto pos1 = in.find('-');  // between year and month
  auto pos2 = in.rfind('-'); // between month and day

  if (pos1 == string::npos || pos2 == pos1)
    return nullopt;

  try {
    y = stoi(in.substr(0, pos1));
    m = stoi(in.substr(pos1 + 1, pos2 - pos1 - 1));
    d = stoi(in.substr(pos2 + 1));
  } catch (...) {
    return nullopt;
  }

  year_month_day ym{year{y}, month{unsigned(m)}, day{unsigned(d)}};
  return ym.ok() ? optional{ym} : nullopt;
}

int TaskCLI::parseAdd(int argc, char *argv[],
                      string &title,
                      Priority &pr,
                      optional<ymd> &due) {

  string_view title_arg{argv[TITLE_IDX]};
  if (title_arg == "help") {
    printAddHelp();
    return EXIT_FAILURE;
  }
  title = string{title_arg};

  for (int i = TITLE_IDX + 1; i < argc; ++i) {
    string_view arg{argv[i]};
    if (arg == "--priority" && ((i + 1) < argc))
      pr = parsePriority(argv[++i]);
    else if (arg == "--due" && ((i + 1) < argc))
      due = parseDate(argv[++i]);
    else
      cerr << "Received unknown flag or argument: " << arg << endl;
  }

  return EXIT_SUCCESS;
}

int TaskCLI::run(int argc, char *argv[]) {
  TaskManager mgr;

  // 1) Load existing tasks (silently ignore if file is missing or malformed)
  mgr.loadFromFile("tasks.json");

  if (argc > 1) {
    // One-shot mode
    string_view cmd{argv[1]};
    if (cmd == "add") {
      // 2) Check for invalid usage
      if (argc < ADD_MIN_ARGS) {
        cerr << BLOOD << FAIL << " Adding a task requires at least 1 argument. None provided." << RESET << endl;
        return EXIT_FAILURE;
      }

      // 3) Parse the add arguments into these locals
      string title = "";
      Priority pr = Priority::Medium;
      optional<ymd> due_opt = nullopt;

      if (parseAdd(argc, argv, title, pr, due_opt) != EXIT_SUCCESS)
        return EXIT_FAILURE;

      // 4) Create the task in the manager and report its new ID
      int id = mgr.addTask(title, pr, due_opt);
      if (id == FXN_FAILURE)
        return EXIT_FAILURE;

      cout << NOTICE << DONE << " Successfully add task #"
           << id << " (\"" << truncate(title) << "\")." << RESET << endl
           << endl;

      // 5) Save the updated task list back to disk
      try {
        mgr.saveToFile("tasks.json");
      } catch (const exception &e) {
        cerr << "Error: failed to save tasks.json (" << e.what() << ")\n";
        return EXIT_FAILURE;
      }
      return EXIT_SUCCESS;
    } else if (cmd == "complete") {
      // 2) Check for invalid usage
      if (argc < ADD_MIN_ARGS) {
        cerr << BLOOD << FAIL << " Completing a task requires at least 1 argument. None provided." << RESET << endl;
        return EXIT_FAILURE;
      }

      // 3) Asking for help only
      if (strcasecmp(argv[TASK_ID_IDX], "help") == 0) {
        printCompleteHelp();
        return EXIT_FAILURE;
      }

      // 4) Extract ID and change to complete
      int id = atoi(argv[TASK_ID_IDX]);
      if (id == 0 || !mgr.completeTask(id)) {
        return EXIT_FAILURE;
      } else {
        cout << NOTICE << DONE << " Successfully completed task #"
             << id << endl
             << endl;
      }

      // 5) Save the updated task list back to disk
      try {
        mgr.saveToFile("tasks.json");
      } catch (const exception &e) {
        cerr << "Error: failed to save tasks.json (" << e.what() << ")\n";
        return EXIT_FAILURE;
      }
      return EXIT_SUCCESS;

    } else if (cmd == "list") {
      // 2) by default, just list will show pending
      if (argc < 3) {
        mgr.printPendingTasks();
        return EXIT_SUCCESS;
      }

      // 3) otherwise, check if arg is help or a filter
      string_view filter{argv[2]};
      if (filter == "help") {
        printListHelp();
        return EXIT_FAILURE;
      } else if (filter == "-a" || filter == "--all") {
        mgr.printAllTasks();
        return EXIT_SUCCESS;
      } else if (filter == "-p" || filter == "--pending") {
        mgr.printPendingTasks();
        return EXIT_SUCCESS;
      } else if (filter == "-c" || filter == "--completed") {
        mgr.printCompletedTasks();
        return EXIT_SUCCESS;
      } else if (filter == "-r" || filter == "--archived") {
        mgr.printArchivedTasks();
        return EXIT_SUCCESS;
      } else {
        cout << BLOOD << FAIL << " Argument not recognized." << RESET << endl;
        return EXIT_FAILURE;
      }
    } else if (cmd == "remove") {
      // 2) Check for invalid usage
      if (argc < ADD_MIN_ARGS) {
        cerr << BLOOD << FAIL << " Removing a task requires at least 1 argument. None provided." << RESET << endl;
        return EXIT_FAILURE;
      }

      // 3) Asking for help only
      if (strcasecmp(argv[TASK_ID_IDX], "help") == 0) {
        printRemoveHelp();
        return EXIT_FAILURE;
      }

      // 4) Extract ID and remove
      int id = atoi(argv[TASK_ID_IDX]);
      if (id == 0 || !mgr.removeTask(id)) {
        return EXIT_FAILURE;
      } else {
        cout << NOTICE << DONE << " Successfully removed task #"
             << id << endl
             << endl;
      }

      // 5) Save the updated task list back to disk
      try {
        mgr.saveToFile("tasks.json");
      } catch (const exception &e) {
        cerr << "Error: failed to save tasks.json (" << e.what() << ")\n";
        return EXIT_FAILURE;
      }
      return EXIT_SUCCESS;
    } else if (cmd == "archive") {
      // 2) Check for invalid usage
      if (argc < ADD_MIN_ARGS) {
        cerr << BLOOD << FAIL << " Archiving a task requires at least 1 argument. None provided." << RESET << endl;
        return EXIT_FAILURE;
      }

      // 3) Asking for help only
      if (strcasecmp(argv[TASK_ID_IDX], "help") == 0) {
        // printArchiveHelp();
        return EXIT_FAILURE;
      }

      // 4) Extract ID and remove
      int id = atoi(argv[TASK_ID_IDX]);
      if (id == 0 || !mgr.archiveTask(id)) {
        return EXIT_FAILURE;
      } else {
        cout << NOTICE << DONE << " Successfully archived task #"
             << id << endl
             << endl;
      }

      // 5) Save the updated task list back to disk
      try {
        mgr.saveToFile("tasks.json");
      } catch (const exception &e) {
        cerr << "Error: failed to save tasks.json (" << e.what() << ")\n";
        return EXIT_FAILURE;
      }
      return EXIT_SUCCESS;
    } else {
      cerr << BLOOD << FAIL << " Did not recognize command." << RESET << endl;
      return EXIT_FAILURE;
    }
  } else {
    // Interactive mode (e.g., ./todo)
    cout << NOTICE << "\nWelcome to the Task Manager." << RESET << endl;
    cout << "Interactive mode is not supported at this time. Run './todo help' for more information." << endl
         << endl;
  }

  return EXIT_SUCCESS;
}