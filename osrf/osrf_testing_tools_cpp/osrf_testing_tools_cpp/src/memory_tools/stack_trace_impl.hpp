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

#ifndef OSRF_TESTING_TOOLS_CPP__STACK_TRACE_IMPL_HPP_
#define OSRF_TESTING_TOOLS_CPP__STACK_TRACE_IMPL_HPP_

#include <memory>
#include <string>
#include <thread>

#include "osrf_testing_tools_cpp/memory_tools/stack_trace.hpp"

#if !defined(_WIN32) && !defined(__ANDROID__)

#pragma GCC diagnostic push
#ifdef __clang__
# pragma clang diagnostic ignored "-Wgnu-include-next"
# pragma clang diagnostic ignored "-Wunused-parameter"
#endif
#include "./vendor/bombela/backward-cpp/backward.hpp"
#pragma GCC diagnostic pop

namespace osrf_testing_tools_cpp
{
namespace memory_tools
{

struct SourceLocationImpl
{
  SourceLocationImpl() = delete;
  explicit SourceLocationImpl(const backward::ResolvedTrace::SourceLoc * sl)
  : source_location(sl)
  {}

  virtual ~SourceLocationImpl() {}

  const backward::ResolvedTrace::SourceLoc * source_location;
};

struct TraceImpl
{
  TraceImpl() = delete;

  TraceImpl(backward::TraceResolver * trace_resolver, const backward::Trace & trace)
  : TraceImpl(trace_resolver->resolve(trace))
  {}

  TraceImpl(const TraceImpl & other)
  : TraceImpl(other.resolved_trace)
  {}

  explicit TraceImpl(backward::ResolvedTrace resolved_trace_input)
  : resolved_trace(resolved_trace_input),
    source_location(
      std::unique_ptr<SourceLocationImpl>(new SourceLocationImpl(&resolved_trace.source)))
  {
    inlined_source_locations.reserve(resolved_trace.inliners.size());
    for (const auto & inliner : resolved_trace.inliners) {
      inlined_source_locations.emplace_back(
        std::shared_ptr<SourceLocationImpl>(new SourceLocationImpl(&inliner))
      );
    }
  }

  virtual ~TraceImpl() {}

  backward::ResolvedTrace resolved_trace;
  SourceLocation source_location;
  std::vector<SourceLocation> inlined_source_locations;
};

struct StackTraceImpl
{
  StackTraceImpl() = delete;
  explicit StackTraceImpl(backward::StackTrace st, std::thread::id tid)
  : stack_trace(st), thread_id(tid)
  {
    trace_resolver.load_stacktrace(stack_trace);
    traces.reserve(stack_trace.size());
    for (size_t i = 0; i < stack_trace.size(); ++i) {
      std::unique_ptr<TraceImpl> tmp(new TraceImpl(&trace_resolver, stack_trace[i]));
      traces.emplace_back(
        std::move(tmp)
      );
    }
  }

  virtual ~StackTraceImpl() {}

  backward::StackTrace stack_trace;
  std::thread::id thread_id;
  backward::TraceResolver trace_resolver;
  std::vector<Trace> traces;
};

}  // namespace memory_tools
}  // namespace osrf_testing_tools_cpp

#else

namespace osrf_testing_tools_cpp
{
namespace memory_tools
{

struct SourceLocationImpl {};

struct TraceImpl {};

struct StackTraceImpl {};

}  // namespace memory_tools
}  // namespace osrf_testing_tools_cpp

#endif  // !defined(_WIN32) && !defined(__ANDROID__)

#endif  // OSRF_TESTING_TOOLS_CPP__STACK_TRACE_IMPL_HPP_
