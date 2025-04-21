#include "task_manager.hpp"

using namespace std;

int TaskManager::add(const string &title, Priority pr, optional<ymd> due) {
  // Empty title → reject immediately
  if (title.empty()) {
    throw invalid_argument("Task title cannot be empty");
  }

  // Check if we already have a task by that name and due date

  return 0;
}

// Reports how many tasks are in the system
// Maps to MAP not QUEUE bc pts go stale in heap and popped lazily
size_t TaskManager::size() const {
  return task_map.size();
}
