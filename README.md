# CSE Programming Assignment
### “Todoist” – A Heap‑Backed Command‑Line Task Manager

> Due: 23:59 PT, 25 April 2025 <br/>
> Total Points: 100

## 0  Purpose

Design and build a tiny Trello‑like task manager that runs in the terminal.  The project exercises dynamic data structures, file I/O, modern C++ (C++20), and unit testing with Google Test—all skills you have touched in lecture/lab this quarter.

## 1 Learning Objectives

After completing PA‑Todoist you will be able to:

- Implement reusable classes with RAII, smart pointers, and const‑correctness.
- Combine a binary heap (std::priority_queue) and a hash table (std::unordered_map) to achieve logarithmic scheduling and O(1) lookup.
- Persist program state via JSON serialization.
- Write Google Test cases that cover CLI behaviour and internal invariants.
- Use CMake + FetchContent to manage third‑party dependencies.

## 2 Handout Repository

Clone the starter repo:
```ruby
git clone https://github.com/ntrappe/todoist
```

The **`main`** branch compiles and runs a minimal shell but does nothing useful yet.  Your job is to fill in the TODO blocks until all tests pass and the sample session (Section 7) works.

## 3 Directory Layout

```html
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

## 4  Starter Code & TODO Map

Below is a quick index of the functions you must implement.  Each has a `// TODO:` comment in the handout.

| File | Function(s) | Points |
| ---- | ----------- | ------ |
| `task.cpp` | `to_json()`, `from_json()` | 10 |
| `task_manager.cpp` | `add`, `remove`, `edit`, `list`, `next`, `load`, `save`, `rebuild_heap` | 40 | 
| `priority functor` | already done – but you may tweak | — |
| Total | | 50 |

The remaining 50 pts come from testing, style, documentation, and runtime behaviour (see rubric).


## 5  Building & Running

### 5.1  Configure once
```java
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug   # Generates Ninja/Make files
```

### 5.2  Every iteration
```java
cmake --build .             # Incremental compile
ctest --output-on-failure   # Runs Google Test suite
./taskmaster                # Manual play‑time (Ctrl‑D to quit)
```

If ctest prints **100 % tests passed** you are very close to full credit.

## 6 Provided Tests

### 6.1 

### 6.2

## 7 Sample Interactive Session
```java
$ ./taskmaster
Loaded 0 tasks from data/tasks.json
> add "Finish HCI paper" --due 2025-02-28 --priority H --tags school,writing
Added task 1.
> next
[1] Finish HCI paper (H) — due 2025-02-28 — tags: school,writing
> quit
Saved 1 task to data/tasks.json
```

Your output does not need to match spacing exactly, but **content and ordering must be identical**.

## 8 Grading Rubric (100 pts)

| Category | Points |
| -------- | ------ |
| Functional correctness (Req FR‑1 → FR‑8) | 45 |
| Implementation quality (RAII, heap logic, aging) | 20 |
| Unit tests (≥ 10, good coverage) | 15 |
| Style & documentation (clang‑format) | 10 |
| Build/packaging (CMake, no warnings) | 10 |

## 9  Submission

1. Push all commits to main on GitLab before the deadline.
2. Tag your final commit v1.0: `git tag v1.0 && git push --tags`
3. Verify the CI pipeline turns green.  A failing pipeline incurs a 0 pt build penalty.

## 10 FAQ

- **Can I use Boost / fmt / … ?** Yes, but you must vendor them and update CMake; TA’s aren’t responsible for external libs.
- **What JSON library is allowed?** We bundled nlohmann/json.hpp; feel free to replace it.
- **Do I need to write a custom heap?** No, std::priority_queue is fine.
- **How big should the codebase be?** At least 350 logical lines you wrote (tests excluded).
