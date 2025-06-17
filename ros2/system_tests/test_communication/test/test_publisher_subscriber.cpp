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
#include <string>
#include <thread>
#include <vector>

#include "rclcpp/rclcpp.hpp"

#include "rcpputils/scope_exit.hpp"

#include "test_msgs/message_fixtures.hpp"

#include "subscribe_array_types.hpp"
#include "subscribe_basic_types.hpp"
#include "subscribe_string_types.hpp"

template<typename T>
void publish(
  rclcpp::Node::SharedPtr node,
  const std::string & message_type,
  std::vector<typename T::SharedPtr> messages,
  size_t number_of_cycles = 100)
{
  auto qos = rclcpp::QoS(rclcpp::KeepLast(messages.size()));

  auto publisher = node->create_publisher<T>(std::string("test/message/") + message_type, qos);

  rclcpp::WallRate cycle_rate(10);
  rclcpp::WallRate message_rate(100);
  size_t cycle_index = 0;
  // publish all messages up to number_of_cycles times, longer sleep between each cycle
  while (rclcpp::ok() && cycle_index < number_of_cycles) {
    size_t message_index = 0;
    // publish all messages one by one, shorter sleep between each message
    while (rclcpp::ok() && message_index < messages.size()) {
      printf("publishing message #%zu\n", message_index + 1);
      publisher->publish(*messages[message_index]);
      ++message_index;
      message_rate.sleep();
    }
    ++cycle_index;
    cycle_rate.sleep();
  }
}

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  RCPPUTILS_SCOPE_EXIT(
  {
    rclcpp::shutdown();
  });
  if (argc != 2) {
    fprintf(stderr, "Wrong number of arguments, pass one message type\n");
    return 1;
  }

  auto start = std::chrono::steady_clock::now();

  std::string message = argv[1];
  auto node = rclcpp::Node::make_shared(std::string("test_publisher_subscriber_") + message);

  rclcpp::SubscriptionBase::SharedPtr subscriber;
  std::vector<bool> received_messages;  // collect flags about received messages

  auto messages_empty = get_messages_empty();
  auto messages_basic_types = get_messages_basic_types();
  auto messages_arrays = get_messages_arrays();
  auto messages_bounded_plain_sequences = get_messages_bounded_plain_sequences();
  auto messages_bounded_sequences = get_messages_bounded_sequences();
  auto messages_unbounded_sequences = get_messages_unbounded_sequences();
  auto messages_multi_nested = get_messages_multi_nested();
  auto messages_nested = get_messages_nested();
  auto messages_builtins = get_messages_builtins();
  auto messages_constants = get_messages_constants();
  auto messages_defaults = get_messages_defaults();
  auto messages_strings = get_messages_strings();
  auto messages_wstrings = get_messages_wstrings();

  std::thread spin_thread([node]() {
      rclcpp::spin(node);
    });

  if (message == "Empty") {
    subscriber = subscribe_empty(node, message, messages_empty, received_messages);
    publish<test_msgs::msg::Empty>(node, message, messages_empty);
  } else if (message == "BasicTypes") {
    subscriber = subscribe_basic_types(node, message, messages_basic_types, received_messages);
    publish<test_msgs::msg::BasicTypes>(node, message, messages_basic_types);
  } else if (message == "Arrays") {
    subscriber = subscribe_arrays(node, message, messages_arrays, received_messages);
    publish<test_msgs::msg::Arrays>(node, message, messages_arrays);
  } else if (message == "UnboundedSequences") {
    subscriber = subscribe_unbounded_sequences(
      node, message, messages_unbounded_sequences, received_messages);
    publish<test_msgs::msg::UnboundedSequences>(
      node, message, messages_unbounded_sequences);
  } else if (message == "BoundedPlainSequences") {
    subscriber = subscribe_bounded_plain_sequences(
      node, message, messages_bounded_plain_sequences, received_messages);
    publish<test_msgs::msg::BoundedPlainSequences>(
      node, message, messages_bounded_plain_sequences);
  } else if (message == "BoundedSequences") {
    subscriber = subscribe_bounded_sequences(
      node, message, messages_bounded_sequences, received_messages);
    publish<test_msgs::msg::BoundedSequences>(
      node, message, messages_bounded_sequences);
  } else if (message == "MultiNested") {
    subscriber = subscribe_multi_nested(node, message, messages_multi_nested, received_messages);
    publish<test_msgs::msg::MultiNested>(node, message, messages_multi_nested);
  } else if (message == "Nested") {
    subscriber = subscribe_nested(node, message, messages_nested, received_messages);
    publish<test_msgs::msg::Nested>(node, message, messages_nested);
  } else if (message == "Builtins") {
    subscriber = subscribe_builtins(node, message, messages_builtins, received_messages);
    publish<test_msgs::msg::Builtins>(node, message, messages_builtins);
  } else if (message == "Constants") {
    subscriber = subscribe_constants(node, message, messages_constants, received_messages);
    publish<test_msgs::msg::Constants>(node, message, messages_constants);
  } else if (message == "Defaults") {
    subscriber = subscribe_defaults(node, message, messages_defaults, received_messages);
    publish<test_msgs::msg::Defaults>(node, message, messages_defaults);
  } else if (message == "Strings") {
    subscriber = subscribe_strings(node, message, messages_strings, received_messages);
    publish<test_msgs::msg::Strings>(node, message, messages_strings);
  } else if (message == "WStrings") {
    subscriber = subscribe_wstrings(node, message, messages_wstrings, received_messages);
    publish<test_msgs::msg::WStrings>(node, message, messages_wstrings);
  } else {
    fprintf(stderr, "Unknown message argument '%s'\n", message.c_str());
    return 1;
  }

  spin_thread.join();

  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<float> diff = (end - start);
  printf("published and subscribed for %f seconds\n", diff.count());

  for (auto received : received_messages) {
    if (!received) {
      return 1;
    }
  }

  return 0;
}
