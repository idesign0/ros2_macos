// Copyright 2015 Open Source Robotics Foundation, Inc.
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

#include <sys/resource.h>
#include <string>

#include <array>
#include "gtest/gtest.h"

#include "rttest/rttest.h"
#include "rttest/utils.hpp"

void * test_callback(void * args)
{
  size_t * counter = static_cast<size_t *>(args);
  /*ASSERT_TRUE(counter != NULL);*/
  *counter = *counter + 1;
  printf("Test callback: %zu\n", *counter);

  return 0;
}

// check that arguments are read from the commandline and accessed via get_params
TEST(TestApi, read_args_get_params) {
  int argc = 15;
  char * argv[] = {
    const_cast<char *>("test_data"),
    const_cast<char *>("-i"), const_cast<char *>("4321"),
    const_cast<char *>("-u"), const_cast<char *>("50us"),
    const_cast<char *>("-t"), const_cast<char *>("42"),
    const_cast<char *>("-s"), const_cast<char *>("fifo"),
    const_cast<char *>("-m"), const_cast<char *>("100kb"),
    const_cast<char *>("-d"), const_cast<char *>("100kb"),
    const_cast<char *>("-f"), const_cast<char *>("foo.txt")
  };
  EXPECT_EQ(0, rttest_read_args(argc, argv));
  struct rttest_params params;
  EXPECT_EQ(-1, rttest_get_params(NULL));
  EXPECT_EQ(0, rttest_get_params(&params));

  EXPECT_EQ(params.iterations, 4321u);
  EXPECT_EQ(params.update_period.tv_sec, 0);
  EXPECT_EQ(params.update_period.tv_nsec, 50000);
  EXPECT_EQ(params.sched_priority, 42);
  EXPECT_EQ(params.sched_policy, static_cast<uint>(SCHED_FIFO));
  EXPECT_EQ(params.stack_size, 102400u);
  EXPECT_EQ(params.prefault_dynamic_size, 102400u);
  EXPECT_EQ(strcmp(params.filename, "foo.txt"), 0);
  EXPECT_EQ(0, rttest_finish());
}

TEST(TestApi, read_args_update_period_over_32bit) {
  // 4294967305 equals "static_cast<uint64_t>(UINT32_MAX) + 10"
  // but avoid calculation to keep test simple
  uint64_t update_period = 4294967305;
  std::string update_period_str(std::to_string(update_period) + "ns");
  struct timespec t;
  uint64_to_timespec(update_period, &t);

  int argc = 3;
  char * argv[] = {
    const_cast<char *>("test_data"),
    const_cast<char *>("-u"), const_cast<char *>(update_period_str.c_str()),
  };

  EXPECT_EQ(0, rttest_read_args(argc, argv));
  struct rttest_params params;
  EXPECT_EQ(0, rttest_get_params(&params));
  EXPECT_EQ(params.update_period.tv_sec, t.tv_sec);
  EXPECT_EQ(params.update_period.tv_nsec, t.tv_nsec);
  EXPECT_EQ(0, rttest_finish());
}

TEST(TestApi, init) {
  struct timespec update_period;
  update_period.tv_sec = 123;
  update_period.tv_nsec = 456;
  size_t stack_size = 100;
  uint64_t prefault_dynamic_size = 100;

  EXPECT_EQ(
    0, rttest_init(
      4321, update_period, SCHED_FIFO, 42, stack_size,
      prefault_dynamic_size, const_cast<char *>("foo.txt")));
  struct rttest_params params;
  EXPECT_EQ(0, rttest_get_params(&params));

  EXPECT_EQ(params.iterations, 4321u);
  EXPECT_EQ(params.update_period.tv_sec, update_period.tv_sec);
  EXPECT_EQ(params.update_period.tv_nsec, update_period.tv_nsec);
  EXPECT_EQ(params.sched_priority, 42);
  EXPECT_EQ(params.sched_policy, static_cast<uint>(SCHED_FIFO));
  EXPECT_EQ(params.stack_size, stack_size);
  EXPECT_EQ(params.prefault_dynamic_size, prefault_dynamic_size);
  EXPECT_EQ(strcmp(params.filename, "foo.txt"), 0);

  EXPECT_EQ(0, rttest_finish());
}

TEST(TestApi, spin_once) {
  // initialize with all default arguments
  struct timespec update_period;
  update_period.tv_sec = 0;
  update_period.tv_nsec = 1000000;
  EXPECT_EQ(0, rttest_init(1, update_period, SCHED_RR, 80, 0, 0, NULL));

  size_t counter = 0;

  struct timespec start_time;
  clock_gettime(CLOCK_MONOTONIC, &start_time);
  EXPECT_EQ(0, rttest_spin_once(test_callback, static_cast<void *>(&counter), &start_time, 0));
  // Block for longer than the update period
  clock_nanosleep(CLOCK_MONOTONIC, 0, &update_period, NULL);
  EXPECT_EQ(counter, 1u);

  EXPECT_EQ(0, rttest_finish());
}

TEST(TestApi, spin) {
  struct timespec update_period;
  update_period.tv_sec = 0;
  update_period.tv_nsec = 1000000;
  size_t iterations = 100;
  rttest_init(iterations, update_period, SCHED_RR, 80, 0, 0, NULL);
  size_t counter = 0;
  EXPECT_EQ(0, rttest_spin(test_callback, static_cast<void *>(&counter)));
  EXPECT_EQ(counter, iterations);
  EXPECT_EQ(0, rttest_finish());
}

TEST(TestApi, get_statistics) {
  struct rusage usage;
  size_t initial_min_pgflts;
  size_t initial_maj_pgflts;
  size_t runtime_min_pgflts;
  size_t runtime_maj_pgflts;

  struct timespec update_period, start_time;
  update_period.tv_sec = 0;
  update_period.tv_nsec = 1000000;
  EXPECT_EQ(0, rttest_init(50, update_period, SCHED_FIFO, 80, 0, 0, NULL));
  size_t counter = 0;
  getrusage(RUSAGE_SELF, &usage);
  initial_min_pgflts = usage.ru_minflt;
  initial_maj_pgflts = usage.ru_majflt;
  // Coarse-grained latency test: Insert an artificial pause before spin_once
  clock_gettime(CLOCK_MONOTONIC, &start_time);
  for (size_t i = 0; i < 50; ++i) {
    clock_nanosleep(CLOCK_MONOTONIC, 0, &update_period, NULL);
    EXPECT_EQ(0, rttest_spin_once(test_callback, static_cast<void *>(&counter), &start_time, i));
  }

  getrusage(RUSAGE_SELF, &usage);
  runtime_min_pgflts = usage.ru_minflt - initial_min_pgflts;
  runtime_maj_pgflts = usage.ru_majflt - initial_maj_pgflts;
  struct rttest_results results;
  EXPECT_EQ(-1, rttest_get_statistics(NULL));
  EXPECT_EQ(0, rttest_get_statistics(&results));
  EXPECT_EQ(runtime_min_pgflts, results.minor_pagefaults);
  EXPECT_EQ(runtime_maj_pgflts, results.major_pagefaults);

  // The average latency should be at least as large as the artificial pause
  double expected_latency = static_cast<double>(timespec_to_uint64(&update_period));
  EXPECT_GE(results.mean_latency, expected_latency);

  EXPECT_EQ(0, rttest_finish());
}

TEST(TestApi, running) {
  struct timespec update_period, start_time;
  clock_gettime(CLOCK_MONOTONIC, &start_time);
  update_period.tv_sec = 0;
  update_period.tv_nsec = 1000;
  EXPECT_EQ(0, rttest_running());
  EXPECT_EQ(0, rttest_init(50, update_period, SCHED_FIFO, 80, 0, 0, NULL));
  EXPECT_EQ(1, rttest_running());
  size_t i = 0;
  size_t counter = 0;
  EXPECT_EQ(0, rttest_spin_once(test_callback, static_cast<void *>(&counter), &start_time, i));
  EXPECT_EQ(1, rttest_running());
  EXPECT_EQ(0, rttest_finish());
  EXPECT_EQ(0, rttest_running());
}

TEST(TestApi, timespec_to_uint64) {
  // failed values in 32bit OS (#94)
  struct timespec t;
  t.tv_sec = 363464;
  t.tv_nsec = 232837182;
  uint64_t v = 363464232837182;

  EXPECT_EQ(v, timespec_to_uint64(&t));

  struct timespec t2;
  uint64_to_timespec(v, &t2);
  EXPECT_EQ(t.tv_sec, t2.tv_sec);
  EXPECT_EQ(t.tv_nsec, t2.tv_nsec);
}
