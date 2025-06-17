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

#ifndef OSRF_TESTING_TOOLS_CPP__MEMORY_TOOLS__MEMORY_TOOLS_SERVICE_HPP_
#define OSRF_TESTING_TOOLS_CPP__MEMORY_TOOLS__MEMORY_TOOLS_SERVICE_HPP_

#include "./stack_trace.hpp"
#include "./visibility_control.hpp"

namespace osrf_testing_tools_cpp
{
namespace memory_tools
{

/// Forward declaration of private MemoryToolsService factory class.
class MemoryToolsServiceFactory;

class MemoryToolsServiceImpl;

enum class MemoryFunctionType
{
  Malloc,
  Realloc,
  Calloc,
  Free,
};

/// Service injected in to user callbacks which allow them to control behavior.
/**
 * This is a Service (in the terminology of the dependency injection pattern)
 * which is given to user callbacks which allows the user to control the
 * behavior of the memory tools test suite, like whether or not to log the
 * occurrence, print a backtrace, or make a gtest assert.
 *
 * Creation of the MemoryToolsService is prevented publicly, so to enforce the
 * functions are only called from within a memory tools callback.
 * Similarly, the instance given by the callback parameter is only valid until
 * the callback returns, therefore it should not be stored globally and called
 * later.
 *
 * The default behavior is that a single line message about the event is
 * printed to stderr and a gtest assertion fails if between the
 * `assert_no_*_{begin,end}()` function calls for a given memory operation.
 * However, you can avoid both behaviors by calling ignore(), or just avoid
 * the gtest failure by calling ignore_but_log().
 *
 * You can have it include a backtrace to see where the memory-related call is
 * originating from by calling print_backtrace() before returning.
 *
 * The user's callback, if set, will occur before memory operations.
 * The gtest failure, if not ignored, will also occur before memory operations.
 * Any logging activity occurs after the memory operations.
 */
struct MemoryToolsService
{
  OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
  virtual ~MemoryToolsService();

  /// Return the memory function type.
  OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
  MemoryFunctionType
  get_memory_function_type() const;

  /// Return the memory function type as a string.
  OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
  const char *
  get_memory_function_type_str() const;

  /// If called last, no log message for the dynamic memory event will be logged.
  OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
  void
  ignore();

  /// If called last, a log message for the dynamic memory event will be logged.
  OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
  void
  unignore();

  /// Adds a backtrace to the log message.
  /** Repeated calls do nothing, and only prints if a log is also printed. */
  OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
  void
  print_backtrace();

  /// Returns a stack trace object for introspection.
  /**
   * Pointer should not be used after MemoryToolsService is out of scope.
   * Will return nullptr if a stack trace is not available, e.g. on Windows.
   */
  OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
  StackTrace *
  get_stack_trace();

  /// Return the address of the source memory function.
  OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
  const char *
  get_source_function_name() const;

protected:
  explicit MemoryToolsService(
    MemoryFunctionType memory_function_type,
    const char * source_function_name);

  std::shared_ptr<MemoryToolsServiceImpl> impl_;

  friend MemoryToolsServiceFactory;
};

}  // namespace memory_tools
}  // namespace osrf_testing_tools_cpp

#endif  // OSRF_TESTING_TOOLS_CPP__MEMORY_TOOLS__MEMORY_TOOLS_SERVICE_HPP_
