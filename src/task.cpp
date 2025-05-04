#include "task.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace chrono;

const std::string PRIORITY_BLOCK[4] = {
    "\e[0;104m \033[0m", "\e[0;107m \033[0m", "\e[0;105m \033[0m", "\e[0;101m \033[0m"};

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

// ymd string_to_ymd(const string &date) {
//   int y, m, d;
//   if (sscanf(date.c_str(), "%d-%d-%d", &y, &m, &d) == 3) {
//     return ymd{year{y}, month{static_cast<unsigned>(m)}, day{static_cast<unsigned>(d)}};
//   } else {
//     throw invalid_argument("Invalid date format: " + date);
//   }
// }

// string ymd_to_string(const ymd &date) {
//   if (!date.ok()) {
//     throw invalid_argument("Invalid year_month_day");
//   }

//   int y = int(date.year());
//   unsigned m = unsigned(date.month());
//   unsigned d = unsigned(date.day());

//   ostringstream oss;
//   oss << setfill('0')
//       << setw(4) << y << '-'
//       << setw(2) << m << '-'
//       << setw(2) << d;

//   return oss.str();
// }

string print_priority(Priority p) {
  string bar;
  int total_blocks = 8;
  int painted_blocks = (static_cast<int>(p) + 1) * 2;

  for (int i = 0; i < total_blocks; i++, painted_blocks--) {
    if (painted_blocks > 0)
      bar += PRIORITY_BLOCK[static_cast<int>(p)];
    else
      bar += "\033[40m \033[0m"; // Unfilled block with black background
  }
  return bar; // Safe: returning by value

  // switch (p) {
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
  // case Priority::Low:
  //   return out << "🟢\t";
  // case Priority::Medium:
  //   return out << "🔵\t";
  // case Priority::High:
  //   return out << "🟠\t";
  // case Priority::Critical:
  //   return out << "🔴\t";
  // default:
  //   return out << "--\t";
  // }
}