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

#include "osrf_testing_tools_cpp/memory_tools/monitoring.hpp"

#include <atomic>

#include "./implementation_monitoring_override.hpp"
#include "osrf_testing_tools_cpp/memory_tools/initialize.hpp"
#include "osrf_testing_tools_cpp/scope_exit.hpp"

namespace osrf_testing_tools_cpp
{
namespace memory_tools
{

static thread_local bool g_tls_thread_specific_enable_set = false;
static thread_local bool g_tls_enabled = false;
static std::atomic<bool> g_enabled(false);

bool
monitoring_enabled()
{
  if (
    !::osrf_testing_tools_cpp::memory_tools::initialized() ||
    ::osrf_testing_tools_cpp::memory_tools::inside_implementation())
  {
    return false;
  }
  if (g_tls_thread_specific_enable_set) {
    return g_tls_enabled;
  } else {
    return g_enabled.load();
  }
}

void
enable_monitoring()
{
  g_tls_enabled = true;
  g_tls_thread_specific_enable_set = true;
}

void
disable_monitoring()
{
  g_tls_enabled = false;
  g_tls_thread_specific_enable_set = true;
}

void
unset_thread_specific_monitoring_enable()
{
  g_tls_enabled = false;
  g_tls_thread_specific_enable_set = false;
}

bool
enable_monitoring_in_all_threads()
{
  return g_enabled.exchange(true);
}

bool
disable_monitoring_in_all_threads()
{
  return g_enabled.exchange(false);
}

}  // namespace memory_tools
}  // namespace osrf_testing_tools_cpp
