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
      cout << "... added " << static_cast<int>(mgr.size()) << " items\n\n";
      mgr.printTasks();
    } else if (cmd == "list") {
      cout << "you want to list them all" << endl;
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