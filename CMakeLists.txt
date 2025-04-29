###############################################################################
# CMakeLists.txt — Minimal Google‑Test setup for Apple‑silicon (arm64 only)
###############################################################################
cmake_minimum_required(VERSION 3.20)

# ---------------------------------------------------------------------------
# 1.  Force the generator and compiler to target arm64 before *anything* else.
#    (Remove the CACHE line if you want a universal binary later.)
# ---------------------------------------------------------------------------
set(CMAKE_OSX_ARCHITECTURES arm64 CACHE STRING "" FORCE)

project(MyProject VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)              # Request C++20
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# 2.  Production library (replace with your real sources)
# ->  Because my_lib publishes src/ as a PUBLIC include dir, 
#     every target that links against my_lib (here unit_tests) 
#     inherits that include path.
# ---------------------------------------------------------------------------
add_library(my_lib
  src/task.cpp
  src/task.hpp
  src/task_manager.cpp
  src/task_manager.hpp
  src/task_cli.cpp
  src/task_cli.hpp
)
target_include_directories(my_lib PUBLIC src)

# ---------------------------------------------------------------------------
# 3.  Google Test via FetchContent (downloads & builds for arm64)
# ---------------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)  # no‑op on macOS
FetchContent_MakeAvailable(googletest)              # <<< builds gtest/gmock

# ---------------------------------------------------------------------------
# 4.  Enable CTest *after* gtest is available
# ---------------------------------------------------------------------------
enable_testing()

# ---------------------------------------------------------------------------
# 5.  Unit‑test executable
# ---------------------------------------------------------------------------
file(GLOB TEST_SOURCES tests/*.cpp)

add_executable(unit_tests ${TEST_SOURCES})
target_link_libraries(unit_tests
  PRIVATE
    my_lib
    GTest::gtest         # testing symbols
    GTest::gtest_main    # main() + pulls in gtest again on macOS
)

include(GoogleTest)
gtest_discover_tests(unit_tests)

# ---------------------------------------------------------------------------
# 6.  CBuild the CLI binary
# ---------------------------------------------------------------------------
add_executable(todo src/main.cpp)
target_link_libraries(todo PRIVATE my_lib)

# ---------------------------------------------------------------------------
# 7.  Convenience target: `make run-tests` or `cmake --build . --target run-tests`
# ---------------------------------------------------------------------------
add_custom_target(run-tests
  COMMAND ctest --output-on-failure
  DEPENDS unit_tests
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
