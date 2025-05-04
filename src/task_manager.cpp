#include "task_manager.hpp"
#include <format>
#include <fstream>
#include <strings.h>

using namespace std;

const string BLANK_DATE = "None";
int MAX_TASKS = 100;
int WARN_TASKS = 90;

int TaskManager::addTask(const string &title, Priority pr, optional<ymd> due) {
  // Empty title → reject immediately
  if (title.empty()) {
    cerr << BLOOD << FAIL << " Task title cannot be empty." << RESET << endl;
    return FXN_FAILURE;
  }

  int curr_num_tasks = static_cast<int>(size());

  // Check if it task cap (to avoid flooding)
  if (curr_num_tasks >= MAX_TASKS) {
    cerr << BLOOD << FAIL << " Task limit reached (100). Cannot add more tasks." << RESET << endl;
    return FXN_FAILURE;
  }

  // Warn if approaching task cap
  if (curr_num_tasks >= WARN_TASKS) {
    cerr << GOLD << WARN << "  Warning: Approaching task limit (" << curr_num_tasks << "/100)." << RESET << endl;
  }

  // Duplicate check
  for (auto &[id, ptr] : task_map) {
    if (strcasecmp(ptr->title.c_str(), title.c_str()) == 0 && ptr->due == due) {
      cerr << BLOOD << FAIL << " Duplicate task: same title and due date already exists." << RESET << endl
           << endl;
      return FXN_FAILURE;
    }
  }

  return insertTaskUnchecked(next_id++, title, pr, due);
}

int TaskManager::insertTaskUnchecked(int id, const string &title, Priority pr, optional<ymd> due) {
  // Check for id collision (shouldn't happen, but just in case)
  if (task_map.contains(id)) {
    cerr << BLOOD << FAIL << " Duplicate ID (" << id << ") on insertTaskUnchecked." << RESET << endl;
    return FXN_FAILURE;
  }

  // Create a unique pointer → move into map (not copyable) to indicate
  // ownership by map. Grab raw pointer before moving.
  auto task = make_unique<Task>(id, title, pr, due);
  Task *raw_task = task.get();

  auto [it, ok] = task_map.emplace(id, std::move(task));
  if (!ok) {
    cerr << BLOOD << FAIL << " Insertion of task failed (" << id << ")." << RESET << endl;
    return FXN_FAILURE;
  }

  // Now push onto the heap
  task_heap.push(raw_task);

  // Maintain correct ID (depending on how many tasks we have already)
  next_id = max(next_id, id + 1);
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

string TaskManager::formatDue(const Task *task, const ymd &today) const {
  if (!task->due.has_value() && task->state == Status::Completed) {
    return GREEN + string("Completed") + RESET + string("\t\t");
  } else if (!task->due.has_value() && task->state == Status::Archived) {
    return GOLD + string("Archived") + RESET + string("\t\t");
  } else if (!task->due.has_value()) {
    return BLANK_DATE + string("\t\t\t");
  }

  // ISO date + color + overdue info
  string out = to_string(task->due.value()) + ' ';
  bool over = is_overdue(*task, today);
  out += over ? RED : GREEN;
  out += '(';
  out += over ? "" : "+";
  out += to_string(task->days_until_due()) + "d)\t";

  return out;
}

void TaskManager::printTasks(Status filter) {
  // 1) Header
  cout << BOLD << "\nID   PRIORITY\tDUE\t\t\tTITLE" << RESET << endl;
  cout << "---------------------------------------------------------------------------" << endl;

  // 2) Gather matching tasks in heap order
  vector<Task *> list;
  auto heap_copy = task_heap;

  while (!heap_copy.empty()) {
    Task *t = heap_copy.top();
    heap_copy.pop();

    switch (filter) {
    case Status::All:
      break;
    case Status::Pending:
      if (t->state != Status::Pending)
        continue;
      break;
    case Status::Completed:
      if (t->state != Status::Completed)
        continue;
      break;
    case Status::Archived:
      if (t->state != Status::Archived)
        continue;
      break;
    }
    list.push_back(t);
  }

  // 3) Body
  if (list.empty())
    cout << "No tasks." << endl;
  else {
    for (auto *task : list) {
      cout
          << "[" << task->id << "]  "
          << print_priority(task->pr) << "\t"
          << formatDue(task, get_today()) << RESET
          << truncate(task->title) << endl;
    }
  }

  // 4) Footer
  const char *label = nullptr;
  switch (filter) {
  case Status::All:
    label = "total";
    break;
  case Status::Pending:
    label = "pending";
    break;
  case Status::Completed:
    label = "completed";
    break;
  case Status::Archived:
    label = "archived";
    break;
  }
  cout << "---------------------------------------------------------------------------\n";
  cout << BOLD << list.size() << " tasks " << label << ".\n\n";
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

bool TaskManager::archiveTask(int id) {
  auto it = task_map.find(id);

  if (it == task_map.end()) {
    cerr << BLOOD << FAIL << " Could not find the task to archive." << RESET << endl;
    return false;
  }

  Task *raw = it->second.get();
  raw->state = Status::Archived;
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

bool TaskManager::loadFromFile(const string &filename) {
  ifstream in(filename);

  // Did not find file. Not an error because this might be the first time we've run
  // the program so nothing saved yet.
  if (!in)
    return false;

  string line;
  int id = -1, pr = 0, status = 0;
  string title;
  optional<ymd> due_opt;
  bool has_id = false, has_title = false, has_pr = false, has_status = false;

  // Did find file. Read in each individual line and extract fields.
  while (getline(in, line)) {
    stringstream ss(line);

    if (line.find("\"id\":") != string::npos) {
      ss.ignore(256, ':');
      ss >> id;
      has_id = true;
    } else if (line.find("\"title\":") != string::npos) {
      size_t start = line.find("\"", line.find(':')) + 1;
      size_t end = line.rfind("\"");
      title = line.substr(start, end - start);
      has_title = true;
    } else if (line.find("\"priority\":") != string::npos) {
      ss.ignore(256, ':');
      ss >> pr;
      has_pr = true;
    } else if (line.find("\"due\":") != string::npos) {
      if (line.find("null") != string::npos) {
        due_opt = nullopt;
      } else {
        size_t start = line.find("\"", line.find(':')) + 1;
        size_t end = line.rfind("\"");
        string due_str = line.substr(start, end - start);
        due_opt = to_ymd(due_str);
      }
    } else if (line.find("\"status\":") != string::npos) {
      ss.ignore(256, ':');
      ss >> status;
      has_status = true;
    } else if (line.find("}") != string::npos) {
      // Check if we have all the required fields
      if (has_id && has_title && has_pr && has_status) {
        int result = insertTaskUnchecked(id, title, static_cast<Priority>(pr), due_opt);
        if (result == -1)
          cerr << BLOOD << FAIL << " Insertion of task failed." << RESET << endl;
        else
          task_map[id]->state = static_cast<Status>(status);
      }
      // Reset for next task
      has_id = has_title = has_pr = has_status = false;
      id = pr = status = -1;
      due_opt = nullopt;
      title.clear();
    }
  }

  return true;
}

bool TaskManager::saveToFile(const string &filename) const {
  ofstream out(filename);

  if (!out) {
    cerr << BLOOD << FAIL << " Error opening file " << filename << ") for writing." << endl;
    return false;
  }

  out << "{\n\t\"tasks\": [\n";

  int count = 0;
  for (const auto &[id, task_ptr] : task_map) {
    const Task &t = *task_ptr;
    out << "\t\t{\n"; // open braces
    out << "\t\t\t\"id\": " << t.id << ",\n";
    out << "\t\t\t\"title\": \"" << t.title << "\",\n";
    out << "\t\t\t\"priority\": " << static_cast<int>(t.pr) << ",\n";
    if (t.due.has_value())
      out << "\t\t\t\"due\": \"" << to_string(t.due.value()) << "\",\n";
    else
      out << "\t\t\t\"due\": null,\n";
    out << "\t\t\t\"status\": " << static_cast<int>(t.state) << endl;
    out << "\t\t}" << (++count < task_map.size() ? "," : "") << endl;
  }

  out << "\t]\n}";

  return true;
}