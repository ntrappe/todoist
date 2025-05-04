# Task Manager++

A simple command-line to-do list manager written in modern C++20. It stores tasks in a JSON file and provides commands to add, archive, list, complete, and remove tasks with priorities and due dates.

## Features
- Add tasks with a title, priority (low, med, high, critical), and optional due date (YYYY-MM-DD).
- List tasks (pending by default), or filter by all/completed/archived.
- Mark tasks as completed.
- Remove tasks.
- Persistent storage to tasks.json using JSON format.

## Setup & Build
Ensure you have CMake and a C++20-compatible compiler installed.
```ruby
# From project root:
rm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

## Running
The compiled binary is located at:
```ruby
build/todo <command>
```
You can run it directly from the build directory.

## Usage
### add
Add a new task.
```ruby
./todo add "TITLE" [--priority <low|med|high|crit>] [--due YYYY-MM-DD]
```
- **title**: Task title (in quotes).
- **priority**: Task priority (default: med).
- **due**: Due date in ISO-8601 format.

### list
List tasks.
```ruby
./todo list [all|completed|archived]
```
No subcommand (or list) shows pending tasks.

- `list all` shows every task.
- `list --completed `shows only completed tasks.
- `list --archived` shows archived tasks (if supported).

### complete
Mark a task as completed.
```ruby
./todo complete <ID>
```

- **ID**: Numeric task identifier.

### remove
Delete a task.
```ruby
./todo remove <ID>
```
- **ID**: Numeric task identifier.

### help
Display help information.
```ruby
./todo help [command]
```
Without arguments, prints overall help.

- `todo help add` prints add-specific usage.

## Examples
```ruby
# Add two tasks
./todo add "Buy milk"
# => Created task #1 ("Buy milk")

./todo add "Submit report" --priority high --due 2025-05-10
# => Created task #2 ("Submit report")

# List pending tasks
./todo list
# =>
# ID   PRIORITY   DUE             TITLE
# ---------------------------------------------
# [1]  med        None            Buy milk
# [2]  high       2025-05-10      Submit report

# Complete the first task
./todo complete 1

# List completed tasks
./todo list completed
# =>
# ID   PRIORITY   DUE             TITLE
# ---------------------------------------------
# [1]  med        None            Buy milk

# List all tasks
./todo list all
# => shows both tasks
```

## Implementation Details
- **Storage:** Tasks are stored in an `unordered_map<int, std::unique_ptr<Task>>` (task_map) for O(1) lookup by ID.
- **Ordering:** A `priority_queue<Task*, vector<Task*>, TaskComparator>` holds raw pointers into task_map so tasks can be listed by due date and priority without copying.
- **Date handling:** Uses C++20’s `<chrono> year_month_day` for dates and helper functions to parse/stringify.
- **Persistence:** `loadFromFile("tasks.json")` and `saveToFile("tasks.json")` wrap JSON serialization.