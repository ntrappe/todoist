#include "task.hpp"

using namespace std;

// ctor
Task::Task(int i, string t, Priority p, optional<ymd> d)
    : id(i), title(t), pr(p), due(d) {}

bool Task::operator==(const Task &other) const {
  return id == other.id && title == other.title && pr == other.pr && state == other.state;
}