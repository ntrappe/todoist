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
                      High };
enum class Status { Pending,
                    Completed,
                    Archived };

using ymd = std::chrono::year_month_day;

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
};
