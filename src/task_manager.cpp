#include "task_manager.hpp"
#include <format>
#include <string>

using namespace std;

string BLANK_DATE = "XXXX-XX-XX";

int TaskManager::addTask(const string &title, Priority pr, optional<ymd> due) {
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

  // Create a unique pointer → move into map (not copyable) to indicate
  //   ownership by map. Grab raw pointer before moving.
  int id = next_id++;
  auto task = make_unique<Task>(id, title, pr, due);
  Task *raw_task = task.get();
  auto [it, ok] = task_map.emplace(id, std::move(task));

  // Sanity check
  if (!ok) {
    cerr << "Insertion of task failed" << endl;
    return -1;
  }

  // Now push onto the heap
  task_heap.push(raw_task);

  // Sanity check
  auto found = task_map.find(id);
  if (found == task_map.end())
    cerr << "Failed to find task that was just added" << endl;

  return id;
}

Task *TaskManager::next() {
  // Pop until we find a stil-pending task or exhaust the heap
  while (!task_heap.empty()) {
    Task *task = task_heap.top();
    task_heap.pop();

    if (task->state == Status::Pending)
      return task;
  }

  // Heap empty or exhausted
  return nullptr;
}

void TaskManager::printTasks(ostream &out) {
  out << "ID   PRIORITY\tDUE\t\tTITLE\n";
  out << "------------------------------------------------------------------\n";

  if (static_cast<int>(task_map.size()) < 1) {
    out << "NO TASKS." << endl;
    return;
  }

  auto heap_copy = task_heap;
  // out << "size of map " << static_cast<int>(task_map.size()) << endl;
  // out << "size of OG heap " << static_cast<int>(task_heap.size()) << endl;
  // out << "size of copied heap " << static_cast<int>(heap_copy.size()) << endl;
  while (!heap_copy.empty()) {
    Task *task = heap_copy.top();
    heap_copy.pop();
    if (task->state == Status::Pending) {
      out << "[" << task->id << "]  ";
      out << task->pr << "\t";
      if (task->due.has_value()) {
        string due_date = to_string(task->due.value());
        if (is_overdue(*task, get_today())) {
          out << RED << due_date << RESET;
        } else {
          out << due_date;
        }
        out << "\t";
      } else {
        out << BLANK_DATE << "\t";
      }
      // out << (task->due.has_value() ? to_string(task->due.value()) : BLANK_DATE) << "\t";
      out << task->title << endl;
    }
  }
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

string colorCode(Priority pr) {
  switch (pr) {
  case Priority::Low:
    return "\033[32m]"; // green
  case Priority::Medium:
    return "\033[34m]"; // blue
  case Priority::High:
    return "\033[35m]"; // magenta
  case Priority::Critical:
    return "\033[31m]"; // red
  default:
    return "\033[0m]"; // reset
  }
}

string prStr(Priority pr) {
  switch (pr) {
  case Priority::Low:
    return "LOW\t\t";
  case Priority::Medium:
    return "MEDIUM\t";
  case Priority::High:
    return "HIGH\t\t";
  case Priority::Critical:
    return "CRITICAL\t";
  default:
    return "--\t\t";
  }
}

void TaskManager::printHeap(ostream &out) const {
  // Copy head so we can pop without mutating the real one
  auto heap_copy = task_heap;

  if (heap_copy.empty()) {
    out << "No tasks added." << endl;
    return;
  }

  out << TaskManager::TASK_TABLE_HEADER;
  out << TaskManager::TABLE_SEPARATOR;

  int i = 0;
  while (!heap_copy.empty() && i < 2) {
    Task *t = heap_copy.top();
    out << "Got item " << t->id << endl;
    heap_copy.pop();
    i++;
    // out << "[" << t->id << "]" << endl;
  }

  out << "done\n";

  // while (!tasks.empty()) {
  //   Task *t = tasks.top();
  //   tasks.pop();
  //   // Check if stale (lazy pop)
  //   if (t->state != Status::Pending) {
  //     continue;
  //   }

  //   string pr_str = colorCode(t->pr);
  //   cout << "[" << t->id << "]\t"
  //        << pr_str << prStr(t->pr) << "\033[0m]";
  //   if (t->due.has_value()) {
  //     ymd date = t->due.value();
  //     cout << date << "\t";
  //   } else {
  //     cout << "--\t\t";
  //   }
  //   cout << t->title << endl;
  // }
}