/**
 * @file    task.cpp
 * @brief   Implementations for Task, date utilities, and printing helpers.
 */

#include "task.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace std::chrono;

// Visual blocks for each Priority level (Low→Critical)
static constexpr const char *kPriorityBlocks[4] = {
    "\e[0;104m \033[0m",
    "\e[0;107m \033[0m",
    "\e[0;105m \033[0m",
    "\e[0;101m \033[0m"};

static const int LEN_PRIORITY_BAR = 8;

/**
 * @brief  Construct a Task with all fields.
 */
Task::Task(int i, string t, Priority p, optional<ymd> d)
    : id(i), title(t), pr(p), due(d) {}

/**
 * @brief  Compare two tasks for equality (ignoring due date).
 */
bool Task::operator==(const Task &other) const {
  return id == other.id && title == other.title && pr == other.pr && state == other.state;
}

/**
 * @brief  Days remaining until due date; negative if overdue.
 */
int Task::days_until_due() const {
  sys_days sys_today = sys_days(get_today());
  sys_days sys_deadline = sys_days(due.value());
  return (sys_deadline - sys_today).count();
}

/**
 * @brief  Return true if a Pending task’s due date is before today.
 */
bool is_overdue(const Task &task, const ymd &today) {
  return task.due.has_value() && (task.state != Status::Completed) && (task.due.value() < today);
}

/**
 * @brief  Shorten titles longer than TITLE_MAX_LEN, appending "...".
 */
string truncate(const string &title) {
  if (title.length() <= TITLE_MAX_LEN)
    return title;
  return title.substr(0, TITLE_MAX_LEN - 3) + "...";
}

/**
 * @brief  Get the current date as year_month_day.
 */
ymd get_today() {
  auto now = system_clock::now();
  return floor<days>(now);
}

/**
 * @brief  Render a bar of colored blocks for a Priority.
 */
string print_priority(Priority p) {
  string bar;
  int total_blocks = LEN_PRIORITY_BAR;
  int painted_blocks = (static_cast<int>(p) + 1) * 2;

  for (int i = 0; i < total_blocks; i++, painted_blocks--) {
    if (painted_blocks > 0)
      bar += kPriorityBlocks[static_cast<int>(p)];
    else
      bar += "\033[40m \033[0m"; // Unfilled block with black background
  }
  return bar; // Safe: returning by value
}

string print_status(Status s) {
  switch (s) {
  case Status::Archived:
    return "ARCHIVED";
  case Status::Pending:
    return "PENDING";
  case Status::Completed:
    return "COMPLETED";
  default:
    return "---";
  }
}
