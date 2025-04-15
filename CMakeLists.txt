cmake_minimum_required(VERSION 3.20)
project(Tododist LANGUAGES CXX)

# 2‑line comfort‑boilerplate
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Production library --------------------------------------------------
add_library(my_lib
  src/add.cpp        # implementation
  src/add.h          # header (optional to list, but nice for IDEs)
)

target_include_directories(my_lib PUBLIC src)

# --- GoogleTest via FetchContent -----------------------------------------
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0           # pin a release; update when you wish
)
# Prevent GoogleTest from adding its own gtest/gmock tests
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)   # MSVC runtime tweak
FetchContent_MakeAvailable(googletest)

enable_testing()   # must come *after* FetchContent_MakeAvailable

# --- Test executable -----------------------------------------------------
add_executable(unit_tests tests/add_test.cpp)
target_link_libraries(unit_tests
    PRIVATE
        my_lib           # code under test
        gtest_main       # GoogleTest entry point
)

include(GoogleTest)
gtest_discover_tests(unit_tests)   # ctest auto‑discovers tests
