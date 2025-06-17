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

#include "osrf_testing_tools_cpp/memory_tools/stack_trace.hpp"

#include "./stack_trace_impl.hpp"

namespace osrf_testing_tools_cpp
{
namespace memory_tools
{

SourceLocation::SourceLocation(std::shared_ptr<SourceLocationImpl> impl)
: impl_(std::move(impl))
{}

SourceLocation::~SourceLocation()
{}

const std::string &
SourceLocation::function() const
{
#if !defined(_WIN32) && !defined(__ANDROID__)
  return impl_->source_location->function;
#else
  throw std::runtime_error("not implemented on Windows or Android");
#endif
}

const std::string &
SourceLocation::filename() const
{
#if !defined(_WIN32) && !defined(__ANDROID__)
  return impl_->source_location->filename;
#else
  throw std::runtime_error("not implemented on Windows or Android");
#endif
}

size_t
SourceLocation::line() const
{
#if !defined(_WIN32) && !defined(__ANDROID__)
  return impl_->source_location->line;
#else
  throw std::runtime_error("not implemented on Windows or Android");
#endif
}

size_t
SourceLocation::column() const
{
#if !defined(_WIN32) && !defined(__ANDROID__)
  return impl_->source_location->col;
#else
  throw std::runtime_error("not implemented on Windows or Android");
#endif
}

Trace::Trace(std::unique_ptr<TraceImpl> impl)
: impl_(std::move(impl))
{}

Trace::Trace(const Trace & other)
: impl_(new TraceImpl(*other.impl_))
{}

Trace::~Trace()
{}

void *
Trace::address() const
{
#if !defined(_WIN32) && !defined(__ANDROID__)
  return impl_->resolved_trace.addr;
#else
  throw std::runtime_error("not implemented on Windows or Android");
#endif
}

size_t
Trace::index_in_stack() const
{
#if !defined(_WIN32) && !defined(__ANDROID__)
  return impl_->resolved_trace.idx;
#else
  throw std::runtime_error("not implemented on Windows or Android");
#endif
}

const std::string &
Trace::object_filename() const
{
#if !defined(_WIN32) && !defined(__ANDROID__)
  return impl_->resolved_trace.object_filename;
#else
  throw std::runtime_error("not implemented on Windows or Android");
#endif
}

const std::string &
Trace::object_function() const
{
#if !defined(_WIN32) && !defined(__ANDROID__)
  return impl_->resolved_trace.object_function;
#else
  throw std::runtime_error("not implemented on Windows or Android");
#endif
}

const SourceLocation &
Trace::source_location() const
{
#if !defined(_WIN32) && !defined(__ANDROID__)
  return impl_->source_location;
#else
  throw std::runtime_error("not implemented on Windows or Android");
#endif
}

const std::vector<SourceLocation> &
Trace::inlined_source_locations() const
{
#if !defined(_WIN32) && !defined(__ANDROID__)
  return impl_->inlined_source_locations;
#else
  throw std::runtime_error("not implemented on Windows or Android");
#endif
}

StackTrace::StackTrace(std::unique_ptr<StackTraceImpl> impl)
: impl_(std::move(impl))
{}

StackTrace::~StackTrace()
{}

std::thread::id
StackTrace::thread_id() const
{
#if !defined(_WIN32) && !defined(__ANDROID__)
  return impl_->thread_id;
#else
  throw std::runtime_error("not implemented on Windows or Android");
#endif
}

const std::vector<Trace> &
StackTrace::get_traces() const
{
#if !defined(_WIN32) && !defined(__ANDROID__)
  return impl_->traces;
#else
  throw std::runtime_error("not implemented on Windows or Android");
#endif
}

std::vector<Trace>
StackTrace::get_traces_from_function_name(const char * function_name) const
{
#if !defined(_WIN32) && !defined(__ANDROID__)
  std::vector<Trace> result;
  bool function_found = false;
  for (const Trace & trace : impl_->traces) {
    if (!function_found && trace.object_function().find(function_name) == 0) {
      function_found = true;
    }
    if (function_found) {
      result.emplace_back(trace);
    }
  }
  return result;
#else
  (void) function_name;
  throw std::runtime_error("not implemented on Windows or Android");
#endif
}

}  // namespace memory_tools
}  // namespace osrf_testing_tools_cpp
