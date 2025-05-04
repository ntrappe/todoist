/**
 * @file    task_manager.hpp
 * @brief   Manages creation, storage, and querying of Task objects for the CLI.
 * @author  Nicole Trappe
 * @date    2025-05-04
 *
 * The TaskManager owns all Task instances, provides methods to add,
 * complete, remove, archive, list, save, and load tasks.
 */

#pragma once
#include "task.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <queue>
#include <unordered_map>

// How many days count towards "recent" for aging score.
static constexpr int kRecentThreshold = 7;
// Failure return code for functions.
static constexpr int FXN_FAILURE = -1;

using day = std::chrono::day;
using month = std::chrono::month;
using year = std::chrono::year;

// Helper: format a ymd to ISO string (private to cpp).
static std::string to_string(const std::chrono::year_month_day &ymd) {
  return std::format("{:%F}", ymd);
}

// Helper: parse ISO string to ymd (private to cpp).
static std::chrono::year_month_day to_ymd(const std::string &in) {
  int y, m, d;
  std::sscanf(in.c_str(), "%d-%d-%d", &y, &m, &d);
  return ymd{year{y}, month{static_cast<unsigned>(m)}, day{static_cast<unsigned>(d)}};
}

/**
 * @class TaskManager
 * @brief  Controls tasks: creation, state transitions, persistence, and display.
 */
class TaskManager {
public:
  /**
   * @brief  Initializes empty map and next ID starting at 1.
   *         Note: same as doing TaskManager() = default;
   */
  TaskManager() : task_map(), next_id(1) {}

  /**
   * @brief  Add a new task.
   * @param  title  Non-empty task title.
   * @param  pr     Priority (default Medium).
   * @param  due    Optional due date.
   * @return Task ID on success; FXN_FAILURE on error.
   */
  int addTask(const std::string &title,
              Priority pr = Priority::Medium,
              std::optional<ymd> due = std::nullopt);

  /**
   * @brief  Mark an existing task as completed.
   * @param  id  Identifier of the task.
   * @return True if found and updated, false otherwise.
   */
  bool completeTask(int id);

  /**
   * @brief  Remove an existing task permanently.
   * @param  id  Identifier of the task.
   * @return True if found and removed, false otherwise.
   */
  bool removeTask(int id);

  /**
   * @brief  Archive an existing task (soft delete).
   * @param  id  Identifier of the task.
   * @return True if found and archived, false otherwise.
   */
  bool archiveTask(int id);

  /**
   * @brief  Print tasks filtered by Status.
   * @param  filter  Status enum to select which tasks to show.
   */
  void printTasks(Status filter = Status::Pending);

  // Convenience wrappers
  void printAllTasks() { printTasks(Status::All); }
  void printPendingTasks() { printTasks(Status::Pending); }
  void printCompletedTasks() { printTasks(Status::Completed); }
  void printArchivedTasks() { printTasks(Status::Archived); }

  /**
   * @brief  Load tasks from JSON file.
   * @param  filename  Path to JSON file.
   * @return True if loaded, false if file missing or error.
   */
  bool loadFromFile(const std::string &filename = "tasks.json");

  /**
   * @brief  Save current tasks to JSON file.
   * @param  filename  Path to output file.
   * @return True on success, false otherwise.
   */
  bool saveToFile(const std::string &filename = "tasks.json") const;

  /**
   * @brief   Compute a combined score from priority and due date.
   * @param   task       Reference to Task.
   * @param   threshold  Days window for aging norm.
   * @return  Score: base_pr + aging_norm.
   */
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
   * @brief   Utility: number of tasks in the manager.
   * @return  Size of the internal task_map.
   */
  size_t size() const {
    return task_map.size();
  }

private:
  /**
   * Comparator for priority_queue: higher score = higher priority. No need to guard
   * against nullptr because should never push that to heap anyways.
   */
  struct PriorityCmp {
    bool operator()(const Task *a, const Task *b) const noexcept {
      double score_a = effective_score(*a, kRecentThreshold);
      double score_b = effective_score(*b, kRecentThreshold);
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

  int next_id; //< Next ID to assign.

  /**
   * @brief   Low-level insert that assumes validation is done.
   * @param   id      Task ID.
   * @param   title   Task title.
   * @param   pr      Priority.
   * @param   due     Optional due date.
   * @return  ID on success or FXN_FAILURE.
   */
  int insertTaskUnchecked(int id, const std::string &title, Priority pr, std::optional<ymd> due);

  /**
   * @brief   Format a single task’s due/status column.
   * @param   task   Pointer to the Task.
   * @param   today  Date for overdue calculations.
   * @return  Formatted string fragment.
   */
  std::string formatDue(const Task *task, const ymd &today) const;
};