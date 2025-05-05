/**
 * @file    task_cli.hpp
 * @brief   Command-line interface for TaskMaster++. Parses user input,
 *          dispatches commands to TaskManager, and handles persistence.
 * @author  Nicole Trappe
 * @date    2025-05-04
 */

#pragma once
#include "task.hpp"
#include "task_manager.hpp"
#include <iostream>

// Minimum number of arguments required for commands that need a parameter
static constexpr int ADD_MIN_ARGS = 3;
// Minimum number of arguments to include subcommand
static constexpr int MIN_ARGS = 2;
// Index of the title argument in argv for `add`
static constexpr int TITLE_IDX = 2;
// Index of the ID argument in argv for ID-based commands
static constexpr int TASK_ID_IDX = 2;

class TaskCLI {
public:
  TaskCLI() {}

  /**
   * @brief   Main entry point for the CLI. Loads tasks, interprets commands,
   *          executes them, and saves on state-changing operations.
   * @param   argc  Number of command-line arguments.
   * @param   argv  Array of argument C-strings.
   * @return  EXIT_SUCCESS on success; EXIT_FAILURE on error or invalid usage.
   */
  int run(int argc, char *argv[]);

  /**
   * @brief   Convert a flag string into a Priority enum.
   * @param   txt  Priority text ("low", "med", "high", "critical").
   * @return  Matching Priority; defaults to Priority::Medium on failure.
   */
  Priority parsePriority(std::string_view txt);

  /**
   * @brief   Parse an ISO-8601 date (YYYY-MM-DD) into ymd.
   * @param   in  Date string to parse.
   * @return  Optional<ymd> if valid; nullopt otherwise.
   */
  std::optional<ymd> parseDate(const std::string &in);

private:
  /**
   * @brief   Display detailed help for the `add` command.
   */
  void printAddHelp() {
    std::cout << NOTICE << "Add a task\n\nUsage:" << RESET << std::endl;
    std::cout << "./todo add \"TITLE\" [--priority <low|med|high|crit>] [--due YYYY-MM-DD]"
                 "\n\n"
                 "Add a new task with the given TITLE.\n"
                 "If no options are supplied the task is created with medium priority and no due date.\n"
                 "\n";
    std::cout << NOTICE << "Options:" << RESET << std::endl;
    std::cout << "  --priority  <low|med|high|crit>   Set task priority (default: med)\n"
                 "  --due       YYYY-MM-DD            Due date in ISO-8601 format\n"
                 "\n";
    std::cout << NOTICE << "Examples:" << RESET << std::endl;
    std::cout << "  ./todo add \"File taxes\" --priority high --due 2025-04-15\n"
                 "  ./todo add \"Buy groceries\" --due 2025-05-02\n"
                 "  ./todo add \"Read a book\"       # title only\n"
              << std::endl; // flush and keep prompt on its own line
  }

  /**
   * @brief   Display detailed help for the `list` command.
   */
  void printListHelp() {
    std::cout << NOTICE << "List tasks\n\nUsage:" << RESET << std::endl;
    std::cout << "./todo list [--all] [--completed] [--pending] [--archived] "
                 "\n\n"
                 "List tasks, optionally filtered by status."
                 "\n\n";
    std::cout << NOTICE << "Options:" << RESET << std::endl;
    std::cout << "  --all            Show all tasks\n"
                 "  --archived       Show only archived tasks\n"
                 "  --completed      Show only completed tasks\n"
                 "  --pending        Show only pending tasks (default)\n"
                 "\n\n";
    std::cout << NOTICE << "Examples:" << RESET << std::endl;
    std::cout << "  ./todo list\n"
                 "  ./todo list --completed\n"
                 "  ./todo list -r\n"
              << std::endl;
  }

  /**
   * @brief   Display detailed help for the `complete` command.
   */
  void printCompleteHelp() {
    std::cout << NOTICE << "Complete a task\n\nUsage:" << RESET << std::endl;
    std::cout << "./todo complete <ID>"
                 "\n\n"
                 "Mark the task with the given ID as completed.\n"
                 "\n";
    std::cout << NOTICE << "Example:" << RESET << std::endl;
    std::cout << "  ./todo complete 3\n"
              << std::endl; // flush and keep prompt on its own line
  }

  /**
   * @brief   Display detailed help for the `archive` command.
   */
  void printArchiveHelp() {
    std::cout << NOTICE << "Archive a task\n\nUsage:" << RESET << std::endl;
    std::cout << "./todo archive <ID>"
                 "\n\n"
                 "Mark the task with the given ID as archived.\n"
                 "\n";
    std::cout << NOTICE << "Example:" << RESET << std::endl;
    std::cout << "  ./todo archive 2\n"
              << std::endl; // flush and keep prompt on its own line
  }

  /**
   * @brief   Display detailed help for the `remove` command.
   */
  void printRemoveHelp() {
    std::cout << NOTICE << "Remove a task\n\nUsage:" << RESET << std::endl;
    std::cout << "./todo remove <ID>"
                 "\n\n"
                 "Delete the task with the given ID.\n"
                 "\n";
    std::cout << NOTICE << "Example:" << RESET << std::endl;
    std::cout << "  ./todo remove 5\n"
              << std::endl; // flush and keep prompt on its own line
  }

  /**
   * @brief   Display overall CLI usage and subcommands.
   */
  void printHelp() {
    std::cout << NOTICE << "Task Manager CLI\n\nUsage:" << RESET << std::endl;
    std::cout << "./todo <command> [options]\n\n";

    std::cout << NOTICE << "Commands:" << RESET << std::endl;
    std::cout << "  add        Add a new task\n"
                 "  archive    Mark a task as archived\n"
                 "  complete   Mark a task as completed\n"
                 "  help       Show this help, or detailed help for a subcommand\n"
                 "  list       List tasks (pending by default)\n"
                 "  remove     Delete a task\n\n";

    std::cout << "Run './todo help <command>' for more information on a specific command.\n";
  }

  /**
   * @brief   Parse flags and values for the `add` command.
   * @param   argc   Argument count.
   * @param   argv   Argument vector.
   * @param   title  (out) Parsed task title.
   * @param   pr     (out) Parsed priority.
   * @param   due    (out) Parsed due date.
   * @return  EXIT_SUCCESS on success; EXIT_FAILURE on invalid flags or missing values.
   */
  int parseAdd(int argc, char *argv[],
               std::string &title,
               Priority &pr,
               std::optional<ymd> &due);
};