#pragma once
#include "task.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <queue>
#include <unordered_map>

static int RECENT = 7;
const int FXN_FAILURE = -1;

using day = std::chrono::day;
using month = std::chrono::month;
using year = std::chrono::year;

// Only visible to task_manager.cpp. Helper function for compat.
static std::string to_string(const std::chrono::year_month_day &ymd) {
  return std::format("{:%F}", ymd);
}

static std::chrono::year_month_day to_ymd(const std::string &in) {
  int y, m, d;
  int r = sscanf(in.c_str(), "%d-%d-%d", &y, &m, &d);
  return ymd{year{y}, month{static_cast<unsigned>(m)}, day{static_cast<unsigned>(d)}};
}

class TaskManager {
public:
  // Compiler generates "obvious" ctor which means map and queue star
  // TaskManager() = default;
  // Explicit version
  TaskManager() : task_map(), next_id(1) {}

  int addTask(const std::string &title,
              Priority pr = Priority::Medium,
              std::optional<ymd> due = std::nullopt);

  /**
   * Marks the task as completed.
   *
   * @param id Unique identifier for the task
   * @return False if id unknown. True if successful.
   */
  bool completeTask(int id);

  /**
   * Removes the task entirely.
   *
   * @param id Unique identifier for the task
   * @return False if id unknown. True if successful.
   */
  bool removeTask(int id);

  /**
   * Archives the task.
   *
   * @param id Unique identifier for the task
   * @return False if id unknown. True if successful.
   */
  bool archiveTask(int id);

  void printTasks(Status filter = Status::Pending);

  // Convenience wrappers
  void printAllTasks() { printTasks(Status::All); }
  void printPendingTasks() { printTasks(Status::Pending); }
  void printCompletedTasks() { printTasks(Status::Completed); }
  void printArchivedTasks() { printTasks(Status::Archived); }

  bool loadFromFile(const std::string &filename = "tasks.json");

  bool saveToFile(const std::string &filename = "tasks.json") const;

  /**
   * Retrieves and removes the highest‐priority pending Task from the heap.
   * Implements lazy deletion of any tasks that have since been completed/removed.
   *
   * @return Pointer to the next pending Task, or nullptr if no pending tasks.
   */
  Task *next();

  static double effective_score(const Task &task, int threshold) {
    // Map priority to an int [1, 4]
    int base_pr = static_cast<int>(task.pr) + 1;

    // If no due date, just return the base priority
    if (!task.due.has_value())
      return static_cast<double>(base_pr);

    // Compute days remaining, clamped to [0, threshold]
    // If a task is overdue (delta < 0), so aging_norm becomes >1.
    const auto today = std::chrono::sys_days{get_today()};
    const auto delta = (std::chrono::sys_days{task.due.value()} - today).count();

    // Normalize aging by diving by threshold, clamped to [0 .. 1]
    double aging_norm = std::clamp(static_cast<double>(threshold - delta) / threshold, 0.0, 1.0);

    // Combine base priority with how soon it's due
    return base_pr + aging_norm;
  }

  /**
   * Utility: number of pending tasks. Maps to MAP not QUEUE because we let
   * pointers go stale in the heap and get lazily popped off.
   *
   * @returns size of unordered map.
   */
  size_t size() const {
    return task_map.size();
  }

private:
  /**
   * No need to guard against nullptr because should never push that to heap
   * anyways.
   */
  struct PriorityCmp {
    bool operator()(const Task *a, const Task *b) const noexcept {
      double score_a = effective_score(*a, RECENT);
      double score_b = effective_score(*b, RECENT);
      return score_a < score_b; // true when a less important than b
    }
  };

  /**
   * Stores each Task. Owns them so uses unique_ptr.
   */
  std::unordered_map<int, std::unique_ptr<Task>> task_map;

  /**
   * Keeps track of which Task should be completed next.
   * Uses raw pointers because points back to objects owned by map.
   */
  std::priority_queue<Task *, std::vector<Task *>, PriorityCmp> task_heap;

  int next_id;

  // Assumes all validation is already done. This is a low-level insertion routine.
  int insertTaskUnchecked(int id, const std::string &title, Priority pr, std::optional<ymd> due);

  /// Format a single task’s due date column.
  std::string formatDue(const Task *task, const ymd &today) const;
};