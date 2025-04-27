#include "task.hpp"

using namespace std;

ostream &operator<<(ostream &out, Priority p) {
  switch (p) {
  case Priority::Low:
    return out << GREEN << "!\t" << RESET;
  case Priority::Medium:
    return out << BLUE << "!!\t" << RESET;
  case Priority::High:
    return out << "\033[35m" << "!!!    " << RESET;
  case Priority::Critical:
    return out << RED << "!!!!    " << RESET;
  default:
    return out << "--";
  }
}

// ctor
Task::Task(int i, string t, Priority p, optional<ymd> d)
    : id(i), title(t), pr(p), due(d) {}

bool Task::operator==(const Task &other) const {
  return id == other.id && title == other.title && pr == other.pr && state == other.state;
}