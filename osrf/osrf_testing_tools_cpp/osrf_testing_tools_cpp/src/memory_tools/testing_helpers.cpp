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

#include "./dispatch_callback.hpp"
#include "osrf_testing_tools_cpp/memory_tools/register_hooks.hpp"
#include "osrf_testing_tools_cpp/memory_tools/testing_helpers.hpp"

namespace osrf_testing_tools_cpp
{
namespace memory_tools
{

static std::atomic<bool> g_malloc_unexpected(false);
static std::atomic<bool> g_realloc_unexpected(false);
static std::atomic<bool> g_calloc_unexpected(false);
static std::atomic<bool> g_free_unexpected(false);

void
on_unexpected_malloc(AnyMemoryToolsCallback callback)
{
  on_malloc(
    [callback](MemoryToolsService & service) {
      if (g_malloc_unexpected.load()) {
        service.unignore();
        dispatch_callback(&callback, service);
      }
    });
}

bool
malloc_expected()
{
  return !g_malloc_unexpected.load();
}

void
expect_no_malloc_begin()
{
  g_malloc_unexpected.store(true);
}

void
expect_no_malloc_end()
{
  g_malloc_unexpected.store(false);
}

void
on_unexpected_realloc(AnyMemoryToolsCallback callback)
{
  on_realloc(
    [callback](MemoryToolsService & service) {
      if (g_realloc_unexpected.load()) {
        service.unignore();
        dispatch_callback(&callback, service);
      }
    });
}

bool
realloc_expected()
{
  return !g_realloc_unexpected.load();
}

void
expect_no_realloc_begin()
{
  g_realloc_unexpected.store(true);
}

void
expect_no_realloc_end()
{
  g_realloc_unexpected.store(false);
}

void
on_unexpected_calloc(AnyMemoryToolsCallback callback)
{
  on_calloc(
    [callback](MemoryToolsService & service) {
      if (g_calloc_unexpected.load()) {
        service.unignore();
        dispatch_callback(&callback, service);
      }
    });
}

bool
calloc_expected()
{
  return !g_calloc_unexpected.load();
}

void
expect_no_calloc_begin()
{
  g_calloc_unexpected.store(true);
}

void
expect_no_calloc_end()
{
  g_calloc_unexpected.store(false);
}

void
on_unexpected_free(AnyMemoryToolsCallback callback)
{
  on_free(
    [callback](MemoryToolsService & service) {
      if (g_free_unexpected.load()) {
        service.unignore();
        dispatch_callback(&callback, service);
      }
    });
}

bool
free_expected()
{
  return !g_free_unexpected.load();
}

void
expect_no_free_begin()
{
  g_free_unexpected.store(true);
}

void
expect_no_free_end()
{
  g_free_unexpected.store(false);
}

}  // namespace memory_tools
}  // namespace osrf_testing_tools_cpp
