#pragma once
#include "task.hpp"
#include "task_manager.hpp"
#include <iostream>

const int ADD_MIN_ARGS = 3;
const int TITLE_IDX = 2;
const int TASK_ID_IDX = 2;

class TaskCLI {
public:
  TaskCLI() {}

  /**
   * @brief   Entry point for CLI commands.
   * @param   argc  Argument count.
   * @param   argv  Argument vector.
   * @return  EXIT_SUCCESS on success; EXIT_FAILURE on error.
   */
  int run(int argc, char *argv[]);

  /**
   * @brief   Parse a priority level from a string. Uses string_view bc it’s just a
   *          pointer and a length, perfect for read‑only parsing.
   * @param   txt  Text (e.g. "low", "med", "high", "critical").
   * @return  The matching Priority enum; defaults to Priority::Medium if unrecognized.
   */
  Priority parsePriority(std::string_view txt);

  /**
   * @brief   Parse an ISO‑8601 date string (YYYY‑MM‑DD).
   * @param   in  Input string to parse.
   * @return  Optional<year_month_day> containing the date if valid; nullopt otherwise.
   */
  std::optional<ymd> parseDate(const std::string &in);

private:
  /**
   * @brief   Print usage/help for the `add` command.
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

  void printListHelp() {
    std::cout << NOTICE << "List tasks\n\nUsage:" << RESET << std::endl;
    std::cout << "./todo list [--all] [--completed] [--pending] "
                 "\n\n"
                 "List tasks, optionally filtered by status."
                 "\n\n";
    std::cout << NOTICE << "Options:" << RESET << std::endl;
    std::cout << "  --all            Show all tasks\n"
                 "  --completed      Show only completed tasks\n"
                 "  --pending        Show only pending tasks (default)\n"
                 "\n\n";
    std::cout << NOTICE << "Examples:" << RESET << std::endl;
    std::cout << "  ./todo list\n"
                 "  ./todo list --completed\n"
                 "  ./todo list -a\n"
              << std::endl;
  }

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

  void printHelp() {
    std::cout << NOTICE << "Task Manager CLI\n\nUsage:" << RESET << std::endl;
    std::cout << "./todo <command> [options]\n\n";

    std::cout << NOTICE << "Commands:" << RESET << std::endl;
    std::cout << "  add        Add a new task\n"
                 "  archive     Delete a task\n"
                 "  complete   Mark a task as completed\n"
                 "  help       Show this help, or detailed help for a subcommand\n"
                 "  list       List tasks (pending by default; see ’all’, ’completed’, ’archived’)\n"
                 "  remove     Delete a task\n\n";

    std::cout << "Run './todo help <command>' for more information on a specific command.\n";
  }

  /**
   * @brief   Parse arguments for the `add` command. Note that we're using references
   *          instead of raw pointers to ensure we pass valid vars (no nullptr).
   * @param   argc     Argument count.
   * @param   argv     Argument vector.
   * @param   title    (out) Receives the task title.
   * @param   pr       (out) Receives the task priority.
   * @param   due      (out) Receives the optional due date.
   * @return  EXIT_SUCCESS if parsing succeeded; EXIT_FAILURE otherwise.
   */
  int parseAdd(int argc, char *argv[],
               std::string &title,
               Priority &pr,
               std::optional<ymd> &due);
};