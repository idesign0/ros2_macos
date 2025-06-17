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

#include <memory>
#include <stdexcept>
#include <thread>

#include "./memory_tools_service_impl.hpp"
#include "./stack_trace_impl.hpp"
#include "osrf_testing_tools_cpp/memory_tools/memory_tools_service.hpp"
#include "osrf_testing_tools_cpp/memory_tools/verbosity.hpp"

namespace osrf_testing_tools_cpp
{
namespace memory_tools
{

MemoryToolsService::MemoryToolsService(
  MemoryFunctionType memory_function_type,
  const char * source_function_name)
: impl_(new MemoryToolsServiceImpl(memory_function_type, source_function_name))
{
  switch(get_verbosity_level()) {
    case VerbosityLevel::quiet:
      impl_->ignored = true;
      impl_->should_print_backtrace = false;
      break;
    case VerbosityLevel::debug:
      impl_->ignored = false;
      impl_->should_print_backtrace = false;
      break;
    case VerbosityLevel::trace:
      impl_->ignored = false;
      impl_->should_print_backtrace = true;
      break;
    default:
      throw std::logic_error("unexpected case for VerbosityLevel");
  }
}

MemoryToolsService::~MemoryToolsService()
{}

MemoryFunctionType
MemoryToolsService::get_memory_function_type() const
{
  return impl_->memory_function_type;
}

const char *
MemoryToolsService::get_memory_function_type_str() const
{
  switch (impl_->memory_function_type) {
    case MemoryFunctionType::Malloc:
      return "malloc";
    case MemoryFunctionType::Realloc:
      return "realloc";
    case MemoryFunctionType::Calloc:
      return "calloc";
    case MemoryFunctionType::Free:
      return "free";
    default:
      throw std::runtime_error("unexpected default case in switch statement");
  }
}

void
MemoryToolsService::ignore()
{
  impl_->ignored = true;
}

void
MemoryToolsService::unignore()
{
  impl_->ignored = false;
}

void
MemoryToolsService::print_backtrace()
{
  impl_->should_print_backtrace = true;
}

StackTrace *
MemoryToolsService::get_stack_trace()
{
#if !defined(_WIN32) && !defined(__ANDROID__)
  if (nullptr == impl_->lazy_stack_trace) {
    backward::StackTrace st;
    st.load_here(256);
    impl_->lazy_stack_trace.reset(new StackTrace(std::unique_ptr<StackTraceImpl>(
      new StackTraceImpl(st, std::this_thread::get_id())
    )));
  }
  return impl_->lazy_stack_trace.get();
#else
  return nullptr;
#endif  // !defined(_WIN32) && !defined(__ANDROID__)
}

const char *
MemoryToolsService::get_source_function_name() const
{
  return impl_->source_function_name;
}

}  // namespace memory_tools
}  // namespace osrf_testing_tools_cpp
