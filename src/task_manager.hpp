#pragma once
#include "task.hpp"
#include <memory>
#include <queue>
#include <unordered_map>

class TaskManager {
public:
  // Compiler generates "obvious" ctor which means map and queue star
  // TaskManager() = default;
  // Explicit version
  TaskManager() : task_map(), next_id(1) {}

  int add(const std::string &title,
          Priority pr = Priority::Medium,
          std::optional<ymd> due = std::nullopt);
  bool complete(int id);
  bool remove(int id);

  size_t size() const;

private:
  std::unordered_map<int, std::unique_ptr<Task>> task_map;

  int next_id;
};