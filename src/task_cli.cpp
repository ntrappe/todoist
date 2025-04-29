#include "task_cli.hpp"

using namespace std;

void TaskCLI::run(int argc, char *argv[]) {
  TaskManager mgr;

  if (argc > 1) {
    // One-shot mode
    vector<string> args(argv + 1, argv + argc);
    string cmd = args[0];

    if (cmd == "add") {
      // ex: add "Title here" --priority H --due 2025-06-01
      int id1 = mgr.addTask("Create instructions for PA1", Priority::High);
      // Task *firstTask = mgr.next();
      // cout << "added task [" << firstTask->id << "] " << firstTask->title << endl;
      int id2 = mgr.addTask("Break down tasks for PA1", Priority::Medium, ymd(2025y, chrono::June, 21d));
      int id3 = mgr.addTask("Code PA1 fxn", Priority::Critical, ymd(2025y, chrono::September, 2d));
      int id4 = mgr.addTask("Do code reviews..", Priority::Low, ymd(2025y, chrono::September, 3d));
      int id5 = mgr.addTask("Finish PA0 from last year", Priority::High, ymd(2023y, chrono::September, 3d));
      int id6 = mgr.addTask("Start thinking about idea for PA2 and PA3 next semester");

      // Task *secondTask = mgr.next();
      // cout << "added task [" << secondTask->id << "] " << secondTask->title << endl;
      mgr.printTasks();
    } else if (cmd == "list") {
      cout << "you want to list them all" << endl;
    } else if (cmd == "--help" || cmd == "help" || cmd == "-h") {
      help();
    }
  } else {
    // Interactive mode
    string line;
    while (true) {
      cout << "> ";
      if (!getline(cin, line) || line == "quit")
        break;
    }
  }
}

void TaskCLI::help() {
  cout << "todo <command>\n\nUsage:\n\n";
  cout << "todo <command> -h\tquick help on <command>\n";
  cout << "todo add\t\tadd a task to the list\n";
  cout << "todo complete\t\tmark a task as complete\n";
  cout << "todo edit\t\tedit a specific task\n";
  cout << "todo list\t\tlist all pending tasks\n";
  cout << "todo remove\t\tremove a specific task\n";
}