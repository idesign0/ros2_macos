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

#include <atomic>
#include <cstring>

#include "./custom_memory_functions.hpp"
#include "osrf_testing_tools_cpp/memory_tools/initialize.hpp"
#include "osrf_testing_tools_cpp/memory_tools/monitoring.hpp"
#include "osrf_testing_tools_cpp/memory_tools/register_hooks.hpp"
#include "osrf_testing_tools_cpp/memory_tools/testing_helpers.hpp"
#include "osrf_testing_tools_cpp/memory_tools/verbosity.hpp"

namespace osrf_testing_tools_cpp
{
namespace memory_tools
{

static std::atomic<bool> g_initialized(false);

bool
initialized()
{
  return g_initialized.load();
}

void
initialize()
{
  auto conditional_print = [](const char * msg) {
    if (get_verbosity_level() != VerbosityLevel::quiet) {
      SAFE_FWRITE(stdout, msg);
    }
  };
  conditional_print("initializing memory tools...\n");
  g_initialized.store(true);
}

bool
uninitialize()
{
  // reset settings
  unset_thread_specific_monitoring_enable();
  disable_monitoring_in_all_threads();
  on_malloc(nullptr);
  on_realloc(nullptr);
  on_calloc(nullptr);
  on_free(nullptr);
  expect_no_malloc_end();
  expect_no_realloc_end();
  expect_no_calloc_end();
  expect_no_free_end();
  return g_initialized.exchange(true);
}

}  // namespace memory_tools
}  // namespace osrf_testing_tools_cpp
