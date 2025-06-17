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
#include <vector>

#include "rclcpp/rclcpp.hpp"

#include "test_msgs/message_fixtures.hpp"

template<typename T>
void publish(
  rclcpp::Node::SharedPtr node,
  const std::string & message_type,
  std::vector<typename T::SharedPtr> messages,
  size_t number_of_cycles = 100)
{
  auto start = std::chrono::steady_clock::now();

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

  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<float> diff = (end - start);
  printf("published for %f seconds\n", diff.count());
}

int main(int argc, char ** argv)
{
  if (argc != 3) {
    fprintf(stderr, "Wrong number of arguments, pass one message type\n");
    return 1;
  }
  rclcpp::init(argc, argv);

  std::string message = argv[1];
  std::string namespace_ = argv[2];
  auto node = rclcpp::Node::make_shared(
    std::string("test_publisher_") + message, namespace_);

  if (message == "Empty") {
    publish<test_msgs::msg::Empty>(node, message, get_messages_empty());
  } else if (message == "BasicTypes") {
    publish<test_msgs::msg::BasicTypes>(node, message, get_messages_basic_types());
  } else if (message == "Arrays") {
    publish<test_msgs::msg::Arrays>(
      node, message, get_messages_arrays());
  } else if (message == "UnboundedSequences") {
    publish<test_msgs::msg::UnboundedSequences>(
      node, message, get_messages_unbounded_sequences());
  } else if (message == "BoundedPlainSequences") {
    publish<test_msgs::msg::BoundedPlainSequences>(
      node, message, get_messages_bounded_plain_sequences());
  } else if (message == "BoundedSequences") {
    publish<test_msgs::msg::BoundedSequences>(
      node, message, get_messages_bounded_sequences());
  } else if (message == "MultiNested") {
    publish<test_msgs::msg::MultiNested>(node, message, get_messages_multi_nested());
  } else if (message == "Nested") {
    publish<test_msgs::msg::Nested>(node, message, get_messages_nested());
  } else if (message == "Builtins") {
    publish<test_msgs::msg::Builtins>(node, message, get_messages_builtins());
  } else if (message == "Constants") {
    publish<test_msgs::msg::Constants>(node, message, get_messages_constants());
  } else if (message == "Defaults") {
    publish<test_msgs::msg::Defaults>(node, message, get_messages_defaults());
  } else if (message == "Strings") {
    publish<test_msgs::msg::Strings>(node, message, get_messages_strings());
  } else if (message == "WStrings") {
    publish<test_msgs::msg::WStrings>(node, message, get_messages_wstrings());
  } else {
    fprintf(stderr, "Unknown message argument '%s'\n", message.c_str());
    rclcpp::shutdown();
    return 1;
  }
  rclcpp::shutdown();
  return 0;
}
