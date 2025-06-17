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

#ifndef MEMORY_TOOLS__CUSTOM_MEMORY_FUNCTIONS_HPP_
#define MEMORY_TOOLS__CUSTOM_MEMORY_FUNCTIONS_HPP_

#include <cstdio>
#include <cstring>

#include "./safe_fwrite.hpp"

#if defined(__APPLE__)
#include <malloc/malloc.h>
#define MALLOC_PRINTF malloc_printf
#else  // defined(__APPLE__)
#define MALLOC_PRINTF printf
#endif  // defined(__APPLE__)

namespace osrf_testing_tools_cpp
{
namespace memory_tools
{

void *
custom_malloc(size_t size) noexcept;

void *
custom_malloc_with_original(
  size_t size,
  void * (*original_malloc)(size_t),
  const char * replacement_malloc_function_name,
  bool check_recursion) noexcept;

void *
custom_realloc(void * memory_in, size_t size) noexcept;

void *
custom_realloc_with_original(
  void * memory_in,
  size_t size,
  void * (*original_realloc)(void *, size_t),
  const char * replacement_realloc_function_name,
  bool check_recursion) noexcept;

void *
custom_calloc(size_t count, size_t size) noexcept;

void *
custom_calloc_with_original(
  size_t count,
  size_t size,
  void * (*original_calloc)(size_t, size_t),
  const char * replacement_calloc_function_name,
  bool check_recursion) noexcept;

void
custom_free(void * memory) noexcept;

void
custom_free_with_original(
  void * memory,
  void (*original_free)(void *),
  const char * replacement_free_function_name,
  bool check_recursion) noexcept;

}  // namespace memory_tools
}  // namespace osrf_testing_tools_cpp

#endif  // MEMORY_TOOLS__CUSTOM_MEMORY_FUNCTIONS_HPP_
