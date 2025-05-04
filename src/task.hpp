/**
 * @file    task.hpp
 * @author  Nicole Trappe
 * @date    2025‑05‑04
 * @brief   Core Task data structure and utility functions for TaskMaster++.
 *
 * This header defines the Task record, its operations (equality, due‑date math),
 * and standalone helpers for date parsing/formatting, title truncation, etc.
 */
#pragma once
#include <chrono>
#include <optional>
#include <string>
#include <vector>

// Shortcut for C++20 chrono date type YYYY‑MM‑DD
using ymd = std::chrono::year_month_day;

/**
 * @enum Priority
 * @brief Levels of task urgency.
 */
enum class Priority { Low = 0,
                      Medium,
                      High,
                      Critical };

/**
 * @enum Status
 * @brief Lifecycle state of a task.
 */
enum class Status { Pending,
                    Completed,
                    Archived,
                    All };

/* ANSI text styles */
static constexpr char const *BOLD = "\033[1m";
static constexpr char const *NOTICE = "\e[1;35m";
static constexpr char const *BLOOD = "\033[31m";
static constexpr char const *GOLD = "\033[33m";
static constexpr char const *GREEN = "\033[32m";
static constexpr char const *RED = "\033[91m";
static constexpr char const *BG_BLACK = "\033[40m";
static constexpr char const *BG_MAGENTA = "\033[45m";
static constexpr char const *RESET = "\033[0m";

/* Symbols */
static constexpr char const *WARN = "⚠️";
static constexpr char const *FAIL = "⛔️";
static constexpr char const *DONE = "✅";

/* Maximum length of title before truncation */
static constexpr size_t TITLE_MAX_LEN = 35;

// Declare the stream-insertion overloads
// std::ostream &operator<<(std::ostream &out, Priority p);

struct Task {
  // ---------- data members ----------
  int id{-1};
  std::string title;
  Priority pr{Priority::Medium};
  Status state{Status::Pending};
  std::optional<ymd> due{std::nullopt};

  /**
   * @brief  Default constructor (invalid id of -1).
   */
  Task() = default;

  /**
   * @brief  Construct a fully‑specified task.
   * @param  id     Unique identifier to assign.
   * @param  title  Text description.
   * @param  pr     Priority level (default: Medium).
   * @param  due    Optional due date.
   */
  Task(int id,
       std::string title,
       Priority pr = Priority::Medium,
       std::optional<ymd> due = std::nullopt);

  /**
   * @brief  Equality comparison (ID, title, priority, state).
   * @param  other  Task to compare against.
   * @return true if key members match.
   */
  bool operator==(const Task &other) const;

  /**
   * @brief  Compute days remaining until the due date.
   * @return Number of days (may be negative if overdue).
   * @throws std::bad_optional_access if no due date is set.
   */
  int days_until_due() const;
};

/* --------------------------- Utility functions --------------------------- */

/**
 * Gets today's date as a year_month_day (YYYY-MM-DD).
 * @return Today's date.
 */
ymd get_today();

/**
 * @brief   Check if a pending task’s due date has passed.
 * @param   task   The Task under test.
 * @param   today  Reference date for comparison.
 * @return  true if task.due < today and state is Pending.
 */
bool is_overdue(const Task &task, const ymd &today);

/**
 * @brief   Truncate a title to fit within TITLE_MAX_LEN.
 * @param   title  Original title.
 * @return  Possibly‑shortened string, with "..." suffix if truncated.
 */
std::string truncate(const std::string &title);

/**
 * @brief   Render a visual priority bar.
 * @param   p  Priority enum to draw.
 * @return  A string of colored blocks.
 */
std::string print_priority(Priority p);
