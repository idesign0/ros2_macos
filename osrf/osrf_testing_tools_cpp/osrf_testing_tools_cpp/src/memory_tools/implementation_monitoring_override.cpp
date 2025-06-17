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

#include "./implementation_monitoring_override.hpp"

namespace osrf_testing_tools_cpp
{
namespace memory_tools
{

static bool g_inside_implementation = false;

bool
inside_implementation()
{
  return g_inside_implementation;
}

void
begin_implementation_section()
{
  g_inside_implementation = true;
}

void
end_implementation_section()
{
  g_inside_implementation = false;
}

ScopedImplementationSection::ScopedImplementationSection()
{
  begin_implementation_section();
}

ScopedImplementationSection::~ScopedImplementationSection()
{
  end_implementation_section();
}

}  // namespace memory_tools
}  // namespace osrf_testing_tools_cpp
