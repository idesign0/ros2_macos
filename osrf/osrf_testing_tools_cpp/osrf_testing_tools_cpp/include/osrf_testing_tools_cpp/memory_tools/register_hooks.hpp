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

#ifndef OSRF_TESTING_TOOLS_CPP__MEMORY_TOOLS__REGISTER_HOOKS_HPP_
#define OSRF_TESTING_TOOLS_CPP__MEMORY_TOOLS__REGISTER_HOOKS_HPP_

#include <functional>

#include "../variant_helper.hpp"
#include "./memory_tools_service.hpp"
#include "./visibility_control.hpp"

namespace osrf_testing_tools_cpp
{
namespace memory_tools
{

/// Signature for callback on a call to a dynamic memory function.
using MemoryToolsCallback = std::function<void(MemoryToolsService &)>;
/// Simpler signature.
using MemoryToolsSimpleCallback = std::function<void()>;

using AnyMemoryToolsCallback = std::variant<
  MemoryToolsCallback,
  MemoryToolsSimpleCallback,
  std::nullptr_t>;

/// Register a hook to be called on malloc().
/**
 * Some dynamic memory calls are expected (the implementation of memory tools),
 * so only "unexpected" calls to dynamic memory functions cause the hooks to
 * be called.
 *
 * Replaces any existing hook, pass nullptr to "unregister".
 *
 * \throws std::bad_alloc if allocating storage for the callback fails
 */
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
void
on_malloc(AnyMemoryToolsCallback callback);

/// Get the current on_malloc callback if set, otherwise null.
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
AnyMemoryToolsCallback
get_on_malloc();

/// Call the registered callback for malloc.
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
void
dispatch_malloc(MemoryToolsService & service);

/// Register a hook to be called on realloc().
/**
 * Replaces any existing hook, pass nullptr to "unregister".
 *
 * \throws std::bad_alloc if allocating storage for the callback fails
 */
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
void
on_realloc(AnyMemoryToolsCallback callback);

/// Get the current on_realloc callback if set, otherwise null.
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
AnyMemoryToolsCallback
get_on_realloc();

/// Call the registered callback for realloc.
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
void
dispatch_realloc(MemoryToolsService & service);

/// Register a hook to be called on calloc().
/**
 * Replaces any existing hook, pass nullptr to "unregister".
 *
 * \throws std::bad_alloc if allocating storage for the callback fails
 */
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
void
on_calloc(AnyMemoryToolsCallback callback);

/// Get the current on_calloc callback if set, otherwise null.
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
AnyMemoryToolsCallback
get_on_calloc();

/// Call the registered callback for calloc.
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
void
dispatch_calloc(MemoryToolsService & service);

/// Register a hook to be called on free().
/**
 * Replaces any existing hook, pass nullptr to "unregister".
 *
 * \throws std::bad_alloc if allocating storage for the callback fails
 */
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
void
on_free(AnyMemoryToolsCallback callback);

/// Get the current on_free callback if set, otherwise null.
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
AnyMemoryToolsCallback
get_on_free();

/// Call the registered callback for free.
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
void
dispatch_free(MemoryToolsService & service);

}  // namespace memory_tools
}  // namespace osrf_testing_tools_cpp

#endif  // OSRF_TESTING_TOOLS_CPP__MEMORY_TOOLS__REGISTER_HOOKS_HPP_
