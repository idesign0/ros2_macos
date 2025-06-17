## osrf_testing_tools_cpp Repository

This repository contains testing tools for C++, and is used in OSRF projects.

### osrf_testing_tools_cpp

The `osrf_testing_tools_cpp` folder is where all the actually useful code lives within a cmake project of the same name.

Any CMake based project can use `osrf_testing_tools_cpp` by using CMake's `find_package()` and then using the various CMake macros, C++ headers and libraries, and other tools as described below.

This package requires C++11 only, but should also work with C++14 and C++17.

#### Contents

There are several useful components in the `osrf_testing_tools_cpp` project, and they're briefly described below.

##### Googletest

There are a few CMake macros which can provide access to one of a few versions of `googletest` so that it can be used within your own projects without having to include a copy in each of them.

###### Example

```cmake
include(CTest)
if(BUILD_TESTING)
  find_package(osrf_testing_tools_cpp REQUIRED)
  osrf_testing_tools_cpp_require_googletest(VERSION_GTE 1.8)  # ensures target gtest_main exists

  add_executable(my_gtest ...)
  target_link_libraries(my_gtest gtest_main ...)
endif()
```

You can also list the available versions with `osrf_testing_tools_cpp_get_googletest_versions()`.

This provides access to both "gtest" and "gmock" headers.

##### add_test with Environment Variables

There is a CMake macro `osrf_testing_tools_cpp_add_test()` which acts much like CMake/CTest's `add_test()` macro, but also has some additional arguments `ENV`, `APPEND_ENV` (for `PATH`-like environment variables), and an OS agnostic `APPEND_LIBRARY_DIRS`.

This is accomplished with an executable (writtin in C++) which gets put in from of your test executable with additional arguments for any environment variable changes you desire.
This "test runner" executable modifies the environment and then executes your test command as specified.

###### Example

```cmake
osrf_testing_tools_cpp_add_test(test_my_executable
  COMMAND "$<TARGET_FILE:my_executable>" arg1 --arg2
  ENV FOO=bar
  APPEND_ENV PATH=/some/additional/path/bin
  APPEND_LIBRARY_DIRS /some/additional/library/path
)
```

This might result in CTest output something like this (this example output is from macOS, hence the `DYLD_LIBRARY_PATH` versus `LD_LIBRARY_PATH` on Linux or `PATH` on Windows):

```
test 1
    Start 1: test_my_executable

1: Test command: /some/path/install/osrf_testing_tools_cpp/lib/osrf_testing_tools_cpp/test_runner "--env" "FOO=bar" "--append-env" "PATH=/some/additional/path/bin" "DYLD_LIBRARY_PATH=/some/additional/library/path" "--" "/some/path/$
build/my_cmake_project/test_example_memory_tools_gtest" "arg1" "--arg2"
```

##### memory_tools

This API lets you intercept calls to dynamic memory calls like `malloc` and `free`, and provides some convenience functions for differentiating between expected and unexpected calls to dynamic memory functions.

Right now it only works on Linux and macOS.

###### Example Test

Here's a simple, googletest based example of how it is used:

```c++
#include <cstdlib>
#include <thread>

#include <gtest/gtest.h>
#include <gtest/gtest-spi.h>

#include "osrf_testing_tools_cpp/memory_tools/memory_tools.hpp"
#include "osrf_testing_tools_cpp/scope_exit.hpp"

void my_first_function()
{
  void * some_memory = std::malloc(1024);
  // .. do something with it
  std::free(some_memory);
}

int my_second_function(int a, int b)
{
  return a + b;
}

TEST(TestMemoryTools, test_example) {
  // you must initialize memory tools, but uninitialization is optional
  osrf_testing_tools_cpp::memory_tools::initialize();
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    osrf_testing_tools_cpp::memory_tools::uninitialize();
  });

  // create a callback for "unexpected" mallocs that does a non-fatal gtest
  // failure and then register it with memory tools
  auto on_unexpected_malloc =
    [](osrf_testing_tools_cpp::memory_tools::MemoryToolsService & service) {
      ADD_FAILURE() << "unexpected malloc";
      // this will cause a bracktrace to be printed for each unexpected malloc
      service.print_backtrace();
    };
  osrf_testing_tools_cpp::memory_tools::on_unexpected_malloc(on_unexpected_malloc);

  // at this point, you'll still not get callbacks, since monitoring is not enabled
  // so calling either user function should not fail
  my_first_function();
  EXPECT_EQ(my_second_function(1, 2), 3);

  // enabling monitoring will allow checking to begin, but the default state is
  // that dynamic memory calls are expected, so again either function will pass
  osrf_testing_tools_cpp::memory_tools::enable_monitoring();
  my_first_function();
  EXPECT_EQ(my_second_function(1, 2), 3);

  // if you then tell memory tools that malloc is unexpected, then it will call
  // your above callback, at least until you indicate malloc is expected again
  EXPECT_NONFATAL_FAILURE({
    EXPECT_NO_MALLOC({
      my_first_function();
    });
  }, "unexpected malloc");
  // There are also explicit begin/end functions if you need variables to leave the scope
  osrf_testing_tools_cpp::memory_tools::expect_no_malloc_begin();
  int result = my_second_function(1, 2);
  osrf_testing_tools_cpp::memory_tools::expect_no_malloc_end();
  EXPECT_EQ(result, 3);

  // enable monitoring only works in the current thread, but you can enable it for all threads
  osrf_testing_tools_cpp::memory_tools::enable_monitoring_in_all_threads();
  std::thread t1([]() {
    EXPECT_NONFATAL_FAILURE({
      EXPECT_NO_MALLOC({
        my_first_function();
      });
    }, "unexpected malloc");
    osrf_testing_tools_cpp::memory_tools::expect_no_malloc_begin();
    int result = my_second_function(1, 2);
    osrf_testing_tools_cpp::memory_tools::expect_no_malloc_end();
    EXPECT_EQ(result, 3);
  });
  t1.join();

  // disabling monitoring in all threads should not catch the malloc in my_first_function()
  osrf_testing_tools_cpp::memory_tools::disable_monitoring_in_all_threads();
  osrf_testing_tools_cpp::memory_tools::enable_monitoring();
  std::thread t2([]() {
    EXPECT_NO_MALLOC({
      my_first_function();
    });
    osrf_testing_tools_cpp::memory_tools::expect_no_malloc_begin();
    int result = my_second_function(1, 2);
    osrf_testing_tools_cpp::memory_tools::expect_no_malloc_end();
    EXPECT_EQ(result, 3);
  });
  t2.join();
}
```

In order for memory tools to work properly and intercept dynamic memory calls in all libraries, the library pre-load environment variable needs to be used, `LD_PRELOAD` on Linux and `DYLD_INSERT_LIBRARIES` on macOS.
You can use the above `osrf_testing_tools_cpp_add_test()` macro to set this environment variable before running any tests, unless you have another way to do so.

For example, here is some CMake code that will build a test and add a CTest for it that properly sets the library pre-load environment variable on all support OS's:

```cmake
include(CTest)
if(BUILD_TESTING)
  find_package(osrf_testing_tools_cpp REQUIRED)
  osrf_testing_tools_cpp_require_googletest(VERSION_GTE 1.8)  # ensures target gtest_main exists

  add_executable(test_example_memory_tools_gtest test/test_example_memory_tools.cpp)
  target_link_libraries(test_example_memory_tools_gtest
    gtest_main
    osrf_testing_tools_cpp::memory_tools
  )
  get_target_property(extra_env_vars
    osrf_testing_tools_cpp::memory_tools LIBRARY_PRELOAD_ENVIRONMENT_VARIABLE)
  osrf_testing_tools_cpp_add_test(test_example_memory_tools
    COMMAND "$<TARGET_FILE:test_example_memory_tools_gtest>"
    ENV ${extra_env_vars}
  )
endif()
```

The `LIBRARY_PRELOAD_ENVIRONMENT_VARIABLE` property of the `osrf_testing_tools_cpp::memory_tools` target contains the appropriate environment variable, but is not used automatically.
It must be set before the test is run, and note that the `$ENV{}` syntax in CMake is not sufficient, as that only affects the environment variables at configure time and not at test time.

You can see this code in action in the `test_osrf_testing_tools_cpp` example CMake project.

##### Various C++ Utilities

###### std::variant Helper

The `std::variant` feature isn't available until C++17, but using the [mpark/variant](https://github.com/mpark/variant) project the `osrf_testing_tools_cpp` project provides access to a C++11 and C++14 compatible version via the `osrf_testing_tools_cpp/variant.hpp` header.

In the case that you're using C++17, the normal `std::variant` is used.
See cppreference.com for documentation:

http://en.cppreference.com/w/cpp/utility/variant

###### Scope Exit Idiom

The `osrf_testing_tools_cpp/scope_exit.hpp` header contains a class and a macro to make it easy to perform some code at the exit of the current scope, which is often useful for setting up automatic resource cleanup.

For example:

```c++
{
  auto foo = new int;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    delete foo;
  });
  if (condition) {
    throw std::runtime_error("error that might have caused foo to leak");
  }
}
```

In the above example, `foo` will be deleted when the scope ends.

###### Demangling C++ Symbols

These functions are in `osrf_testing_tools_cpp/demangle.hpp` and can be used to generate human readable symbols (on supported) platforms from the result of `typeid` or a mangled string (gathered from a backtrace or from the output of `nm`, for example).

You can use it with a type directly:

```c++
#include <cstdio>
#include <map>

#include <osrf_testing_tools_cpp/demangle.hpp>

int main(void) {
  std::map<int, float> some_map;
  printf("%s\n", osrf_testing_tools_cpp::demangle(some_map).c_str());
  auto type_name = typeid(int).name();
  printf("%s\n", osrf_testing_tools_cpp::demangle_str(type_name).c_str());
  return 0;
}

```

This program might output (on supported platforms):

```
std::__1::map<int, float, std::__1::less<int>, std::__1::allocator<std::__1::pair<int const, float> > >
int
```

### test_osrf_testing_tools_cpp

The `test_osrf_testing_tools_cpp` folder is an example or test cmake project, which demonstrates how to use it, including `find_package()`'ing it and using `memory_tools` to implement a test.
