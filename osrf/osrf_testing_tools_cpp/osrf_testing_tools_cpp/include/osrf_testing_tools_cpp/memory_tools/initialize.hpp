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

#ifndef OSRF_TESTING_TOOLS_CPP__MEMORY_TOOLS__INITIALIZE_HPP_
#define OSRF_TESTING_TOOLS_CPP__MEMORY_TOOLS__INITIALIZE_HPP_

#include "./visibility_control.hpp"

namespace osrf_testing_tools_cpp
{
namespace memory_tools
{

/// Return true if initialized, otherwise false.
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
bool
initialized();

/// Install hooks into dynamic memory functions.
/**
 * May do nothing on some platforms, as installing the hooks happens during
 * linking rather than at runtime.
 * If necessary, however, this function will replace the dynamic memory hooks.
 *
 * This function, and it's counterpart `uninitialize()`, are meant to
 * be called infrequently, if you want to disable the checked temporarily, use
 * thread-specific `enable_monitoring()`/`disable_monitoring()` functions or
 * the global functions
 * `enable_monitoring_in_all_threads()`/`disable_monitoring_in_all_threads()`.
 *
 * May throw an exception if unable to install memory allocation hooks.
 * On platforms where memory tools are not supported, nothing will happen.
 *
 * More may need to be done in order to have memory tools installed, like using
 * the `LD_PRELOAD` environment variable on Linux.
 * See other documentation for details.
 *
 * This function may allocate memory and then free it to test that the hooks
 * have been installed.
 *
 * \throws std::runtime_error if hooks cannot be installed.
 */
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
void
initialize();

/// Disable, or uninstall if possible, hooks in the dynamic memory functions.
/**
 * This function will  attempt to remove the hooks in the dynamic memory
 * functions, but will return false if it cannot.
 * Even if the hooks cannot be uninstalled, calling this function will prevent
 * the hooks from being called.
 * Calling this function before `initialize()` does nothing, and
 * false will be returned.
 *
 * Also resets all state and clears all callbacks.
 *
 * \returns true if actually uninstalled or not installed yet, false otherwise.
 */
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
bool
uninitialize();

}  // namespace memory_tools
}  // namespace osrf_testing_tools_cpp

#endif  // OSRF_TESTING_TOOLS_CPP__MEMORY_TOOLS__INITIALIZE_HPP_
