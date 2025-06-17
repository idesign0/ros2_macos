// Copyright 2015-2017 Open Source Robotics Foundation, Inc.
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
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "rclcpp/rclcpp.hpp"

#include "test_msgs/message_fixtures.hpp"

using namespace std::chrono_literals;

template<typename T>
rclcpp::SubscriptionBase::SharedPtr attempt_subscribe(
  rclcpp::Node::SharedPtr node,
  const std::string & topic_name,
  std::vector<typename T::SharedPtr> & expected_messages,
  std::vector<bool> & received_messages)
{
  received_messages.assign(expected_messages.size(), false);

  auto callback =
    [&expected_messages, &received_messages](
    const typename T::ConstSharedPtr received_message
    ) -> void
    {
      // find received message in vector of expected messages
      auto received = received_messages.begin();
      bool known_message = false;
      size_t index = 0;
      for (auto expected_message : expected_messages) {
        if (*received_message == *expected_message) {
          *received = true;
          printf("received message #%zu of %zu\n", index + 1, expected_messages.size());
          known_message = true;
          break;
        }
        ++received;
        ++index;
      }
      if (!known_message) {
        fprintf(stderr, "received message does not match any expected message\n");
        rclcpp::shutdown();
        throw std::runtime_error("received message does not match any expected message");
      }

      // shutdown node when all expected messages have been received
      for (auto received_msg : received_messages) {
        if (!received_msg) {
          return;
        }
      }
      rclcpp::shutdown();
    };

  auto subscriber = node->create_subscription<T>(topic_name, expected_messages.size(), callback);
  return subscriber;
}

template<typename T>
rclcpp::SubscriptionBase::SharedPtr attempt_subscribe(
  rclcpp::Node::SharedPtr node,
  const std::string & topic_name,
  bool & sub_callback_called,
  rclcpp::executors::SingleThreadedExecutor & exec)
{
  auto subscription_callback =
    [&sub_callback_called, &exec](const typename T::ConstSharedPtr) -> void
    {
      printf("***SUB_CALLBACK***\n");
      sub_callback_called = true;
      exec.cancel();
    };

  auto subscriber = node->create_subscription<T>(topic_name, 10, subscription_callback);
  return subscriber;
}

rclcpp::TimerBase::SharedPtr create_timer(
  rclcpp::Node::SharedPtr node,
  bool & timer_callback_called,
  rclcpp::executors::SingleThreadedExecutor & exec)
{
  auto timer_callback =
    [&timer_callback_called, &exec]() -> void
    {
      printf("***TIMER_CALLBACK***\n");
      timer_callback_called = true;
      exec.cancel();
    };
  auto timer = node->create_wall_timer(8s, timer_callback, nullptr);

  return timer;
}


int main(int argc, char ** argv)
{
  if (argc != 4) {
    fprintf(
      stderr,
      "Wrong number of arguments,\n"
      "pass a node name, a topic name and a should_timeout boolean\n");
    return 1;
  }
  std::string message = argv[1];
  std::string namespace_ = argv[3];
  std::string node_name = "test_secure_subscriber";
  std::string topic_name = "chatter";
  bool should_timeout =
    ((0 == strcmp(argv[2], "false")) || (0 == strcmp(argv[2], "0"))) ? false : true;

  const char * args[] = {"--ros-args", "--enclave", "/subscriber"};
  rclcpp::init(sizeof(args) / sizeof(char *), args);
  std::shared_ptr<rclcpp::Node> node = nullptr;
  try {
    node = rclcpp::Node::make_shared(node_name, namespace_);
  } catch (std::runtime_error & exc) {
    fprintf(stderr, "should not have thrown!\n%s\n", exc.what());
    rclcpp::shutdown();
    return 1;
  }
  rclcpp::SubscriptionBase::SharedPtr subscriber;

  if (!should_timeout) {
    std::vector<bool> received_messages;  // collect flags about received messages
    auto messages_empty = get_messages_empty();
    auto messages_basic_types = get_messages_basic_types();
    auto messages_arrays = get_messages_arrays();
    auto messages_unbounded_sequences = get_messages_unbounded_sequences();
    auto messages_bounded_sequences = get_messages_bounded_sequences();
    auto messages_nested = get_messages_nested();
    auto messages_multi_nested = get_messages_multi_nested();
    auto messages_strings = get_messages_strings();
    auto messages_constants = get_messages_constants();
    auto messages_defaults = get_messages_defaults();
    auto messages_builtins = get_messages_builtins();
    if (message == "Empty") {
      subscriber = attempt_subscribe<test_msgs::msg::Empty>(
        node, topic_name, messages_empty, received_messages);
    } else if (message == "UnboundedSequences") {
      subscriber = attempt_subscribe<test_msgs::msg::UnboundedSequences>(
        node, topic_name, messages_unbounded_sequences, received_messages);
    } else {
      fprintf(stderr, "Unknown message argument '%s'\n", message.c_str());
      rclcpp::shutdown();
      return 1;
    }

    rclcpp::spin(node);
  } else {
    bool timer_callback_called = false;
    bool sub_callback_called = false;
    rclcpp::TimerBase::SharedPtr timer;
    rclcpp::executors::SingleThreadedExecutor executor;
    if (message == "Empty") {
      subscriber = attempt_subscribe<test_msgs::msg::Empty>(
        node, topic_name, sub_callback_called, executor);
    } else if (message == "UnboundedSequences") {
      subscriber = attempt_subscribe<test_msgs::msg::UnboundedSequences>(
        node, topic_name, sub_callback_called, executor);
    } else {
      fprintf(stderr, "Unknown message argument '%s'\n", message.c_str());
      rclcpp::shutdown();
      return 1;
    }
    timer = create_timer(node, timer_callback_called, executor);
    executor.add_node(node);
    executor.spin();
    if (!timer_callback_called || sub_callback_called) {
      rclcpp::shutdown();
      return 1;
    }
  }

  rclcpp::shutdown();
  return 0;
}
