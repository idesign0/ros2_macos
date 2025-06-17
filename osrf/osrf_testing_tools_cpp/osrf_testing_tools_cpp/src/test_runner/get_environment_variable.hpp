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

#ifndef TEST_RUNNER__GET_ENVIRONMENT_VARIABLE_HPP_
#define TEST_RUNNER__GET_ENVIRONMENT_VARIABLE_HPP_

#include <stdexcept>
#include <string>

namespace test_runner
{

/// Return value for environment variable, or "" if not set.
std::string
get_environment_variable(const std::string & env_var_name)
{
  const char * env_value = nullptr;
#if defined(_WIN32)
  char * dup_env_value = nullptr;
  size_t dup_env_value_len = 0;
  errno_t ret = _dupenv_s(&dup_env_value, &dup_env_value_len, env_var_name.c_str());
  if (ret) {
    throw std::runtime_error("failed to get environment variable");
  }
  env_value = dup_env_value;
#else
  env_value = std::getenv(env_var_name.c_str());
#endif
  if (!env_value) {
    env_value = "";
  }
  std::string return_value = env_value;
#if defined(_WIN32)
  // also done with env_value, so free dup_env_value
  free(dup_env_value);
#endif
  return return_value;
}

}  // namespace test_runner

#endif  // TEST_RUNNER__GET_ENVIRONMENT_VARIABLE_HPP_
