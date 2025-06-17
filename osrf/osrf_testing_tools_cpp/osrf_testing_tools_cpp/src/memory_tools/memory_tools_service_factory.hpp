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

#ifndef MEMORY_TOOLS__MEMORY_TOOLS_SERVICE_FACTORY_HPP_
#define MEMORY_TOOLS__MEMORY_TOOLS_SERVICE_FACTORY_HPP_

#include "./memory_tools_service_impl.hpp"
#include "osrf_testing_tools_cpp/memory_tools/memory_tools_service.hpp"

namespace osrf_testing_tools_cpp
{
namespace memory_tools
{

class MemoryToolsServiceFactory
{
public:
  MemoryToolsServiceFactory(
    MemoryFunctionType memory_function_type,
    const char * source_function_name)
  : service_(memory_function_type, source_function_name)
  {}

  MemoryToolsService &
  get_memory_tools_service()
  {
    return service_;
  }

  bool
  should_ignore()
  {
    return !service_.impl_->should_print_backtrace && service_.impl_->ignored;
  }

  bool
  should_print_backtrace()
  {
    return service_.impl_->should_print_backtrace;
  }

private:
  MemoryToolsService service_;
};

}  // namespace memory_tools
}  // namespace osrf_testing_tools_cpp

#endif  // MEMORY_TOOLS__MEMORY_TOOLS_SERVICE_FACTORY_HPP_
