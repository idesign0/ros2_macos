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

#include <benchmark/benchmark.h>
#include <cstdlib>
#include <string>

#include "./macros.h"

// This does not make use of PauseTiming or ResumeTiming because timing is very short for these
// benchmarks. However, they should allow for comparisons to the other benchmark_malloc_realloc
static void benchmark_on_malloc(benchmark::State & state)
{
  const size_t alloc_size = state.range(0);
  if (alloc_size < 1) {
    state.SkipWithError("Size for allocation is too small for this test");
  }
  for (auto _ : state) {
    void * ptr = std::malloc(alloc_size);
    if (PERFORMANCE_TEST_FIXTURE_UNLIKELY(nullptr == ptr)) {
      state.SkipWithError("Malloc failed to malloc");
    }
    std::free(ptr);
    benchmark::DoNotOptimize(ptr);
    benchmark::ClobberMemory();
  }
}

static void benchmark_on_calloc(benchmark::State & state)
{
  const size_t alloc_size = state.range(0);
  if (alloc_size < 1) {
    state.SkipWithError("Size for allocation is too small for this test");
  }
  for (auto _ : state) {
    void * ptr = std::calloc(1, alloc_size);
    if (PERFORMANCE_TEST_FIXTURE_UNLIKELY(nullptr == ptr)) {
      state.SkipWithError("Calloc failed to calloc");
    }
    std::free(ptr);
    benchmark::DoNotOptimize(ptr);
    benchmark::ClobberMemory();
  }
}

static void benchmark_on_realloc(benchmark::State & state)
{
  const int64_t malloc_size = state.range(0);
  if (malloc_size < 1) {
    state.SkipWithError("Size for allocation is too small for this test");
  }
  const size_t realloc_size = state.range(1);
  if (realloc_size < 1) {
    state.SkipWithError("Size for reallocation is too small for this test");
  }
  for (auto _ : state) {
    void * ptr = std::malloc(malloc_size);
    if (PERFORMANCE_TEST_FIXTURE_UNLIKELY(nullptr == ptr)) {
      state.SkipWithError("Malloc failed to malloc");
    }
    void * realloc_ptr = std::realloc(ptr, realloc_size);
    if (PERFORMANCE_TEST_FIXTURE_UNLIKELY(nullptr == realloc_ptr)) {
      std::free(ptr);
      state.SkipWithError("Malloc failed to realloc");
    }
    std::free(realloc_ptr);
    benchmark::DoNotOptimize(realloc_ptr);
    benchmark::ClobberMemory();
  }
}

// Malloc sizes Range from 1 to 2^27 each time multiplying by 16
BENCHMARK(benchmark_on_malloc)->ArgNames({"Alloc Size"})->RangeMultiplier(16)->Range(1, 1 << 28);

BENCHMARK(benchmark_on_calloc)->ArgNames({"Alloc Size"})->RangeMultiplier(16)->Range(1, 1 << 28);

// Three types of realloc tests, one where malloc is smaller than realloc, one where they are
// the same, and one where malloc is larger than realloc. Realloc size ranges from 1 to 2^27
// each time multiplying by 32.
static void realloc_args(benchmark::internal::Benchmark * b)
{
  for (int64_t malloc_adjustment = -1; malloc_adjustment <= 1; ++malloc_adjustment) {
    for (int64_t realloc_shift = 0; realloc_shift < 32; realloc_shift += 8) {
      const int64_t malloc_shift = realloc_shift + malloc_adjustment;
      if (malloc_shift < 0) {
        continue;
      }
      b->Args({1ll << malloc_shift, 1ll << realloc_shift});
    }
  }
}
BENCHMARK(benchmark_on_realloc)->ArgNames({"Alloc Size", "Realloc Size"})->Apply(realloc_args);
