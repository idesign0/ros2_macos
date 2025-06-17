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

#include <string>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "test_msgs/msg/empty.hpp"
#include "test_msgs/msg/basic_types.hpp"
#include "test_msgs/msg/builtins.hpp"
#include "test_msgs/msg/constants.hpp"
#include "test_msgs/msg/defaults.hpp"

#include "subscribe_basic_types.hpp"
#include "subscribe_helper.hpp"

rclcpp::SubscriptionBase::SharedPtr subscribe_empty(
  rclcpp::Node::SharedPtr node,
  const std::string & message_type,
  const std::vector<test_msgs::msg::Empty::SharedPtr> & messages_expected,
  std::vector<bool> & received_messages)
{
  return subscribe<test_msgs::msg::Empty>(node, message_type, messages_expected, received_messages);
}

rclcpp::SubscriptionBase::SharedPtr subscribe_basic_types(
  rclcpp::Node::SharedPtr node,
  const std::string & message_type,
  const std::vector<test_msgs::msg::BasicTypes::SharedPtr> & messages_expected,
  std::vector<bool> & received_messages)
{
  return subscribe<test_msgs::msg::BasicTypes>(
    node, message_type, messages_expected, received_messages);
}

rclcpp::SubscriptionBase::SharedPtr subscribe_builtins(
  rclcpp::Node::SharedPtr node,
  const std::string & message_type,
  const std::vector<test_msgs::msg::Builtins::SharedPtr> & messages_expected,
  std::vector<bool> & received_messages)
{
  return subscribe<test_msgs::msg::Builtins>(
    node, message_type, messages_expected, received_messages);
}

rclcpp::SubscriptionBase::SharedPtr subscribe_constants(
  rclcpp::Node::SharedPtr node,
  const std::string & message_type,
  const std::vector<test_msgs::msg::Constants::SharedPtr> & messages_expected,
  std::vector<bool> & received_messages)
{
  return subscribe<test_msgs::msg::Constants>(
    node, message_type, messages_expected, received_messages);
}

rclcpp::SubscriptionBase::SharedPtr subscribe_defaults(
  rclcpp::Node::SharedPtr node,
  const std::string & message_type,
  const std::vector<test_msgs::msg::Defaults::SharedPtr> & messages_expected,
  std::vector<bool> & received_messages)
{
  return subscribe<test_msgs::msg::Defaults>(
    node, message_type, messages_expected, received_messages);
}
