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

#ifndef MEMORY_TOOLS__MEMORY_TOOLS_SERVICE_IMPL_HPP_
#define MEMORY_TOOLS__MEMORY_TOOLS_SERVICE_IMPL_HPP_

#include <memory>

#include "osrf_testing_tools_cpp/memory_tools/memory_tools_service.hpp"

namespace osrf_testing_tools_cpp
{
namespace memory_tools
{

class MemoryToolsServiceImpl
{
public:
  MemoryToolsServiceImpl(
    MemoryFunctionType memory_function_type_in,
    const char * source_function_name_in)
  : memory_function_type(memory_function_type_in),
    source_function_name(source_function_name_in),
    lazy_stack_trace(nullptr)
  {}

  MemoryFunctionType memory_function_type;
  const char * source_function_name;

  bool ignored;
  bool should_print_backtrace;
  std::unique_ptr<StackTrace> lazy_stack_trace;
};

}  // namespace memory_tools
}  // namespace osrf_testing_tools_cpp

#endif  // MEMORY_TOOLS__MEMORY_TOOLS_SERVICE_IMPL_HPP_
