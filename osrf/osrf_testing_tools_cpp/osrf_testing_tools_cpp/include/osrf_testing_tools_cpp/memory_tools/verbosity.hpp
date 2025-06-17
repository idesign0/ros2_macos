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

#ifndef OSRF_TESTING_TOOLS_CPP__MEMORY_TOOLS__VERBOSITY_HPP_
#define OSRF_TESTING_TOOLS_CPP__MEMORY_TOOLS__VERBOSITY_HPP_

#include "./visibility_control.hpp"

namespace osrf_testing_tools_cpp
{
namespace memory_tools
{

enum class VerbosityLevel {
  quiet,
  debug,
  trace,
};

OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
VerbosityLevel
get_verbosity_level();

OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC
VerbosityLevel
set_verbosity_level(VerbosityLevel verbosity_level);

}  // namespace memory_tools
}  // namespace osrf_testing_tools_cpp

#endif  // OSRF_TESTING_TOOLS_CPP__MEMORY_TOOLS__VERBOSITY_HPP_
