#include "task_manager.hpp"
#include <iostream>

using namespace std;

int TaskManager::add(const string &title, Priority pr, optional<ymd> due) {
  // Empty title → reject immediately
  if (title.empty()) {
    throw invalid_argument("Task title cannot be empty");
  }

  // Check if we already have a task by that name and due date
  for (auto &[id, ptr] : task_map) {
    if (ptr->title == title && ptr->due == due) {
      throw invalid_argument("Task was already added");
    }
  }

  int id = next_id++;
  auto task = make_unique<Task>(id, title, pr, due);
  // More efficient than insert (less copying)
  task_map.emplace(id, move(task));

  return id;
}

bool TaskManager::complete(int id) {
  auto it = task_map.find(id);

  if (it == task_map.end()) {
    cerr << "Could not find the task to complete\n";
    return false;
  }

  Task *raw = it->second.get();
  raw->state = Status::Completed;
  return true;
}

bool TaskManager::remove(int id) {
  auto it = task_map.find(id);

  if (it == task_map.end()) {
    cerr << "Could not find the task to remove\n";
    return false;
  }

  task_map.erase(it);
  return true;
}

// Reports how many tasks are in the system
// Maps to MAP not QUEUE bc pts go stale in heap and popped lazily
size_t TaskManager::size() const {
  return task_map.size();
}
