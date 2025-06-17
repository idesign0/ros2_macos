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


static const std::chrono::milliseconds sleep_per_loop(10);
static const int max_loops = 200;

class CLASSNAME (test_intra_process_within_one_node, RMW_IMPLEMENTATION) : public ::testing::Test
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

TEST_F(CLASSNAME(test_intra_process_within_one_node, RMW_IMPLEMENTATION), nominal_usage) {
  // use intra process = true
  auto node = rclcpp::Node::make_shared(
    "test_intra_process",
    rclcpp::NodeOptions().use_intra_process_comms(true));

  auto publisher = node->create_publisher<test_rclcpp::msg::UInt32>("test_intra_process", 10);

  int counter = 0;
  auto callback =
    [&counter](
    const test_rclcpp::msg::UInt32::ConstSharedPtr msg,
    const rclcpp::MessageInfo & message_info
    ) -> void
    {
      ++counter;
      printf("  callback() %d with message data %u\n", counter, msg->data);
      ASSERT_GE(counter, 0);
      ASSERT_EQ(static_cast<unsigned int>(counter), msg->data);
      ASSERT_TRUE(message_info.get_rmw_message_info().from_intra_process);
    };

  test_rclcpp::msg::UInt32 msg;
  msg.data = 0;
  rclcpp::executors::SingleThreadedExecutor executor;

  {
    auto subscriber = node->create_subscription<test_rclcpp::msg::UInt32>(
      "test_intra_process", 10, callback);

    // start condition
    ASSERT_EQ(0, counter);

    // nothing should be pending here
    printf("spin_node_once(nonblocking) - no callback expected\n");
    executor.spin_node_once(node, std::chrono::milliseconds(0));
    ASSERT_EQ(0, counter);
    printf("spin_node_some() - no callback expected\n");
    executor.spin_node_some(node);
    ASSERT_EQ(0, counter);

    // wait a moment for everything to initialize
    // TODO(gerkey): fix nondeterministic startup behavior
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    msg.data = 1;
    publisher->publish(msg);
    ASSERT_EQ(0, counter);

    // wait for the first callback
    {
      int i = 0;
      executor.spin_node_once(node, std::chrono::milliseconds(0));
      while (counter == 0 && i < max_loops) {
        printf("spin_node_once() - callback (1) expected - try %d/%d\n", ++i, max_loops);
        std::this_thread::sleep_for(sleep_per_loop);
        executor.spin_node_once(node, std::chrono::milliseconds(0));
      }
    }
    ASSERT_EQ(1, counter);

    // nothing should be pending here
    printf("spin_node_once(nonblocking) - no callback expected\n");
    executor.spin_node_once(node, std::chrono::milliseconds(0));
    ASSERT_EQ(1, counter);
    printf("spin_node_some() - no callback expected\n");
    executor.spin_node_some(node);
    ASSERT_EQ(1, counter);

    msg.data = 2;
    publisher->publish(msg);
    msg.data = 3;
    publisher->publish(msg);
    msg.data = 4;
    publisher->publish(msg);
    msg.data = 5;
    publisher->publish(msg);
    ASSERT_EQ(1, counter);

    // while four messages have been published one callback should be triggered here
    {
      int i = 0;
      executor.spin_node_once(node, std::chrono::milliseconds(0));
      while (counter == 1 && i < max_loops) {
        printf("spin_node_once(nonblocking) - callback (2) expected - try %d/%d\n", ++i, max_loops);
        std::this_thread::sleep_for(sleep_per_loop);
        executor.spin_node_once(node, std::chrono::milliseconds(0));
      }
    }
    ASSERT_EQ(2, counter);

    // check for next pending call
    {
      int i = 0;
      executor.spin_node_once(node, std::chrono::milliseconds(0));
      while (counter == 2 && i < max_loops) {
        printf("spin_node_once(nonblocking) - callback (3) expected - try %d/%d\n", ++i, max_loops);
        std::this_thread::sleep_for(sleep_per_loop);
        executor.spin_node_once(node, std::chrono::milliseconds(0));
      }
    }
    ASSERT_EQ(3, counter);

    // check for all remaning calls
    {
      printf("spin_node_some() - callbacks (4 and 5) expected\n");
      int i = 0;
      executor.spin_node_once(node, std::chrono::milliseconds(0));
      while ((counter == 3 || counter == 4) && i < max_loops) {
        printf(
          "spin_node_some() - callback (%s) expected - try %d/%d\n",
          counter == 3 ? "4 and 5" : "5", ++i, max_loops);
        std::this_thread::sleep_for(sleep_per_loop);
        executor.spin_node_once(node, std::chrono::milliseconds(0));
      }
    }
    ASSERT_EQ(5, counter);
  }
  // the subscriber goes out of scope and should be not receive any callbacks anymore

  msg.data = 6;
  publisher->publish(msg);

  std::this_thread::sleep_for(std::chrono::milliseconds(25));

  // check that no further callbacks have been invoked
  printf("spin_node_some() - no callbacks expected\n");
  executor.spin_node_some(node);
  ASSERT_EQ(5, counter);
}
