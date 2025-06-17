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

#include <atomic>
#include <cinttypes>
#include <cstdlib>

#include "osrf_testing_tools_cpp/memory_tools/initialize.hpp"
#include "osrf_testing_tools_cpp/memory_tools/monitoring.hpp"
#include "osrf_testing_tools_cpp/memory_tools/memory_tools_service.hpp"
#include "osrf_testing_tools_cpp/memory_tools/register_hooks.hpp"
#include "osrf_testing_tools_cpp/memory_tools/testing_helpers.hpp"
#include "osrf_testing_tools_cpp/scope_exit.hpp"

#include "./count_function_occurrences_in_backtrace.hpp"
#include "./custom_memory_functions.hpp"
#include "./implementation_monitoring_override.hpp"
#include "./memory_tools_service_factory.hpp"
#include "./print_backtrace.hpp"

namespace osrf_testing_tools_cpp
{
namespace memory_tools
{

void *
custom_malloc(size_t size) noexcept
{
  return custom_malloc_with_original(size, std::malloc, "malloc", true);
}

static inline
void *
custom_malloc_with_original_except(
  size_t size,
  void * (*original_malloc)(size_t),
  const char * replacement_malloc_function_name,
  bool check_recursion)
{
  if (
    // First check that we're initialized to avoid memory fucntions called in count function
    !osrf_testing_tools_cpp::memory_tools::initialized() ||
    // we've recursed, use original directly to avoid infinite loop
    (
      check_recursion &&
      count_function_occurrences_in_backtrace(custom_malloc_with_original_except) > 1
    ) ||
    // monitoring not enabled, use original
    !osrf_testing_tools_cpp::memory_tools::monitoring_enabled())
  {
    return original_malloc(size);
  }

  // prevent dynamic memory calls from within this function from being considered
  ScopedImplementationSection section;

  using osrf_testing_tools_cpp::memory_tools::MemoryToolsServiceFactory;
  MemoryToolsServiceFactory factory(MemoryFunctionType::Malloc, replacement_malloc_function_name);
  osrf_testing_tools_cpp::memory_tools::dispatch_malloc(factory.get_memory_tools_service());

  void * memory = original_malloc(size);
  if (!factory.should_ignore()) {
    using osrf_testing_tools_cpp::memory_tools::malloc_expected;
    uint64_t fw_size = size;
    MALLOC_PRINTF(
      " malloc  (%s) %" PRIu64 " -> %p\n",
      malloc_expected() ? "    expected" : "not expected", fw_size, memory);
    if (factory.should_print_backtrace()) {
      print_backtrace();
    }
  }
  return memory;
}

void *
custom_malloc_with_original(
  size_t size,
  void * (*original_malloc)(size_t),
  const char * replacement_malloc_function_name,
  bool check_recursion) noexcept
{
  try {
    return custom_malloc_with_original_except(
      size,
      original_malloc,
      replacement_malloc_function_name,
      check_recursion);
  } catch (...) {
    fprintf(stderr, "unexpected error in custom malloc\n");
    return nullptr;
  }
}

void *
custom_realloc(void * memory_in, size_t size) noexcept
{
  return custom_realloc_with_original(memory_in, size, std::realloc, "realloc", true);
}

static inline
void *
custom_realloc_with_original_except(
  void * memory_in,
  size_t size,
  void * (*original_realloc)(void *, size_t),
  const char * replacement_realloc_function_name,
  bool check_recursion)
{
  if (
    // First check that we're initialized to avoid memory fucntions called in count function
    !osrf_testing_tools_cpp::memory_tools::initialized() ||
    // we've recursed, use original directly to avoid infinite loop
    (
      check_recursion &&
      count_function_occurrences_in_backtrace(custom_realloc_with_original_except) > 1
    ) ||
    // monitoring not enabled, use original
    !osrf_testing_tools_cpp::memory_tools::monitoring_enabled())
  {
    return original_realloc(memory_in, size);
  }

  // prevent dynamic memory calls from within this function from being considered
  ScopedImplementationSection section;

  using osrf_testing_tools_cpp::memory_tools::MemoryToolsServiceFactory;
  MemoryToolsServiceFactory factory(
    MemoryFunctionType::Realloc,
    replacement_realloc_function_name);
  osrf_testing_tools_cpp::memory_tools::dispatch_realloc(factory.get_memory_tools_service());

  void * memory = original_realloc(memory_in, size);
  if (!factory.should_ignore()) {
    using osrf_testing_tools_cpp::memory_tools::realloc_expected;
    uint64_t fw_size = size;
    MALLOC_PRINTF(
      " realloc (%s) %p %" PRIu64 " -> %p\n",
      realloc_expected() ? "    expected" : "not expected", memory_in, fw_size, memory);
    if (factory.should_print_backtrace()) {
      print_backtrace();
    }
  }
  return memory;
}

void *
custom_realloc_with_original(
  void * memory_in,
  size_t size,
  void * (*original_realloc)(void *, size_t),
  const char * replacement_realloc_function_name,
  bool check_recursion) noexcept
{
  try {
    return custom_realloc_with_original_except(
      memory_in,
      size,
      original_realloc,
      replacement_realloc_function_name,
      check_recursion);
  } catch (...) {
    fprintf(stderr, "unexpected error in custom realloc\n");
    return nullptr;
  }
}

void *
custom_calloc(size_t count, size_t size) noexcept
{
  return custom_calloc_with_original(count, size, std::calloc, "calloc", true);
}

static inline
void *
custom_calloc_with_original_except(
  size_t count,
  size_t size,
  void * (*original_calloc)(size_t, size_t),
  const char * replacement_calloc_function_name,
  bool check_recursion)
{
  if (
    // First check that we're initialized to avoid memory fucntions called in count function
    !osrf_testing_tools_cpp::memory_tools::initialized() ||
    // we've recursed, use original directly to avoid infinite loop
    (
      check_recursion &&
      count_function_occurrences_in_backtrace(custom_calloc_with_original_except) > 1
    ) ||
    // monitoring not enabled, use original
    !osrf_testing_tools_cpp::memory_tools::monitoring_enabled())
  {
    return original_calloc(count, size);
  }

  // prevent dynamic memory calls from within this function from being considered
  ScopedImplementationSection section;

  using osrf_testing_tools_cpp::memory_tools::MemoryToolsServiceFactory;
  MemoryToolsServiceFactory factory(MemoryFunctionType::Calloc, replacement_calloc_function_name);
  osrf_testing_tools_cpp::memory_tools::dispatch_calloc(factory.get_memory_tools_service());

  void * memory = original_calloc(count, size);
  if (!factory.should_ignore()) {
    using osrf_testing_tools_cpp::memory_tools::calloc_expected;
    uint64_t fw_count = count;
    uint64_t fw_size = size;
    uint64_t fw_total = count * size;
    MALLOC_PRINTF(
      " calloc  (%s) %" PRIu64 " (%" PRIu64 " * %" PRIu64 ") -> %p\n",
      calloc_expected() ? "    expected" : "not expected", fw_total, fw_count, fw_size, memory);
    if (factory.should_print_backtrace()) {
      print_backtrace();
    }
  }
  return memory;
}

void *
custom_calloc_with_original(
  size_t count,
  size_t size,
  void * (*original_calloc)(size_t, size_t),
  const char * replacement_calloc_function_name,
  bool check_recursion) noexcept
{
  try {
    return custom_calloc_with_original_except(
      count,
      size,
      original_calloc,
      replacement_calloc_function_name,
      check_recursion);
  } catch (...) {
    fprintf(stderr, "unexpected error in custom calloc\n");
    return nullptr;
  }
}

void
custom_free(void * memory) noexcept
{
  custom_free_with_original(memory, std::free, "free", true);
}

static inline
void
custom_free_with_original_except(
  void * memory,
  void (*original_free)(void *),
  const char * replacement_free_function_name,
  bool check_recursion)
{
  if (
    // First check that we're initialized to avoid memory fucntions called in count function
    !osrf_testing_tools_cpp::memory_tools::initialized() ||
    // we've recursed, use original directly to avoid infinite loop
    (
      check_recursion &&
      count_function_occurrences_in_backtrace(custom_free_with_original_except) > 1
    ) ||
    // monitoring not enabled, use original
    !osrf_testing_tools_cpp::memory_tools::monitoring_enabled())
  {
    original_free(memory);
    return;
  }

  // prevent dynamic memory calls from within this function from being considered
  ScopedImplementationSection section;

  using osrf_testing_tools_cpp::memory_tools::MemoryToolsServiceFactory;
  MemoryToolsServiceFactory factory(MemoryFunctionType::Free, replacement_free_function_name);
  osrf_testing_tools_cpp::memory_tools::dispatch_free(factory.get_memory_tools_service());

  original_free(memory);
  if (!factory.should_ignore()) {
    using osrf_testing_tools_cpp::memory_tools::free_expected;
    MALLOC_PRINTF(
      " free    (%s) %p\n",
      free_expected() ? "    expected" : "not expected", memory);
  }
    if (factory.should_print_backtrace()) {
      print_backtrace();
    }
}

void
custom_free_with_original(
  void * memory,
  void (*original_free)(void *),
  const char * replacement_free_function_name,
  bool check_recursion) noexcept
{
  try {
    custom_free_with_original_except(
      memory,
      original_free,
      replacement_free_function_name,
      check_recursion);
  } catch (...) {
    fprintf(stderr, "unexpected error in custom free\n");
  }
}

}  // namespace memory_tools
}  // namespace osrf_testing_tools_cpp
