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

#include <stdexcept>
#include <string>

#include <gtest/gtest.h>  // NOLINT(build/include_order)

#include "../../src/test_runner/parse_environment_variable.hpp"

TEST(TestTestRunner, test_parse_environment_variable) {
  {
    std::string argument = "";
    EXPECT_THROW(test_runner::parse_environment_variable(argument), std::invalid_argument);
  }
  {
    std::string argument = "12";
    EXPECT_THROW(test_runner::parse_environment_variable(argument), std::invalid_argument);
  }
  {
    std::string argument = "longer";
    EXPECT_THROW(test_runner::parse_environment_variable(argument), std::invalid_argument);
  }
  {
    std::string argument = "=bar";
    EXPECT_THROW(test_runner::parse_environment_variable(argument), std::invalid_argument);
  }
  {
    std::string argument = "foo=";
    auto pair = test_runner::parse_environment_variable(argument);
    EXPECT_EQ("foo", pair.first);
    EXPECT_EQ("", pair.second);
  }
  {
    std::string argument = "foo=bar";
    auto pair = test_runner::parse_environment_variable(argument);
    EXPECT_EQ("foo", pair.first);
    EXPECT_EQ("bar", pair.second);
  }
}
