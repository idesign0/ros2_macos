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
#include <string>
#include <vector>

#include "rclcpp/rclcpp.hpp"

#include "test_msgs/message_fixtures.hpp"

template<typename T>
int8_t attempt_publish(
  rclcpp::Node::SharedPtr node,
  const std::string & topic_name,
  std::vector<typename T::SharedPtr> messages,
  size_t number_of_cycles = 150)
{
  auto start = std::chrono::steady_clock::now();

  auto publisher = node->create_publisher<T>(topic_name, messages.size());

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

  return 0;
}

int main(int argc, char ** argv)
{
  if (argc != 3) {
    fprintf(
      stderr,
      "Wrong number of arguments,\n"
      "pass a message type\n");
    return 1;
  }
  const char * args[] = {"--ros-args", "--enclave", "/publisher"};
  rclcpp::init(sizeof(args) / sizeof(char *), args);
  std::string message = argv[1];
  std::string namespace_ = argv[2];
  std::string node_name = "test_secure_publisher";
  std::string topic_name = "chatter";
  std::shared_ptr<rclcpp::Node> node = nullptr;
  try {
    node = rclcpp::Node::make_shared(node_name, namespace_);
  } catch (std::runtime_error & exc) {
    fprintf(stderr, "should not have thrown!\n%s\n", exc.what());
    rclcpp::shutdown();
    return 1;
  }
  printf("node created, attempt to publish\n");
  int8_t ret;

  if (message == "Empty") {
    ret = attempt_publish<test_msgs::msg::Empty>(
      node, topic_name, get_messages_empty());
  } else if (message == "BasicTypes") {
    ret = attempt_publish<test_msgs::msg::BasicTypes>(
      node, topic_name, get_messages_basic_types());
  } else if (message == "Arrays") {
    ret = attempt_publish<test_msgs::msg::Arrays>(
      node, topic_name, get_messages_arrays());
  } else if (message == "UnboundedSequences") {
    ret = attempt_publish<test_msgs::msg::UnboundedSequences>(
      node, topic_name, get_messages_unbounded_sequences());
  } else if (message == "BoundedSequences") {
    ret = attempt_publish<test_msgs::msg::BoundedSequences>(
      node, topic_name, get_messages_bounded_sequences());
  } else if (message == "Nested") {
    ret = attempt_publish<test_msgs::msg::Nested>(
      node, topic_name, get_messages_nested());
  } else if (message == "MultiNested") {
    ret = attempt_publish<test_msgs::msg::MultiNested>(
      node, topic_name, get_messages_multi_nested());
  } else if (message == "Strings") {
    ret = attempt_publish<test_msgs::msg::Strings>(
      node, topic_name, get_messages_strings());
  } else if (message == "Constants") {
    ret = attempt_publish<test_msgs::msg::Constants>(
      node, topic_name, get_messages_constants());
  } else if (message == "Defaults") {
    ret = attempt_publish<test_msgs::msg::Defaults>(
      node, topic_name, get_messages_defaults());
  } else if (message == "Builtins") {
    ret = attempt_publish<test_msgs::msg::Builtins>(
      node, topic_name, get_messages_builtins());
  } else {
    fprintf(stderr, "Unknown message argument '%s'\n", message.c_str());
    rclcpp::shutdown();
    return 1;
  }
  rclcpp::shutdown();
  return ret;
}
