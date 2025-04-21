#include "task.hpp"
#include "task_manager.hpp"
#include <gtest/gtest.h>

using namespace std;

const auto now = chrono::system_clock::now();
const auto today = ymd(chrono::floor<chrono::days>(now));

/* --------------------- Simple Tests for Task Object --------------------- */
TEST(TaskCtor, DefaultInvalidId) {
  Task t;
  EXPECT_EQ(t.id, -1);
  EXPECT_EQ(t.state, Status::Pending);
  EXPECT_EQ(t.due, nullopt);
}

TEST(TaskCtor, TaskNoDueDate) {
  // Use the ctor that omits the due‐date parameter
  Task t(1, "PA 1", Priority::Low);
  EXPECT_EQ(t.id, 1);
  EXPECT_EQ(t.title, "PA 1");
  EXPECT_EQ(t.pr, Priority::Low);
  EXPECT_EQ(t.state, Status::Pending);
  // The optional should be disengaged
  EXPECT_FALSE(t.due.has_value());
  // or equivalently:
  EXPECT_EQ(t.due, std::nullopt);
}

TEST(TaskCtor, TaskWithDueDate) {
  // Create a fake due date
  Task t(1, "PA 2", Priority::Low, today);
  EXPECT_EQ(t.id, 1);
  EXPECT_EQ(t.title, "PA 2");
  EXPECT_EQ(t.pr, Priority::Low);
  EXPECT_EQ(t.state, Status::Pending);
  // Now the optional should be engaged
  ASSERT_TRUE(t.due.has_value());
  EXPECT_EQ(*t.due, today);
}

/* ------------------------ Tests for Task Manager ------------------------ */
TEST(TaskManagerSize, EmptyMgr) {
  TaskManager mgr;
  EXPECT_EQ(mgr.size(), 0);
}

TEST(TaskManagerError, BlankTitle) {
  TaskManager mgr;
  EXPECT_THROW(mgr.add("", Priority::Low), invalid_argument);
}

TEST(TaskManagerSize, SingleTask) {
  TaskManager mgr;
  int id = mgr.add("Write code", Priority::High);
  EXPECT_EQ(mgr.size(), 1u);
}

TEST(TaskManagerSize, AddThenDeleteTask) {
  TaskManager mgr;
  int id = mgr.add("PA3", Priority::High);
  EXPECT_EQ(mgr.size(), 1u);
  bool success = mgr.remove(id);
  EXPECT_EQ(success, true);
  EXPECT_EQ(mgr.size(), 0);
}
