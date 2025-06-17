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

#ifndef OSRF_TESTING_TOOLS_CPP__MEMORY_TOOLS__IS_WORKING_HPP_
#define OSRF_TESTING_TOOLS_CPP__MEMORY_TOOLS__IS_WORKING_HPP_

#include <string>
#include <cstdlib>

#include "./visibility_control.hpp"

namespace osrf_testing_tools_cpp
{
namespace memory_tools
{

/// Return true if memory tools is enabled, installed (LD_PRELOAD was done), and working.
/**
 * This works by temporarily installing a on_malloc hook and then calling
 * malloc in a way that cannot be optimized out by the compiler.
 */
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
bool
is_working();

/// Copy an input string into allocated memory, guaranteeing malloc is called.
/**
 * Makes sure that the compiler doesn't optimize the malloc and free out.
 * The content of the input string doesn't matter, but should be non-empty.
 */
OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
void
guaranteed_malloc(const std::string & str);

}  // namespace memory_tools
}  // namespace osrf_testing_tools_cpp

#endif  // OSRF_TESTING_TOOLS_CPP__MEMORY_TOOLS__IS_WORKING_HPP_
