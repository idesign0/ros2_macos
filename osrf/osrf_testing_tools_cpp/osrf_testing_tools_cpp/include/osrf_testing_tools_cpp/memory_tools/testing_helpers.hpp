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

#ifndef OSRF_TESTING_TOOLS_CPP__MEMORY_TOOLS__TESTING_HELPERS_HPP_
#define OSRF_TESTING_TOOLS_CPP__MEMORY_TOOLS__TESTING_HELPERS_HPP_

#include "./register_hooks.hpp"
#include "./visibility_control.hpp"

namespace osrf_testing_tools_cpp
{
namespace memory_tools
{

/// Register callback to be called when malloc is unexpected.
/**
 * Uses and is overridden by on_malloc().
 *
 * \sa expect_no_malloc_begin()
 * \sa expect_no_malloc_end()
 */
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
void
on_unexpected_malloc(AnyMemoryToolsCallback callback);

/// Call corresponding callback if the given statements call malloc.
#define EXPECT_NO_MALLOC(statements) \
  osrf_testing_tools_cpp::memory_tools::expect_no_malloc_begin(); \
  statements; \
  osrf_testing_tools_cpp::memory_tools::expect_no_malloc_end()

/// Return true if malloc is expected.
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
bool
malloc_expected();

/// Toggle calling of callback on from within malloc.
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
void
expect_no_malloc_begin();

/// Toggle calling of callback off from within malloc.
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
void
expect_no_malloc_end();

/// Register callback to be called when realloc is unexpected.
/**
 * Uses and is overridden by on_realloc().
 *
 * \sa expect_no_realloc_begin()
 * \sa expect_no_realloc_end()
 */
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
void
on_unexpected_realloc(AnyMemoryToolsCallback callback);

/// Call corresponding callback if the given statements call realloc.
#define EXPECT_NO_REALLOC(statements) \
  osrf_testing_tools_cpp::memory_tools::expect_no_realloc_begin(); \
  statements; \
  osrf_testing_tools_cpp::memory_tools::expect_no_realloc_end()

/// Return true if realloc is expected.
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
bool
realloc_expected();

/// Toggle calling of callback on from within realloc.
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
void
expect_no_realloc_begin();

/// Toggle calling of callback off from within realloc.
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
void
expect_no_realloc_end();

/// Register callback to be called when calloc is unexpected.
/**
 * Uses and is overridden by on_calloc().
 *
 * \sa expect_no_calloc_begin()
 * \sa expect_no_calloc_end()
 */
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
void
on_unexpected_calloc(AnyMemoryToolsCallback callback);

/// Call corresponding callback if the given statements call calloc.
#define EXPECT_NO_CALLOC(statements) \
  osrf_testing_tools_cpp::memory_tools::expect_no_calloc_begin(); \
  statements; \
  osrf_testing_tools_cpp::memory_tools::expect_no_calloc_end()

/// Return true if calloc is expected.
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
bool
calloc_expected();

/// Toggle calling of callback on from within calloc.
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
void
expect_no_calloc_begin();

/// Toggle calling of callback off from within calloc.
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
void
expect_no_calloc_end();

/// Register callback to be called when free is unexpected.
/**
 * Uses and is overridden by on_free().
 *
 * \sa expect_no_free_begin()
 * \sa expect_no_free_end()
 */
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
void
on_unexpected_free(AnyMemoryToolsCallback callback);

/// Call corresponding callback if the given statements call free.
#define EXPECT_NO_FREE(statements) \
  osrf_testing_tools_cpp::memory_tools::expect_no_free_begin(); \
  statements; \
  osrf_testing_tools_cpp::memory_tools::expect_no_free_end()

/// Return true if free is expected.
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
bool
free_expected();

/// Toggle calling of callback on from within free.
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
void
expect_no_free_begin();

/// Toggle calling of callback off from within free.
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
void
expect_no_free_end();

/// Start checking for unexpected memory operations.
#define EXPECT_NO_MEMORY_OPERATIONS_BEGIN() \
  osrf_testing_tools_cpp::memory_tools::expect_no_malloc_begin(); \
  osrf_testing_tools_cpp::memory_tools::expect_no_realloc_begin(); \
  osrf_testing_tools_cpp::memory_tools::expect_no_calloc_begin(); \
  osrf_testing_tools_cpp::memory_tools::expect_no_free_begin()

/// Stop checking for unexpected memory operations.
#define EXPECT_NO_MEMORY_OPERATIONS_END() \
  osrf_testing_tools_cpp::memory_tools::expect_no_malloc_end(); \
  osrf_testing_tools_cpp::memory_tools::expect_no_realloc_end(); \
  osrf_testing_tools_cpp::memory_tools::expect_no_calloc_end(); \
  osrf_testing_tools_cpp::memory_tools::expect_no_free_end()

/// Call corresponding callback assert on any memory operation.
#define EXPECT_NO_MEMORY_OPERATIONS(statements) \
  EXPECT_NO_MEMORY_OPERATIONS_BEGIN(); \
  statements; \
  EXPECT_NO_MEMORY_OPERATIONS_END()

}  // namespace memory_tools
}  // namespace osrf_testing_tools_cpp

#endif  // OSRF_TESTING_TOOLS_CPP__MEMORY_TOOLS__TESTING_HELPERS_HPP_
