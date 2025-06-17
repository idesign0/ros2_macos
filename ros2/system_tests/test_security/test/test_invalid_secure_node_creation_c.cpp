// Copyright 2016-2020 Open Source Robotics Foundation, Inc.
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

#include <stdio.h>
#include <stdlib.h>

#include <gtest/gtest.h>

#include <functional>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#include "osrf_testing_tools_cpp/scope_exit.hpp"

#include "rcl/rcl.h"
#include "rcl/error_handling.h"

#include "rcutils/env.h"
#include "rcutils/strdup.h"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

void custom_putenv(const char * name, const char * value)
{
#ifdef _WIN32
  _putenv_s(name, value);
#else
  setenv(name, value, 1);
#endif  // _WIN32
}

struct TestConfig
{
  const char * ROS_SECURITY_KEYSTORE;
  const char * ROS_SECURITY_ENABLE;
  const char * ROS_SECURITY_STRATEGY;
  const char * context_name;
  bool should_fail_context_creation;
  bool should_fail_node_creation;
  const char * test_description;
};

std::ostream & operator<<(
  std::ostream & out,
  const TestConfig & config)
{
  out << config.test_description;
  return out;
}

class CLASSNAME (TestSecureNodes, RMW_IMPLEMENTATION)
  : public ::testing::TestWithParam<TestConfig>
{
public:
  rcl_context_t context;
  rcl_node_t node;
  char * previous_keystore_dir;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  void SetUp()
  {
    const char * keystore_dir = nullptr;
    ASSERT_EQ(nullptr, rcutils_get_env("ROS_SECURITY_KEYSTORE", &keystore_dir));
    previous_keystore_dir = rcutils_strdup(keystore_dir, allocator);
    ASSERT_NE(nullptr, previous_keystore_dir);
  }

  void TearDown()
  {
    custom_putenv("ROS_SECURITY_KEYSTORE", previous_keystore_dir);
    allocator.deallocate(previous_keystore_dir, allocator.state);
  }
};

using TestSecureNodes = CLASSNAME(TestSecureNodes, RMW_IMPLEMENTATION);

TEST_P(TestSecureNodes, test_invalid_keystore) {
  const auto & test_config = GetParam();
  if (test_config.ROS_SECURITY_KEYSTORE != NULL) {
    custom_putenv("ROS_SECURITY_KEYSTORE", test_config.ROS_SECURITY_KEYSTORE);
  }
  custom_putenv("ROS_SECURITY_ENABLE", test_config.ROS_SECURITY_ENABLE);
  custom_putenv("ROS_SECURITY_STRATEGY", test_config.ROS_SECURITY_STRATEGY);
  rcl_ret_t ret;
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  context = rcl_get_zero_initialized_context();
  const char * argv[] = {"--ros-args", "--enclave", test_config.context_name};
  ret = rcl_init(sizeof(argv) / sizeof(char *), argv, &init_options, &context);
  if (test_config.should_fail_context_creation) {
    ASSERT_EQ(RCL_RET_ERROR, ret);
    rcl_reset_error();
    return;
  }
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ret = rcl_shutdown(&context);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  this->node = rcl_get_zero_initialized_node();
  rcl_node_options_t node_options = rcl_node_get_default_options();
  ret = rcl_node_init(
    &this->node, "test_invalid_secure_node_creation_c", "", &context, &node_options);
  if (test_config.should_fail_node_creation) {
    ASSERT_EQ(RCL_RET_ERROR, ret);
    rcl_reset_error();
  } else {
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_node_fini(&this->node);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }
}

std::vector<TestConfig> test_configs {
  {
    "garbage",
    "true",
    "Enforce",
    "/publisher",
    true,
    true,
    "test_invalid_keystore",
  },
  {
    NULL,
    "true",
    "Enforce",
    "/publisher2",
    true,
    true,
    "test_unknown_identity",
  },
  {
    NULL,
    "true",
    "Enforce",
    "/publisher_invalid_cert",
    false,
    true,
    "test_invalid_cert",
  },
  {
    NULL,
    "true",
    "Enforce",
    "/publisher_missing_key",
    false,
    true,
    "test_missing_key",
  },
};

INSTANTIATE_TEST_SUITE_P(
  TestInitializationFails,
  TestSecureNodes,
  ::testing::ValuesIn(test_configs),
  ::testing::PrintToStringParamName());
