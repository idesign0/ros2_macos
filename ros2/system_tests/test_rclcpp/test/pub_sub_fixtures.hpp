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

#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"

#include "test_rclcpp/msg/u_int32.hpp"
#include "test_rclcpp/utils.hpp"

#ifndef PUB_SUB_FIXTURES_HPP_
#define PUB_SUB_FIXTURES_HPP_

template<typename MessageT>
void
single_message_pub_sub_fixture(
  const std::string & topic_name,
  int & counter,
  std::function<
    typename rclcpp::Subscription<MessageT>::SharedPtr(
      rclcpp::Node::SharedPtr,
      const std::string &
    )
  > create_subscription_func,
  std::function<
    void(typename rclcpp::Publisher<MessageT>::SharedPtr, MessageT)
  > publish_func,
  const rclcpp::QoS & custom_qos = rclcpp::QoS(rclcpp::KeepLast(10)),
  std::function<void(rclcpp::executors::SingleThreadedExecutor &)> pre_subscription_hook = nullptr,
  size_t max_retries = 3,  // number of times it will try to publish
  size_t max_loops = 200,  // number of times it will check for data
  std::chrono::milliseconds sleep_per_loop = std::chrono::milliseconds(10))
{
  auto node = rclcpp::Node::make_shared(topic_name + "_node");

  auto publisher = node->create_publisher<MessageT>(topic_name, custom_qos);

  MessageT msg;
  msg.data = 0;
  rclcpp::executors::SingleThreadedExecutor executor;

  // optionally call the pre subscription hook
  if (pre_subscription_hook != nullptr) {
    pre_subscription_hook(executor);
  }

  // wait a moment for a "clean" state with no subscription
  test_rclcpp::wait_for_subscriber(node, topic_name, false);

  // call custom create subscription function
  auto sub = create_subscription_func(node, topic_name);

  // wait a moment for everything to initialize
  test_rclcpp::wait_for_subscriber(node, topic_name);

  // start condition
  ASSERT_EQ(0, counter);

  // nothing should be pending here
  executor.spin_node_some(node);
  ASSERT_EQ(0, counter);

  // try to send data N times
  // this is necessary because sometimes the first message does not go through
  // this is very common with Connext due to a race condition
  size_t retry = 0;
  msg.data = 0;
  while (retry < max_retries && counter == 0) {
    msg.data++;
    // call custom publish function
    publish_func(publisher, msg);

    // wait for the first callback
    printf("spin_node_some() - callback (1) expected\n");

    executor.spin_node_some(node);
    // spin for up to 2s
    size_t loop = 0;
    while ((counter != 1) && (loop++ < max_loops)) {
      // printf("callback not called, sleeping and trying again\n");
      std::this_thread::sleep_for(sleep_per_loop);
      executor.spin_node_some(node);
    }

    if (counter == 0) {
      printf("  callback was not called, trying to publish again\n");
    }
  }

  ASSERT_EQ(1, counter);
}

#endif  // PUB_SUB_FIXTURES_HPP_
