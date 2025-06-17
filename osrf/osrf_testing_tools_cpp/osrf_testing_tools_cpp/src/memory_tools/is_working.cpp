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

#include "osrf_testing_tools_cpp/memory_tools/is_working.hpp"

#include <cstring>
#include <string>

#include "osrf_testing_tools_cpp/memory_tools/register_hooks.hpp"

namespace osrf_testing_tools_cpp
{
namespace memory_tools
{

volatile char side_effect[1024];

void
guaranteed_malloc(const std::string & str)
{
  void * some_memory = std::malloc(1024);
  // We need to do something with the malloc'ed memory to make sure this
  // function doesn't get optimized away.  memset isn't enough, so we do a
  // memcpy from a passed in string, and then copy *that* out to an array that
  // is globally visible (assuring we have a side-effect).  This is enough to
  // keep the optimizer away.
  memcpy(some_memory, str.c_str(), str.length());
  memcpy((void *)side_effect, some_memory, str.length());
  std::free(some_memory);
}

bool
is_working()
{
  auto original_on_malloc = get_on_malloc();
  bool malloc_was_called = false;
  on_malloc([&]() {malloc_was_called = true;});
  std::string tmp("doesn't matter");
  guaranteed_malloc(tmp);
  on_malloc(original_on_malloc);
  return malloc_was_called;
}

}  // namespace memory_tools
}  // namespace osrf_testing_tools_cpp
