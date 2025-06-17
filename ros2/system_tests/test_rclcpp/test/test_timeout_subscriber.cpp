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

#include <chrono>
#include <stdexcept>
#include <string>

#include "gtest/gtest.h"

#include "rclcpp/rclcpp.hpp"
#include "test_rclcpp/msg/u_int32.hpp"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

void callback(const test_rclcpp::msg::UInt32::ConstSharedPtr /*msg*/)
{
  throw std::runtime_error("The subscriber received a message but there should be no publisher!");
}

class CLASSNAME (test_timeout_subscriber, RMW_IMPLEMENTATION) : public ::testing::Test
{
public:
  static void SetUpTestCase()
  {
    rclcpp::init(0, nullptr);
  }

  static void TearDownTestCase()
  {
    rclcpp::shutdown();
  }
};

TEST_F(CLASSNAME(test_timeout_subscriber, RMW_IMPLEMENTATION), timeout_subscriber) {
  auto start = std::chrono::steady_clock::now();

  auto node = rclcpp::Node::make_shared("test_timeout_subscriber");
  // Add subscription to its own callback group to avoid interference from other things in the node
  auto cg = node->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
  rclcpp::SubscriptionOptions options;
  options.callback_group = cg;

  auto subscriber = node->create_subscription<test_rclcpp::msg::UInt32>(
    "test_message_timeout_uint32", 10, callback, options);

  rclcpp::executors::SingleThreadedExecutor executor;
  executor.add_callback_group(cg, node->get_node_base_interface());

  size_t num_cycles = 5;

  for (size_t i = 0; i < num_cycles; ++i) {
    auto tolerance = std::chrono::milliseconds(20);

    // ensure that the non-blocking spin does return immediately
    auto nonblocking_start = std::chrono::steady_clock::now();
    executor.spin_once(std::chrono::milliseconds::zero());
    auto nonblocking_end = std::chrono::steady_clock::now();
    auto nonblocking_diff = nonblocking_end - nonblocking_start;
    EXPECT_LT(
      std::chrono::duration_cast<std::chrono::nanoseconds>(nonblocking_diff).count(),
      std::chrono::duration_cast<std::chrono::nanoseconds>(tolerance).count());

    // ensure that the blocking spin does return after the specified timeout
    auto blocking_timeout = std::chrono::milliseconds(100);
    auto blocking_start = std::chrono::steady_clock::now();
    executor.spin_once(blocking_timeout);
    auto blocking_end = std::chrono::steady_clock::now();
    auto blocking_diff = blocking_end - blocking_start;
    EXPECT_GT(
      std::chrono::duration_cast<std::chrono::nanoseconds>(blocking_diff).count(),
      std::chrono::duration_cast<std::chrono::nanoseconds>(blocking_timeout - tolerance).count());
    EXPECT_LT(
      std::chrono::duration_cast<std::chrono::nanoseconds>(blocking_diff).count(),
      std::chrono::duration_cast<std::chrono::nanoseconds>(blocking_timeout + tolerance).count());
  }

  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<float> diff = (end - start);
  printf("subscribed for %f seconds\n", diff.count());
}
