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

#include "./unix_common.hpp"

#include <cstddef>
#include <mutex>

#include "../custom_memory_functions.hpp"
#include "osrf_testing_tools_cpp/scope_exit.hpp"

static bool g_static_initialization_complete = false;

static std::recursive_mutex* g_memory_function_recursive_mutex;

void
complete_static_initialization()
{
  g_memory_function_recursive_mutex = new std::recursive_mutex;
  g_static_initialization_complete = true;
}

bool &
get_static_initialization_complete()
{
  return g_static_initialization_complete;
}

static size_t g_inside_custom_memory_function = 0;

extern "C"
{

void *
unix_replacement_malloc(size_t size, void *(*original_malloc)(size_t))
{
  // Short-circuit to original function during self-recursion, if static initialization is done.
  if (!g_static_initialization_complete || 0 != g_inside_custom_memory_function) {
    return original_malloc(size);
  }
  std::lock_guard<std::recursive_mutex> lock(*g_memory_function_recursive_mutex);
  g_inside_custom_memory_function++;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    g_inside_custom_memory_function--;
  });

  using osrf_testing_tools_cpp::memory_tools::custom_malloc_with_original;
  return custom_malloc_with_original(size, original_malloc, __func__, false);
}

void *
unix_replacement_realloc(void * memory_in, size_t size, void *(*original_realloc)(void *, size_t))
{
  // Short-circuit to original function during self-recursion, if static initialization is done.
  if (!g_static_initialization_complete || 0 != g_inside_custom_memory_function) {
    return original_realloc(memory_in, size);
  }
  std::lock_guard<std::recursive_mutex> lock(*g_memory_function_recursive_mutex);
  g_inside_custom_memory_function++;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    g_inside_custom_memory_function--;
  });

  using osrf_testing_tools_cpp::memory_tools::custom_realloc_with_original;
  return custom_realloc_with_original(memory_in, size, original_realloc, __func__, false);
}

void *
unix_replacement_calloc(size_t count, size_t size, void *(*original_calloc)(size_t, size_t))
{
  // Short-circuit to original function during self-recursion, if static initialization is done.
  if (!g_static_initialization_complete || 0 != g_inside_custom_memory_function) {
    return original_calloc(count, size);
  }
  std::lock_guard<std::recursive_mutex> lock(*g_memory_function_recursive_mutex);
  g_inside_custom_memory_function++;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    g_inside_custom_memory_function--;
  });

  using osrf_testing_tools_cpp::memory_tools::custom_calloc_with_original;
  return custom_calloc_with_original(count, size, original_calloc, __func__, false);
}

void
unix_replacement_free(void * memory, void (*original_free)(void *))
{
  if (nullptr == memory) {
    return;
  }
  // Short-circuit to original function during self-recursion, if static initialization is done.
  if (!g_static_initialization_complete || 0 != g_inside_custom_memory_function) {
    return original_free(memory);
  }
  std::lock_guard<std::recursive_mutex> lock(*g_memory_function_recursive_mutex);
  g_inside_custom_memory_function++;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    g_inside_custom_memory_function--;
  });

  using osrf_testing_tools_cpp::memory_tools::custom_free_with_original;
  custom_free_with_original(memory, original_free, __func__, false);
}

}  // extern "C"
