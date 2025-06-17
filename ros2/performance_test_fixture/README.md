# performance_test_fixture

This repository contains the source code for the `performance_test_fixture` package.

`performance_test_fixture` provides:
1. A Google Benchmark fixture that leverages memory tools from `osrf_testing_tools_cpp` to record memory allocation statistics
2. A CMake macro for compiling, linking, and configuring benchmarking runs, as well as automatically skipping the tests on platforms where the `osrf_testing_tools_cpp` memory tools are not supported

## Usage

The test fixture can be used just like any other Google Benchmark fixture:
```c++
#include <performance_test_fixture/performance_test_fixture.hpp>

using performance_test_fixture::PerformanceTest;

BENCHMARK_F(PerformanceTest, example_test)(benchmark::State & st)
{
  ...
}
```

The CMake macro can be used in the same way as the rest of the `ament_cmake_test` family of macros:
```cmake
  find_package(performance_test_fixture REQUIRED)
  add_performance_test(example_test test/example_test.cpp)
```

Because it has a large impact on performance, trace messages coming from the memory tools in `osrf_testing_tools_cpp` are suppressed by default.
To enable memory operation trace logging, set the environment variable `PERFORMANCE_TEST_FIXTURE_ENABLE_TRACE=1`.
