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
#include "./implementation_monitoring_override.hpp"
#include "osrf_testing_tools_cpp/memory_tools/register_hooks.hpp"

namespace osrf_testing_tools_cpp
{
namespace memory_tools
{

static std::atomic<AnyMemoryToolsCallback *> g_on_malloc_callback(nullptr);
static std::atomic<AnyMemoryToolsCallback *> g_on_realloc_callback(nullptr);
static std::atomic<AnyMemoryToolsCallback *> g_on_calloc_callback(nullptr);
static std::atomic<AnyMemoryToolsCallback *> g_on_free_callback(nullptr);

void
on_malloc(AnyMemoryToolsCallback callback)
{
  // prevents new from triggering existing hooks
  ScopedImplementationSection implementation_section;
  // duplicate user callback and then delete the old one
  auto old = g_on_malloc_callback.exchange(new AnyMemoryToolsCallback(callback));
  if (nullptr != old) {
    delete old;
  }
}

AnyMemoryToolsCallback
get_on_malloc()
{
  auto current = g_on_malloc_callback.load();
  if (current) {
    return *current;
  }
  return nullptr;
}

void
dispatch_malloc(MemoryToolsService & service)
{
  dispatch_callback(g_on_malloc_callback.load(), service);
}

void
on_realloc(AnyMemoryToolsCallback callback)
{
  // prevents new from triggering existing hooks
  ScopedImplementationSection implementation_section;
  // duplicate user callback and then delete the old one
  delete g_on_realloc_callback.exchange(new AnyMemoryToolsCallback(callback));
}

AnyMemoryToolsCallback
get_on_realloc()
{
  auto current = g_on_realloc_callback.load();
  if (current) {
    return *current;
  }
  return nullptr;
}

void
dispatch_realloc(MemoryToolsService & service)
{
  dispatch_callback(g_on_realloc_callback.load(), service);
}

void
on_calloc(AnyMemoryToolsCallback callback)
{
  // prevents new from triggering existing hooks
  ScopedImplementationSection implementation_section;
  // duplicate user callback and then delete the old one
  delete g_on_calloc_callback.exchange(new AnyMemoryToolsCallback(callback));
}

AnyMemoryToolsCallback
get_on_calloc()
{
  auto current = g_on_calloc_callback.load();
  if (current) {
    return *current;
  }
  return nullptr;
}

void
dispatch_calloc(MemoryToolsService & service)
{
  dispatch_callback(g_on_calloc_callback.load(), service);
}

void
on_free(AnyMemoryToolsCallback callback)
{
  // prevents new from triggering existing hooks
  ScopedImplementationSection implementation_section;
  // duplicate user callback and then delete the old one
  delete g_on_free_callback.exchange(new AnyMemoryToolsCallback(callback));
}

AnyMemoryToolsCallback
get_on_free()
{
  auto current = g_on_free_callback.load();
  if (current) {
    return *current;
  }
  return nullptr;
}

void
dispatch_free(MemoryToolsService & service)
{
  dispatch_callback(g_on_free_callback.load(), service);
}

}  // namespace memory_tools
}  // namespace osrf_testing_tools_cpp
