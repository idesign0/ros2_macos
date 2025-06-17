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

#ifndef TEST_RUNNER__PARSE_ENVIRONMENT_VARIABLE_HPP_
#define TEST_RUNNER__PARSE_ENVIRONMENT_VARIABLE_HPP_

#include <map>
#include <stdexcept>
#include <string>
#include <utility>

namespace test_runner
{

std::pair<std::string, std::string>
parse_environment_variable(const std::string & argument)
{
  if (argument.empty()) {
    throw std::invalid_argument("argument is empty");
  }
  if (argument.size() < 3) {
    // need at least `k=v`
    throw std::invalid_argument("argument is not long enough");
  }
  auto first_equal_sign = argument.find_first_of('=');
  if (first_equal_sign == 0) {
    throw std::invalid_argument("argument starts with a '='");
  }
  if (first_equal_sign == std::string::npos) {
    throw std::invalid_argument("argument does not contain '='");
  }
  return {argument.substr(0, first_equal_sign), argument.substr(first_equal_sign + 1)};
}

}  // namespace test_runner

#endif  // TEST_RUNNER__PARSE_ENVIRONMENT_VARIABLE_HPP_
