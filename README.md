# CSE Programming Assignment
## “Todoist” – A Heap‑Backed Command‑Line Task Manager

> Due: 23:59 PT, 25 May 2025 <br/>
> Total Points: 100

### 0  Purpose

Design and build a tiny Trello‑like task manager that runs in the terminal.  The project exercises dynamic data structures, file I/O, modern C++ (C++20), and unit testing with Google Test—all skills you have touched in lecture/lab this quarter.

### 1 Learning Objectives

After completing PA‑Todoist you will be able to:

Implement reusable classes with RAII, smart pointers, and const‑correctness.

Combine a binary heap (std::priority_queue) and a hash table (std::unordered_map) to achieve logarithmic scheduling and O(1) lookup.

Persist program state via JSON serialization.

Write Google Test cases that cover CLI behaviour and internal invariants.

Use CMake + FetchContent to manage third‑party dependencies.

### 2 Handout Repository

Clone the starter repo:
```
git clone https://github.com/ntrappe/todoist
```

The **`main`** branch compiles and runs a minimal shell but does nothing useful yet.  Your job is to fill in the TODO blocks until all tests pass and the sample session (Section 7) works.

### 3 Directory Layout

```
├── CMakeLists.txt               # top‑level build script
├── include/
│   ├── task.hpp                 # class Task  (YOU EDIT)
│   ├── task_manager.hpp         # class TaskManager & PriorityPolicy  (YOU EDIT)
│   └── json.hpp                 # single‑header nlohmann/json (do not edit)
├── src/
│   ├── task.cpp                 # Task implementation  (YOU EDIT)
│   ├── task_manager.cpp         # TaskManager implementation  (YOU EDIT)
│   └── main.cpp                 # CLI entry point  (edit optional)
├── tests/
│   ├── unit_task_tests.cpp      # starter unit tests  (optional edit)
│   └── cli_smoke_test.cpp       # runs `./taskmaster` via popen  (optional)
└── data/                        # auto‑generated JSON lives here
```

### 4  Starter Code & TODO Map

Below is a quick index of the functions you must implement.  Each has a `// TODO:` comment in the handout.

| File | Function(s) | Points |
| ---- | ----------- | ------ |
| `task.cpp` | `to_json()`, `from_json()` | 10 |
| `task_manager.cpp` | `add`, `remove`, `edit`, `list`, `next`, `load`, `save`, `rebuild_heap` | 40 | 
| `priority functor` | already done – but you may tweak | — |
| Total | | 50 |

The remaining 50 pts come from testing, style, documentation, and runtime behaviour (see rubric).


### 5  Building & Running

#### 5.1  Configure once
```java
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug   # Generates Ninja/Make files
```

#### 5.2  Every iteration
```java
cmake --build .             # Incremental compile
ctest --output-on-failure   # Runs Google Test suite
./taskmaster                # Manual play‑time (Ctrl‑D to quit)
```

If ctest prints **100 % tests passed** you are very close to full credit.

