#pragma once
#include "task.hpp"
#include "task_manager.hpp"
#include <iostream>

class TaskCLI {
public:
  TaskCLI() {}

  void run(int argc, char *argv[]);

  void handleAdd();
  void handleList();
  void handleNext();
  void handleComplete();
  void handleRemove();
  void handleEdit();
  bool loadTasks();

  void help();
  void help_add();
};