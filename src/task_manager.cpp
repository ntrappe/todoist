#include "task_manager.hpp"
#include <format>
#include <string>

using namespace std;

string BLANK_DATE = "None\t";
int MAX_TASKS = 100;
int WARN_TASKS = 90;

int TaskManager::addTask(const string &title, Priority pr, optional<ymd> due) {
  int curr_num_tasks = static_cast<int>(size());

  // Check if it task cap (to avoid flooding)
  if (curr_num_tasks >= MAX_TASKS) {
    cerr << BLOOD << FAIL << " Task limit reached (100). Cannot add more tasks." << RESET << endl;
    return -1;
  }

  // Warn if approaching task cap
  if (curr_num_tasks >= WARN_TASKS) {
    cerr << GOLD << WARN << "  Warning: Approaching task limit (" << curr_num_tasks << "/100)." << RESET << endl;
  }

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
  auto task = make_unique<Task>(id, truncate(title), pr, due);
  Task *raw_task = task.get();

  auto [it, ok] = task_map.emplace(id, std::move(task));
  if (!ok) {
    cerr << BLOOD << FAIL << " Insertion of task failed." << RESET << endl;
    return -1;
  }

  // Now push onto the heap
  task_heap.push(raw_task);

  // Success confirmation message
  cout << GREEN << DONE << " Added task [" << id << "] " << truncate(title) << RESET << endl;

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
  // Start off table syntax
  out << BOLD << "ID   PRIORITY\tDUE\t\t\tTITLE" << RESET << endl;
  out << "----------------------------------------------------------------------\n";

  if (static_cast<int>(task_map.size()) < 1) {
    out << "NO TASKS." << endl;
    return;
  }

  auto heap_copy = task_heap;

  while (!heap_copy.empty()) {
    Task *task = heap_copy.top();
    heap_copy.pop();

    // Skip stale tasks
    if (task->state != Status::Pending)
      continue;

    bool overdue = task->due.has_value() && is_overdue(*task, get_today());
    // if (overdue)
    //   out << MAGENTA;

    string due_date;
    if (!task->due.has_value()) {
      due_date = BLANK_DATE + '\t';
    } else {
      due_date = to_string(task->due.value()) + ' ';
      due_date += is_overdue(*task, get_today()) ? RED : GREEN;
      due_date += is_overdue(*task, get_today()) ? "(" : "(+";
      due_date += to_string(task->days_until_due()) + "d)";
    }

    // Print id, priority, and due date
    out << "[" << task->id << "]  "
        << task->pr << '\t'
        // << (task->due.has_value() ? to_string(task->due.value()) : BLANK_DATE)
        << due_date << RESET << '\t'
        << task->title << endl;
    // << (overdue ? RESET : "") << endl;
  }

  // Finish off table syntax
  out << "----------------------------------------------------------------------\n";
  // out << BOLD << "Key: " << RESET << CRI_PR << " = Critical | " << HIGH_PR << "  = High | " << MED_PR << "  = Medium | " << LOW_PR << "  = Low" << endl
  //     << endl;
  out << BOLD << to_string(size()) << " tasks pending.\n\n";
}

bool TaskManager::completeTask(int id) {
  auto it = task_map.find(id);

  if (it == task_map.end()) {
    cerr << BLOOD << FAIL << " Could not find the task to complete." << RESET << endl;
    return false;
  }

  Task *raw = it->second.get();
  raw->state = Status::Completed;
  return true;
}

bool TaskManager::removeTask(int id) {
  auto it = task_map.find(id);

  if (it == task_map.end()) {
    cerr << BLOOD << FAIL << " Could not find the task to remove." << RESET << endl;
    return false;
  }

  task_map.erase(it);
  return true;
}
