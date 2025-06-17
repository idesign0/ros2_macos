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

#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <cstdio>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>  // NOLINT(build/include_order)

#include "../../src/test_runner/get_environment_variable.hpp"
#include "../../src/test_runner/parse_environment_variable.hpp"
#include "../../src/test_runner/starts_with.hpp"

void
usage(const std::string & program_name)
{
  printf("usage: %s [--env ENV=VALUE [ENV2=VALUE [...]]]\n", program_name.c_str());
}

std::vector<std::string> g_args;

TEST(TestTestRunner, environment_variables_set_correctly)
{
  bool should_show_usage_and_exit = (g_args.size() <= 1);
  for (auto arg : g_args) {
    should_show_usage_and_exit |= test_runner::starts_with_any(arg, {"-h", "--help"});
  }
  if (should_show_usage_and_exit) {
    usage(g_args[0].c_str());
    FAIL();
  }

  std::map<std::string, std::string> env_variables;

  std::string mode = "none";
  for (auto arg : g_args) {
    if (arg.empty()) {
      fprintf(stderr, "argument unexpectedly empty: %s\n", arg.c_str());
      FAIL();
    }
    // once in env mode, consume all flags no matter the content
    if (mode == "env") {
      auto env_pair = test_runner::parse_environment_variable(arg);
      env_variables[env_pair.first] = env_pair.second;
      continue;
    }

    // determine if the mode needs to change
    if (test_runner::starts_with(arg, "--env")) {
      mode = "env";
      continue;
    }

    // determine where to store the argument
    if (mode == "none") {
      fprintf(stderr, "unexpected positional argument: %s\n", arg.c_str());
      usage(g_args[0].c_str());
      FAIL();
    }
  }

  // Get the environment variables and check their values.
  for (auto pair : env_variables) {
    std::string expected_value = pair.second;
#if defined(_WIN32)
    // replace `:` with `;` on Windows for path like env variables
    std::replace(expected_value.begin(), expected_value.end(), ':', ';');
#endif
    std::string actual_value = test_runner::get_environment_variable(pair.first.c_str());
    EXPECT_EQ(expected_value, actual_value);
  }
}

int main(int argc, char * argv[])
{
  ::testing::InitGoogleTest(&argc, argv);

  g_args.reserve(static_cast<size_t>(argc));
  for (int i = 0; i < argc; ++i) {
    if (i == 0) {
      continue;
    }
    printf("%s\n", argv[i]);
    g_args.emplace_back(argv[i]);
  }

  return RUN_ALL_TESTS();
}
