// Copyright 2015 Open Source Robotics Foundation, Inc.
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

#ifndef OSRF_TESTING_TOOLS_CPP__SCOPE_EXIT_HPP_
#define OSRF_TESTING_TOOLS_CPP__SCOPE_EXIT_HPP_

#include <functional>

#include "./macros.hpp"

namespace osrf_testing_tools_cpp
{

template<typename Callable>
struct ScopeExit
{
  explicit ScopeExit(Callable callable)
  : callable_(callable) {}
  ~ScopeExit() {callable_();}

private:
  Callable callable_;
};

template<typename Callable>
ScopeExit<Callable>
make_scope_exit(Callable callable)
{
  return ScopeExit<Callable>(callable);
}

}  // namespace osrf_testing_tools_cpp

#define OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(code) \
  auto OSRF_TESTING_TOOLS_CPP_STRING_JOIN(scope_exit_, __LINE__) = \
    osrf_testing_tools_cpp::make_scope_exit([&]() {code;})

#endif  // OSRF_TESTING_TOOLS_CPP__SCOPE_EXIT_HPP_
