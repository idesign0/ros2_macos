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

#include <cstdlib>
#include <string>

#include "./macros.h"
#include "performance_test_fixture/performance_test_fixture.hpp"

namespace
{

constexpr int kEnablePerformanceTracking = 1;
constexpr int kDisablePerformanceTracking = 0;

}  // namespace

class PerformanceTestFixture : public performance_test_fixture::PerformanceTest
{
public:
  void SetUp(benchmark::State & state)
  {
    if (kEnablePerformanceTracking != state.range(0) &&
      kDisablePerformanceTracking != state.range(0))
    {
      std::string message =
        std::string("Invalid enable/disable value: ") + std::to_string(state.range(0));
      state.SkipWithError(message.c_str());
    }
    if (kEnablePerformanceTracking == state.range(0)) {
      performance_test_fixture::PerformanceTest::SetUp(state);
    }
  }

  void TearDown(benchmark::State & state)
  {
    if (kEnablePerformanceTracking == state.range(0)) {
      performance_test_fixture::PerformanceTest::TearDown(state);
    }
  }
};

BENCHMARK_DEFINE_F(PerformanceTestFixture, benchmark_pause_resume_timing)(
  benchmark::State & state)
{
  for (auto _ : state) {
    auto start = std::chrono::steady_clock::now();
    state.PauseTiming();
    state.ResumeTiming();
    auto duration =
      std::chrono::duration_cast<std::chrono::duration<double>>(
      std::chrono::steady_clock::now() - start);
    state.SetIterationTime(duration.count());
  }
}

BENCHMARK_DEFINE_F(PerformanceTestFixture, benchmark_pause_resume_measurements)(
  benchmark::State & state)
{
  for (auto _ : state) {
    // Manual timing is used here because state.PauseTiming() is called from within.
    auto start = std::chrono::steady_clock::now();
    PERFORMANCE_TEST_FIXTURE_PAUSE_MEASUREMENTS(state, {});
    auto duration =
      std::chrono::duration_cast<std::chrono::duration<double>>(
      std::chrono::steady_clock::now() - start);
    state.SetIterationTime(duration.count());
  }
}

BENCHMARK_REGISTER_F(PerformanceTestFixture, benchmark_pause_resume_timing)
->UseManualTime()
->ArgNames({"Enable Performance Tracking"})
->Arg(kEnablePerformanceTracking)
->Arg(kDisablePerformanceTracking);

BENCHMARK_REGISTER_F(PerformanceTestFixture, benchmark_pause_resume_measurements)
->UseManualTime()
->ArgNames({"Enable Performance Tracking"})
->Arg(kEnablePerformanceTracking);

BENCHMARK_MAIN();
