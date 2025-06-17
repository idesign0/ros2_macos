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

#ifndef RTTEST__RTTEST_H_
#define RTTEST__RTTEST_H_

#include <time.h>
#include <sched.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

// rttest can have one instance per thread!
struct rttest_params
{
  size_t iterations;
  struct timespec update_period;
  size_t sched_policy;
  int sched_priority;
  size_t stack_size;
  uint64_t prefault_dynamic_size;

  // TODO(dirk-thomas) currently this pointer is never deallocated or copied
  // so whatever value is being assigned must stay valid forever
  char * filename;
};

struct rttest_results
{
  // Max iteration that this result describes
  size_t iteration;
  int64_t min_latency;
  int64_t max_latency;
  double mean_latency;
  double latency_stddev;

  size_t minor_pagefaults;
  size_t major_pagefaults;
};

/// \brief Initialize rttest with arguments
/// \param[in] argc Size of argument vector
/// \param[out] argv Argument vector
/// \return Error code to propagate to main
int rttest_read_args(int argc, char ** argv);

/// \brief Initialize rttest. Preallocate the sample buffer, store user
/// parameters, lock memory if necessary
/// Not real time safe.
/// \param[in] iterations How many iterations to spin for
/// \param[in] update_period Time interval representing the spin period
/// \param[in] sched_policy Scheduling policy, e.g. round robin, FIFO
/// \param[in] sched_priority The thread priority
/// \param[in] stack_size How many bytes to prefault when
/// rttest_prefault_stack() is called.
/// \param[in] prefault_dynamic_size How many bytes to prefault on the heap.
/// \param[in] filename Name of the file to save results to.
/// \return Error code to propagate to main
int rttest_init(
  size_t iterations, struct timespec update_period,
  size_t sched_policy, int sched_priority, size_t stack_size,
  uint64_t prefault_dynamic_size, char * filename);

/// \brief Fill an rttest_params struct with the current rttest params.
/// \param[in] params Reference to the struct to fill in
/// \return Error code
int rttest_get_params(struct rttest_params * params);

/// \brief Create a new rttest instance for a new thread.
/// The thread's parameters are based on the first thread that called rttest_init.
/// To be called directly after the user creates the thread.
/// \return Error code to propagate to main
int rttest_init_new_thread();

/// \brief Spin at the specified wakeup period for the specified number of
/// iterations.
/// \param[in] user_function Function pointer to execute on wakeup
/// \param[in] args Arguments to the function
/// \return Error code to propagate to main
int rttest_spin(void * (*user_function)(void *), void * args);

// TODO(jacquelinekay) better signature for user function
/// \brief Spin at the specified wakeup period for the specified number of
/// iterations. rttest_spin will attempt to time the execution of user_function
/// according to update_period.
/// Call this after everything has been initialized.
/// \param[in] user_function Function pointer to execute on wakeup.
/// \param[in] args Arguments to the function
/// \param[in] update_period Update period (overrides param read in rttest_init)
/// \param[in] iterations Iterations (overrides param read in rttest_init)
/// \return Error code to propagate to main
int rttest_spin_period(
  void * (*user_function)(void *), void * args,
  const struct timespec * update_period, const size_t iterations);

/// \brief Schedule a function call based on the start time, update period,
/// and the iteration of the spin call.
/// The statistics of the wakeup will be collected as the 'ith' entry in the data buffer.
/// \param[in] user_function Function pointer to execute on interrupt.
/// \param[in] update_period
/// \param[out] Error code to propagate to main function.
/// \return Error code to propagate to main
int rttest_spin_once_period(
  void * (*user_function)(void *), void * args,
  const struct timespec * start_time,
  const struct timespec * update_period, const size_t i);

/// \brief Schedule a function call based on the start time, update period,
/// and the iteration of the spin call.
/// The statistics of the wakeup will be collected as the 'ith' entry in the data buffer.
/// TODO: implement asynchronous scheduling/logging
/// \param[in] user_function Function pointer to execute on interrupt.
/// \param[out] Error code to propagate to main function.
/// \return Error code to propagate to main
int rttest_spin_once(
  void * (*user_function)(void *), void * args,
  const struct timespec * start_time, const size_t i);

/// \brief Lock currently paged memory using mlockall.
/// \return Error code to propagate to main
int rttest_lock_memory();

/// \brief Prefault the stack.
/// \param[in] stack_size The size of the stack
/// \return Error code to propagate to main
int rttest_prefault_stack_size(const size_t stack_size);

/// \brief Prefault the stack using default stack size.
/// \return Error code to propagate to main
int rttest_prefault_stack();

/// \brief Commit a pool of dynamic memory based on the memory already cached
/// by this process by checking the number of pagefaults.
/// \return Error code to propagate to main
int rttest_lock_and_prefault_dynamic();

/// \brief Set the priority and scheduling policy for this thread (pthreads)
/// \param[in] sched_priority The scheduling priority. Max is 99.
/// \param[in] policy The scheduling policy (FIFO, Round Robin, etc.)
/// \return Error code to propagate to main
int rttest_set_sched_priority(const size_t sched_priority, const int policy);

/// \brief Set the priority and scheduling policy for this thread using
/// default parameters.
/// \return Error code to propagate to main
int rttest_set_thread_default_priority();

/// \brief Get rusage (pagefaults) and record in the sample buffer at a
/// particular iteration
/// \param[in] i Index at which to store the pagefault information.
/// \return Error code to propagate to main
int rttest_get_next_rusage(size_t i);

/// \brief Calculate statistics and fill the given results struct.
/// \param[in] results The results struct to fill with statistics.
/// \return Error code if results struct is NULL or if calculations invalid
int rttest_calculate_statistics(struct rttest_results * results);

/// \brief Get accumulated statistics
/// \return Error code if results struct is NULL
int rttest_get_statistics(struct rttest_results * results);

/// \brief Get latency sample at the given iteration.
/// \param[in] iteration Iteration of the test to get the sample from
/// \param[out] The resulting sample: time in nanoseconds between the expected
/// wakeup time and the actual wakeup time
int rttest_get_sample_at(const size_t iteration, int64_t * sample);

/// \brief Write the sample buffer to a file.
/// \return Error code to propagate to main
int rttest_write_results();

/// \brief Write the sample buffer to a file.
/// \param[in] Filename to store the sample buffer; overrides default param.
/// \return Error code to propagate to main
int rttest_write_results_file(char * filename);

/// \brief Free memory and cleanup
/// \return Error code to propagate to main
int rttest_finish();

/// \brief Check if the rttest instance is running (ready to collect data or
/// collecting data)
/// \return 0 if the instance is not running, 1 if it is running.
int rttest_running();

#ifdef __cplusplus
}
#endif

#endif  // RTTEST__RTTEST_H_
