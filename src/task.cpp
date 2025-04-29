#include "task.hpp"

using namespace std;
using namespace chrono;

// ctor
Task::Task(int i, string t, Priority p, optional<ymd> d)
    : id(i), title(t), pr(p), due(d) {}

bool Task::operator==(const Task &other) const {
  return id == other.id && title == other.title && pr == other.pr && state == other.state;
}

int Task::days_until_due() const {
  sys_days sys_today = sys_days(get_today());
  sys_days sys_deadline = sys_days(due.value());

  days days_left = sys_deadline - sys_today;
  return days_left.count();
}

bool is_overdue(const Task &task, const ymd &today) {
  return task.due.has_value() && (task.state == Status::Pending) && (task.due.value() < today);
}

string truncate(const string &title) {
  if (title.length() <= TITLE_MAX_LEN)
    return title;
  return title.substr(0, TITLE_MAX_LEN - 3) + "...";
}

ymd get_today() {
  auto now = system_clock::now();
  return floor<days>(now);
}

ostream &operator<<(ostream &out, Priority p) {
  switch (p) {
  // case Priority::Low:
  //   return out << LOW_PR << '\t';
  // case Priority::Medium:
  //   return out << MED_PR << '\t';
  // case Priority::High:
  //   return out << HIGH_PR << '\t';
  // case Priority::Critical:
  //   return out << CRI_PR << '\t';
  // case Priority::Low:
  //   return out << BOLD << "!" << RESET << '\t';
  // case Priority::Medium:
  //   return out << BOLD << CYAN << "!!" << RESET << '\t';
  // case Priority::High:
  //   return out << BOLD << GOLD << "!!!" << RESET;
  // case Priority::Critical:
  //   return out << BOLD << MAGENTA << "!!!!" << RESET;
  case Priority::Low:
    return out << "🟢\t";
  case Priority::Medium:
    return out << "🔵\t";
  case Priority::High:
    return out << "🟠\t";
  case Priority::Critical:
    return out << "🔴\t";
  default:
    return out << "--\t";
  }
}