// Copyright 2018 Open Source Robotics Foundation, Inc.
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

#ifndef OSRF_TESTING_TOOLS_CPP__MEMORY_TOOLS__MONITORING_HPP_
#define OSRF_TESTING_TOOLS_CPP__MEMORY_TOOLS__MONITORING_HPP_

#include "./visibility_control.hpp"

namespace osrf_testing_tools_cpp
{
namespace memory_tools
{

/// Return true if the memory tools hooks are enabled.
/**
 * Hooks will be enabled if either the thread-specific `enable()` or the
 * global `enable_in_all_threads()` have been called.
 *
 * The thread-specific `disable()` will disable hooks in that thread even if
 * `enable_in_all_threads()` has been called.
 * To disable hooks in a thread while still respecting
 * `enable_in_all_threads()`, use the `unset_thread_specific_enable()`
 * function.
 *
 * The thread-specific `enable()` will override the global
 * `disable_in_all_threads()`, even if `disable_in_all_threads()` was called
 * second.
 *
 * For granular control of hooks in threads, avoid `enable_in_all_threads()`.
 *
 * If `install()` has not been called, then this will return false.
 */
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
bool
monitoring_enabled();

/// Enable memory tools hooks in dynamic memory functions, thread-specific.
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
void
enable_monitoring();

/// Disable memory tools hooks in dynamic memory functions, thread-specific.
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
void
disable_monitoring();

/// Disable hooks if not enabled globally, thread-specific and default state.
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
void
unset_thread_specific_monitoring_enable();

/// Enable memory tools hooks in dynamic memory functions, in all threads.
/** \returns the previous state */
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
bool
enable_monitoring_in_all_threads();

/// Disable memory tools hooks in dynamic memory functions, in all threads.
/** \returns the previous state */
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
bool
disable_monitoring_in_all_threads();

}  // namespace memory_tools
}  // namespace osrf_testing_tools_cpp

#endif  // OSRF_TESTING_TOOLS_CPP__MEMORY_TOOLS__MONITORING_HPP_
