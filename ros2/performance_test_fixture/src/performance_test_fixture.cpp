// Copyright 2020 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "performance_test_fixture/performance_test_fixture.hpp"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <vector>

#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

namespace performance_test_fixture
{

PerformanceTest::PerformanceTest()
: suppress_memory_tools_logging(true), are_allocation_measurements_active(false)
{
  const char * performance_test_fixture_enable_trace = getenv(
    "PERFORMANCE_TEST_FIXTURE_ENABLE_TRACE");
  if (nullptr != performance_test_fixture_enable_trace &&
    strcmp("1", performance_test_fixture_enable_trace) == 0)
  {
    suppress_memory_tools_logging = false;
  }

  ComputeStatistics(
    "max",
    [](const std::vector<double> & v) -> double {
      return *(std::max_element(std::begin(v), std::end(v)));
    });
  ComputeStatistics(
    "min",
    [](const std::vector<double> & v) -> double {
      return *(std::min_element(std::begin(v), std::end(v)));
    });
}

void PerformanceTest::SetUp(benchmark::State &)
{
  reset_heap_counters();

  osrf_testing_tools_cpp::memory_tools::initialize();
  osrf_testing_tools_cpp::memory_tools::on_unexpected_calloc(
    std::function<void(osrf_testing_tools_cpp::memory_tools::MemoryToolsService &)>(
      std::bind(&PerformanceTest::on_alloc, this, std::placeholders::_1)));
  osrf_testing_tools_cpp::memory_tools::on_unexpected_malloc(
    std::function<void(osrf_testing_tools_cpp::memory_tools::MemoryToolsService &)>(
      std::bind(&PerformanceTest::on_alloc, this, std::placeholders::_1)));
  osrf_testing_tools_cpp::memory_tools::on_unexpected_realloc(
    std::function<void(osrf_testing_tools_cpp::memory_tools::MemoryToolsService &)>(
      std::bind(&PerformanceTest::on_alloc, this, std::placeholders::_1)));
  osrf_testing_tools_cpp::memory_tools::enable_monitoring();
  osrf_testing_tools_cpp::memory_tools::expect_no_calloc_begin();
  osrf_testing_tools_cpp::memory_tools::expect_no_malloc_begin();
  osrf_testing_tools_cpp::memory_tools::expect_no_realloc_begin();
}

void PerformanceTest::TearDown(benchmark::State & state)
{
  osrf_testing_tools_cpp::memory_tools::expect_no_calloc_end();
  osrf_testing_tools_cpp::memory_tools::expect_no_malloc_end();
  osrf_testing_tools_cpp::memory_tools::expect_no_realloc_end();

  if (osrf_testing_tools_cpp::memory_tools::is_working()) {
    state.counters["heap_allocations"] = benchmark::Counter(
      static_cast<double>(allocation_count),
      benchmark::Counter::kAvgIterations);
  }

  osrf_testing_tools_cpp::memory_tools::disable_monitoring();
  osrf_testing_tools_cpp::memory_tools::uninitialize();
}

void PerformanceTest::on_alloc(
  osrf_testing_tools_cpp::memory_tools::MemoryToolsService & service
)
{
  // Refraining from using an if-branch here in performance-critical code
  allocation_count += static_cast<size_t>(are_allocation_measurements_active);

  if (suppress_memory_tools_logging) {
    service.ignore();
  }
}

void PerformanceTest::reset_heap_counters()
{
  allocation_count = 0;
  are_allocation_measurements_active = true;
}

void PerformanceTest::set_are_allocation_measurements_active(bool value)
{
  are_allocation_measurements_active = value;
}


}  // namespace performance_test_fixture
