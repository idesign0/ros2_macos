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

#ifndef TEST_RUNNER__STARTS_WITH_HPP_
#define TEST_RUNNER__STARTS_WITH_HPP_

#include <stdexcept>
#include <string>
#include <vector>

namespace test_runner
{

/// Return true if a string starts with a prefix.
bool
starts_with(const std::string & str, const std::string & prefix)
{
  return !str.compare(0, prefix.size(), prefix);
}

/// Return true if a string starts with any of the given prefixes.
bool
starts_with_any(const std::string & str, const std::vector<std::string> & prefixes)
{
  for (auto prefix : prefixes) {
    if (test_runner::starts_with(str, prefix)) {
      return true;
    }
  }
  return false;
}

}  // namespace test_runner

#endif  // TEST_RUNNER__STARTS_WITH_HPP_
