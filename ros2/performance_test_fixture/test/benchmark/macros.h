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

#ifndef BENCHMARK__MACROS_H_
#define BENCHMARK__MACROS_H_

// Provide the compiler with branch prediction information
#ifndef _WIN32
/**
 * \def PERFORMANCE_TEST_FIXTURE_LIKELY
 * Instruct the compiler to optimize for the case where the argument equals 1.
 */
# define PERFORMANCE_TEST_FIXTURE_LIKELY(x) __builtin_expect((x), 1)
/**
 * \def PERFORMANCE_TEST_FIXTURE_UNLIKELY
 * Instruct the compiler to optimize for the case where the argument equals 0.
 */
# define PERFORMANCE_TEST_FIXTURE_UNLIKELY(x) __builtin_expect((x), 0)
#else
/**
 * \def PERFORMANCE_TEST_FIXTURE_LIKELY
 * No op since Windows doesn't support providing branch prediction information.
 */
# define PERFORMANCE_TEST_FIXTURE_LIKELY(x) (x)
/**
 * \def PERFORMANCE_TEST_FIXTURE_UNLIKELY
 * No op since Windows doesn't support providing branch prediction information.
 */
# define PERFORMANCE_TEST_FIXTURE_UNLIKELY(x) (x)
#endif  // _WIN32

#endif  // BENCHMARK__MACROS_H_
