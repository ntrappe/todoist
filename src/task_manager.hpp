#pragma once
#include "task.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <queue>
#include <unordered_map>

// Only visible to task_manager.cpp. Helper function for compat.
static std::string to_string(const std::chrono::year_month_day &ymd) {
  return std::format("{:%F}", ymd);
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
  bool complete(int id);

  /**
   * Removes the task entirely.
   *
   * @param id Unique identifier for the task
   * @return False if id unknown. True if successful.
   */
  bool remove(int id);

  void printHeap(std::ostream &out = std::cout) const;

  /**
   * Retrieves and removes the highest‐priority pending Task from the heap.
   * Implements lazy deletion of any tasks that have since been completed/removed.
   *
   * @return Pointer to the next pending Task, or nullptr if no pending tasks.
   */
  Task *next();

  void printTasks(std::ostream &out = std::cout);

  /**
   * Utility: check if task is overdue. Uses a reference so we have a non-null
   * guarantee. Overdue should be a derived-property not status.
   *
   * @param task Reference to a valid Task.
   * @param today Current year_month_day.
   * @return True if overdue. Otherwise, false.
   */
  static bool is_overdue(const Task &task, const ymd &today) {
    return task.due.has_value() && (task.state == Status::Pending) && (task.due.value() < today);
  }

  static ymd get_today() {
    auto now = std::chrono::system_clock::now();
    return std::chrono::floor<std::chrono::days>(now);
  }

  static double effective_score(const Task &task, int threshold) {
    // Map priority to an int [1, 4]
    int base_pr = static_cast<int>(task.pr) + 1;

    // If no due date, just return the base priority
    if (!task.due.has_value())
      return static_cast<double>(base_pr);

    // Compute days remaining, clamped to [0, threshold]
    const auto today = std::chrono::sys_days{get_today()};
    const auto delta = (std::chrono::sys_days{task.due.value()} - today).count();

    // Normalize aging by diving by threshold, clamped to [0 .. 1]
    double aging_norm = static_cast<double>(threshold - delta) / threshold;

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
      // const ymd today = TaskManager::get_today();

      // if (TaskManager::is_overdue(*a, today) && !TaskManager::is_overdue(*b, today)) {
      //   return false;
      // } else if (!TaskManager::is_overdue(*a, today) && TaskManager::is_overdue(*b, today)) {
      //   return true;
      // } else {
      //   return true;
      // }
      return true;
    }
  };

  //  A             B
  //  Overdue       Overdue   -->
  //  Overdue       X         --> A (false)
  //  X             Overdue   --> B (true)
  //  X             X         -->

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

  std::string TASK_TABLE_HEADER = "ID\tPRIORITY\tDUE\t\tTITLE\n";
  std::string TABLE_SEPARATOR = "------------------------------------------------------------------\n";
};