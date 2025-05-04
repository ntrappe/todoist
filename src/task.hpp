/**
 * File:    task.hpp
 * Purpose: Plain data record for one to‑do item.
 */
#pragma once
#include <chrono>
#include <optional>
#include <string>
#include <vector>

enum class Priority { Low = 0,
                      Medium,
                      High,
                      Critical };
enum class Status { Pending,
                    Completed,
                    Archived,
                    All };

/* ANSI text styles */
static constexpr char const *BOLD = "\033[1m";
static constexpr char const *NOTICE = "\e[1;35m";
static constexpr char const *BLOOD = "\033[31m";
static constexpr char const *CYAN = "\033[36m";
static constexpr char const *GOLD = "\033[33m";
static constexpr char const *GREEN = "\033[32m";
static constexpr char const *MAGENTA = "\033[35m";
static constexpr char const *RED = "\033[91m";
static constexpr char const *BG_BLACK = "\033[40m";
static constexpr char const *BG_MAGENTA = "\033[45m";
static constexpr char const *RESET = "\033[0m";

/* Symbols */
static constexpr char const *WARN = "⚠️";
static constexpr char const *FAIL = "⛔️";
static constexpr char const *DONE = "✅";

/* Numerical constants */
static constexpr size_t TITLE_MAX_LEN = 35;

using ymd = std::chrono::year_month_day;

// Declare the stream-insertion overloads
// std::ostream &operator<<(std::ostream &out, Priority p);

struct Task {
  // ---------- data members ----------
  int id{-1};
  std::string title;
  Priority pr{Priority::Medium};
  Status state{Status::Pending};
  std::optional<ymd> due{std::nullopt};

  // std::vector<std::string> tags;
  // sys_seconds created;
  // std::optional<sys_seconds> completed;

  Task() = default; // leaves id = -1 (invalid)

  Task(int id,
       std::string title,
       Priority pr = Priority::Medium,
       std::optional<ymd> due = std::nullopt);

  // Member signature. Overload equals operator.
  bool operator==(const Task &other) const;

  int days_until_due() const;
};

/* --------------------------- Utility functions --------------------------- */

/**
 * Gets today's date as a year_month_day (YYYY-MM-DD).
 *
 * @return Today's date.
 */
ymd get_today();

/**
 * Returns true if the task's due date has passed compared to today.
 *
 * @param task The task to check.
 * @param today The current date to compare against.
 * @return true if overdue, false otherwise.
 */
bool is_overdue(const Task &task, const ymd &today);

/**
 * Truncates a title to a maximum length, adding "..." if too long.
 *
 * @param title The original title string.
 * @return A possibly shortened version with ellipsis.
 */
std::string truncate(const std::string &title);

std::string print_priority(Priority p);

/**
 * @brief   Convert an ISO‑8601 string into a year_month_day.
 * @param   date A string in “YYYY‑MM‑DD” format.
 * @return  A year_month_day object.
 * @throws  std::invalid_argument if invalid format.
 */
ymd string_to_ymd(const std::string &date);

/**
 * @brief   Convert a year_month_day into an ISO‑8601 date string.
 * @param   date  A year_month_day object to stringify.
 * @return  A string in “YYYY‑MM‑DD” format.
 * @throws  std::invalid_argument if the provided date is not a valid ymd.
 */
std::string ymd_to_string(const ymd &date);