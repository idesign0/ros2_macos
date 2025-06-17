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
#include <iostream>
#include <memory>

#include "gtest/gtest.h"

#include "rclcpp/rclcpp.hpp"

#include "test_rclcpp/msg/u_int32.hpp"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

class CLASSNAME (test_repeated_publisher_subscriber, RMW_IMPLEMENTATION)
  : public ::testing::Test
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

TEST_F(
  CLASSNAME(test_repeated_publisher_subscriber, RMW_IMPLEMENTATION),
  subscription_and_spinning)
{
  auto node = rclcpp::Node::make_shared("test_repeated_publisher_subscriber");

  auto callback =
    [](const test_rclcpp::msg::UInt32::ConstSharedPtr) -> void
    {
    };

  test_rclcpp::msg::UInt32 msg;
  msg.data = 0;
  rclcpp::executors::SingleThreadedExecutor executor;

  {
    printf("Creating publisher and subscriber...\n");
    fflush(stdout);
    auto publisher = node->create_publisher<test_rclcpp::msg::UInt32>(
      "test_repeated_publisher_subscriber", 10);
    auto subscriber = node->create_subscription<test_rclcpp::msg::UInt32>(
      "test_repeated_publisher_subscriber", 10, callback);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    printf("spin_node_some()\n");
    fflush(stdout);
    executor.spin_node_some(node);

    msg.data = 1;
    publisher->publish(msg);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    printf("spin_node_some()\n");
    fflush(stdout);
    executor.spin_node_some(node);

    printf("Destroying publisher and subscriber...\n");
    fflush(stdout);
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  {
    printf("Recreating publisher and subscriber...\n");
    fflush(stdout);
    auto publisher = node->create_publisher<test_rclcpp::msg::UInt32>(
      "test_repeated_publisher_subscriber", 10);
    auto subscriber = node->create_subscription<test_rclcpp::msg::UInt32>(
      "test_repeated_publisher_subscriber", 10, callback);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    printf("spin_node_some()\n");
    fflush(stdout);
    executor.spin_node_some(node);

    msg.data = 2;
    publisher->publish(msg);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    printf("spin_node_some()\n");
    fflush(stdout);
    executor.spin_node_some(node);

    printf("Destroying publisher and subscriber...\n");
    fflush(stdout);
  }
  rclcpp::shutdown();
}
