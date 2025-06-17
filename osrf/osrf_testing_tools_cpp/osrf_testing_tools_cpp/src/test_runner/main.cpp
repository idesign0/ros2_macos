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

#include <cerrno>
#include <cstdlib>
#include <cstdio>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "./execute_process.hpp"
#include "./get_environment_variable.hpp"
#include "./starts_with.hpp"
#include "./parse_environment_variable.hpp"

void
usage(const std::string & program_name)
{
  printf(
    "usage: %s "
    "[--env ENV=VALUE [ENV2=VALUE [...]]] "
    "[--append-env ENV=VALUE [ENV2=VALUE [...]]] "
    "-- <command>\n", program_name.c_str());
}

int
main(int argc, char const * argv[])
{
  std::vector<std::string> args;
  args.reserve(static_cast<size_t>(argc));
  for (int i = 0; i < argc; ++i) {
    if (i == 0) {
      continue;
    }
    args.emplace_back(argv[i]);
  }

  bool should_show_usage_and_exit = (args.size() <= 1);
  for (auto arg : args) {
    should_show_usage_and_exit |= test_runner::starts_with_any(arg, {"-h", "--help"});
  }
  if (should_show_usage_and_exit) {
    usage(argv[0]);
    return 1;
  }

  std::map<std::string, std::string> env_variables;
  std::map<std::string, std::string> append_env_variables;
  std::vector<std::string> commands;

  std::string mode = "none";
  for (auto arg : args) {
    if (arg.empty()) {
      fprintf(stderr, "argument unexpectedly empty: %s\n", arg.c_str());
      return 1;
    }
    // once in command mode, consume all flags no matter the content
    if (mode == "command") {
      commands.push_back(arg);
      continue;
    }

    // determine if the mode needs to change
    if (test_runner::starts_with(arg, "--env")) {
      mode = "env";
      continue;
    }
    if (test_runner::starts_with(arg, "--append-env")) {
      mode = "append_env";
      continue;
    }
    if (arg == "--") {
      mode = "command";
      continue;
    }

    // determine where to store the argument
    if (mode == "none") {
      fprintf(stderr, "unexpected positional argument: %s\n", arg.c_str());
      usage(argv[0]);
      return 1;
    } else if (mode == "env") {
      try {
        auto env_pair = test_runner::parse_environment_variable(arg);
        env_variables[env_pair.first] = env_pair.second;
      } catch (const std::invalid_argument & exc) {
        fprintf(
          stderr,
          "invalid environment variable, expected ENV=VALUE, %s: %s\n",
          exc.what(), arg.c_str());
        return 1;
      }
    } else if (mode == "append_env") {
      try {
        auto env_pair = test_runner::parse_environment_variable(arg);
        append_env_variables[env_pair.first] = env_pair.second;
      } catch (const std::invalid_argument & exc) {
        fprintf(
          stderr,
          "invalid environment variable, expected ENV=VALUE, %s: %s\n",
          exc.what(), arg.c_str());
        return 1;
      }
    } else {
      fprintf(stderr, "unexpected mode '%s'\n", mode.c_str());
      return 1;
    }
  }

  // Set the environment variables.
  for (auto pair : env_variables) {
#if defined(_WIN32)
    if (0 != _putenv_s(pair.first.c_str(), pair.second.c_str())) {
#else
    if (0 != setenv(pair.first.c_str(), pair.second.c_str(), 1)) {
#endif
      fprintf(stderr,
        "failed to set environment variable '%s=%s'\n",
        pair.first.c_str(), pair.second.c_str());
      return 1;
    }
  }

  // Append the PATH-like environment variables.
  for (auto pair : append_env_variables) {
    std::string new_value;
    try {
      new_value = test_runner::get_environment_variable(pair.first.c_str());
    } catch (const std::runtime_error &) {
      fprintf(stderr, "failed to get environment variable '%s'\n", pair.first.c_str());
      return 1;
    }
#if defined(_WIN32)
    auto path_sep = ";";
#else
    auto path_sep = ":";
#endif
    if (!new_value.empty() && new_value.substr(new_value.size() - 1) != path_sep) {
      new_value += path_sep;
    }
    new_value += pair.second;
#if defined(_WIN32)
    if (0 != _putenv_s(pair.first.c_str(), new_value.c_str())) {
#else
    if (0 != setenv(pair.first.c_str(), new_value.c_str(), 1)) {
#endif
      fprintf(stderr,
        "failed to set environment variable '%s=%s'\n",
        pair.first.c_str(), new_value.c_str());
      return 1;
    }
  }

  // Run the command.
  return test_runner::execute_process(commands);
}
