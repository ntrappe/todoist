```
Undefined symbols for architecture arm64:
  "TaskManager::size() const", referenced from:
      TaskManagerSize_EmptyMgr_Test::TestBody() in task_test.cpp.o
ld: symbol(s) not found for architecture arm64
```

**FIX:** Check CMakeLists.txt for the file.

**EXPLANATION**: That undefined‑symbol for `TaskManager::size()` almost always means your linker never saw the definition in `task_manager.cpp`. Make sure you're compiling `task_manager.cpp` alongside the other files.


```
Undefined symbols for architecture arm64:
  "testing::internal::MakeAndRegisterTestInfo(std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>>, char const*, char const*, char const*, testing::internal::CodeLocation, void const*, void (*)(), void (*)(), testing::internal::TestFactoryBase*)", referenced from:
      ___cxx_global_var_init in task_test.cpp.o
ld: symbol(s) not found for architecture arm64
clang++: error: linker command failed with exit code 1 (use -v to see invocation)
make[2]: *** [unit_tests] Error 1
make[1]: *** [CMakeFiles/unit_tests.dir/all] Error 2
```

**FIX:** Remove all instances of Google Test managed by package manager.

**EXPLANATION:** You may have multiple instances so the build system doesn't know which to use.


```
using ymd = std::chrono::year_month_day;
                         ~~~~~~~~~~~~~~
```
**FIX:** Compile with `g++ -std=c++20`.

**EXPLANATION:** This feature is only available with C++20 so we need to tell the compiler to use this version instead of the default (C++17 for me). Make sure it's listed in CMakeLists.txt and c_cpp_properties.json for .vscode.


```
warning: unqualified call to 'std::move' [-Wunqualified-std-cast-call]
```

**FIX:** `std::move()`

**EXPLANATION:**  To avoid ambiguity and ensure that the intended std::move is called, it's recommended to always qualify it with the std namespace.


```
strcmp
~~~~~~
```
**FIX:** Use `#include <strings.h>` instead of `#include <string>` in .cpp files.

**EXPLANATION:** Just using `string` fails on GitHub Actions.