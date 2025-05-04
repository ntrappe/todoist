#include "task.hpp"
#include "task_cli.hpp"
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
  EXPECT_THROW(mgr.addTask("", Priority::Low), invalid_argument);
}

TEST(TaskManagerSize, SingleTask) {
  TaskManager mgr;
  int id = mgr.addTask("Write code", Priority::High);
  EXPECT_EQ(mgr.size(), 1u);
}

TEST(TaskManagerSize, AddThenDeleteTask) {
  TaskManager mgr;
  int id = mgr.addTask("PA3", Priority::High);
  EXPECT_EQ(mgr.size(), 1u);
  bool success = mgr.removeTask(id);
  EXPECT_EQ(success, true);
  EXPECT_EQ(mgr.size(), 0);
}

TEST(Utilities, IsOverdue) {
  TaskManager mgr;
  Task t(5, "Think harder", Priority::Low, today);
  constexpr auto yesterday = ymd(2020y, chrono::January, 31d);
  EXPECT_EQ(is_overdue(t, today), false);
}

TEST(Utilities, IsDueToday) {
  TaskManager mgr;
  Task t(6, "Think harder", Priority::Low, today);
  EXPECT_EQ(t.days_until_due(), 0);
}

TEST(Utilities, IsDueNextWeek) {
  TaskManager mgr;
  Task t(7, "Read 'Design of Everyday Things'", Priority::Medium, ymd(2028y, chrono::May, 1d));
  EXPECT_GT(t.days_until_due(), 2);
}

TEST(Utilities, IsDuePast) {
  TaskManager mgr;
  Task t(7, "Read 'Design of Everyday Things'", Priority::Medium, ymd(2024y, chrono::December, 31d));
  EXPECT_LT(t.days_until_due(), -1);
}

TEST(Utilities, ParseDate1) {
  TaskCLI cli;
  auto date_opt = cli.parseDate("2025-10-10");
  auto expected = ymd(2025y, chrono::October, 10d);
  EXPECT_EQ(date_opt.value(), expected);
}

TEST(Utilities, ParseDate2) {
  TaskCLI cli;
  auto date_opt = cli.parseDate("2025-01-03");
  auto expected = ymd(2025y, chrono::January, 3d);
  EXPECT_EQ(date_opt.value(), expected);
}

TEST(Utilities, ParseDate3) {
  TaskCLI cli;
  auto date_opt = cli.parseDate("2025-3-31");
  auto expected = ymd(2025y, chrono::March, 31d);
  EXPECT_EQ(date_opt.value(), expected);
}

TEST(Utilities, ParseDateInvalid) {
  TaskCLI cli;
  auto date_opt = cli.parseDate("2025-200-31");
  auto expected = ymd(2025y, chrono::March, 31d);
  EXPECT_EQ(date_opt, nullopt);
}

TEST(Utilities, ParsePriority1) {
  TaskCLI cli;
  auto pr = cli.parsePriority("low");
  EXPECT_EQ(pr, Priority::Low);
}

TEST(Utilities, ParsePriority2) {
  TaskCLI cli;
  auto pr = cli.parsePriority("medium");
  EXPECT_EQ(pr, Priority::Medium);
}

TEST(Utilities, ParsePriority3) {
  TaskCLI cli;
  auto pr = cli.parsePriority("yeet");
  EXPECT_EQ(pr, Priority::Medium);
}