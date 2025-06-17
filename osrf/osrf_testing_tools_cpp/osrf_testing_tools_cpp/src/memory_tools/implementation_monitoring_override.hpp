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

#ifndef MEMORY_TOOLS__IMPLEMENTATION_MONITORING_OVERRIDE_HPP_
#define MEMORY_TOOLS__IMPLEMENTATION_MONITORING_OVERRIDE_HPP_

namespace osrf_testing_tools_cpp
{
namespace memory_tools
{

/// Return true if dynamic memory hooks should be avoided due to being inside implementation.
bool
inside_implementation();

/// Begin an implementation section, thread-specific.
void
begin_implementation_section();

/// End an implementation section, thread-specific.
void
end_implementation_section();

/// Scoped implementation context, thread-specific.
/**
 * While this object is in scope, it's considered to be an implementation section.
 */
class ScopedImplementationSection
{
public:
  ScopedImplementationSection();
  ~ScopedImplementationSection();
};

}  // namespace memory_tools
}  // namespace osrf_testing_tools_cpp

#endif  // MEMORY_TOOLS__IMPLEMENTATION_MONITORING_OVERRIDE_HPP_
