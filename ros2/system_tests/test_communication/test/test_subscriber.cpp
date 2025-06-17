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
#include <vector>

#include "rclcpp/rclcpp.hpp"

#include "test_msgs/message_fixtures.hpp"

#include "subscribe_array_types.hpp"
#include "subscribe_basic_types.hpp"
#include "subscribe_string_types.hpp"

int main(int argc, char ** argv)
{
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <message_type> <namespace>\n", argv[0]);
    return 1;
  }
  rclcpp::init(argc, argv);

  auto start = std::chrono::steady_clock::now();

  std::string message = argv[1];
  std::string namespace_ = argv[2];
  auto node = rclcpp::Node::make_shared(
    std::string("test_subscriber_") + message, namespace_);

  auto messages_empty = get_messages_empty();
  auto messages_basic_types = get_messages_basic_types();
  auto messages_arrays = get_messages_arrays();
  auto messages_unbounded_sequences = get_messages_unbounded_sequences();
  auto messages_bounded_plain_sequences = get_messages_bounded_plain_sequences();
  auto messages_bounded_sequences = get_messages_bounded_sequences();
  auto messages_multi_nested = get_messages_multi_nested();
  auto messages_nested = get_messages_nested();
  auto messages_builtins = get_messages_builtins();
  auto messages_constants = get_messages_constants();
  auto messages_defaults = get_messages_defaults();
  auto messages_strings = get_messages_strings();
  auto messages_wstrings = get_messages_wstrings();

  rclcpp::SubscriptionBase::SharedPtr subscriber;
  std::vector<bool> received_messages;  // collect flags about received messages
  if (message == "Empty") {
    subscriber = subscribe_empty(node, message, messages_empty, received_messages);
  } else if (message == "BasicTypes") {
    subscriber = subscribe_basic_types(node, message, messages_basic_types, received_messages);
  } else if (message == "Arrays") {
    subscriber = subscribe_arrays(node, message, messages_arrays, received_messages);
  } else if (message == "UnboundedSequences") {
    subscriber = subscribe_unbounded_sequences(
      node, message, messages_unbounded_sequences, received_messages);
  } else if (message == "BoundedPlainSequences") {
    subscriber = subscribe_bounded_plain_sequences(
      node, message, messages_bounded_plain_sequences, received_messages);
  } else if (message == "BoundedSequences") {
    subscriber = subscribe_bounded_sequences(
      node, message, messages_bounded_sequences, received_messages);
  } else if (message == "MultiNested") {
    subscriber = subscribe_multi_nested(node, message, messages_multi_nested, received_messages);
  } else if (message == "Nested") {
    subscriber = subscribe_nested(node, message, messages_nested, received_messages);
  } else if (message == "Builtins") {
    subscriber = subscribe_builtins(node, message, messages_builtins, received_messages);
  } else if (message == "Constants") {
    subscriber = subscribe_constants(node, message, messages_constants, received_messages);
  } else if (message == "Defaults") {
    subscriber = subscribe_defaults(node, message, messages_defaults, received_messages);
  } else if (message == "Strings") {
    subscriber = subscribe_strings(node, message, messages_strings, received_messages);
  } else if (message == "WStrings") {
    subscriber = subscribe_wstrings(node, message, messages_wstrings, received_messages);
  } else {
    fprintf(stderr, "Unknown message argument '%s'\n", message.c_str());
    rclcpp::shutdown();
    return 1;
  }

  try {
    rclcpp::spin(node);
  } catch (const std::exception & e) {
    fprintf(stderr, "%s\n", e.what());
    rclcpp::shutdown();
    return 1;
  }

  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<float> diff = (end - start);
  printf("subscribed for %f seconds\n", diff.count());

  rclcpp::shutdown();
  return 0;
}
