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

#ifndef SUBSCRIBE_ARRAY_TYPES_HPP_
#define SUBSCRIBE_ARRAY_TYPES_HPP_

#include <string>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "test_msgs/msg/arrays.hpp"
#include "test_msgs/msg/unbounded_sequences.hpp"
#include "test_msgs/msg/bounded_plain_sequences.hpp"
#include "test_msgs/msg/bounded_sequences.hpp"
#include "test_msgs/msg/multi_nested.hpp"
#include "test_msgs/msg/nested.hpp"

rclcpp::SubscriptionBase::SharedPtr subscribe_arrays(
  rclcpp::Node::SharedPtr node,
  const std::string & message_type,
  const std::vector<test_msgs::msg::Arrays::SharedPtr> & expected_messages,
  std::vector<bool> & received_messages);

rclcpp::SubscriptionBase::SharedPtr subscribe_unbounded_sequences(
  rclcpp::Node::SharedPtr node,
  const std::string & message_type,
  const std::vector<test_msgs::msg::UnboundedSequences::SharedPtr> & expected_messages,
  std::vector<bool> & received_messages);

rclcpp::SubscriptionBase::SharedPtr subscribe_bounded_plain_sequences(
  rclcpp::Node::SharedPtr node,
  const std::string & message_type,
  const std::vector<test_msgs::msg::BoundedPlainSequences::SharedPtr> & expected_messages,
  std::vector<bool> & received_messages);

rclcpp::SubscriptionBase::SharedPtr subscribe_bounded_sequences(
  rclcpp::Node::SharedPtr node,
  const std::string & message_type,
  const std::vector<test_msgs::msg::BoundedSequences::SharedPtr> & expected_messages,
  std::vector<bool> & received_messages);

rclcpp::SubscriptionBase::SharedPtr subscribe_multi_nested(
  rclcpp::Node::SharedPtr node,
  const std::string & message_type,
  const std::vector<test_msgs::msg::MultiNested::SharedPtr> & expected_messages,
  std::vector<bool> & received_messages);

rclcpp::SubscriptionBase::SharedPtr subscribe_nested(
  rclcpp::Node::SharedPtr node,
  const std::string & message_type,
  const std::vector<test_msgs::msg::Nested::SharedPtr> & expected_messages,
  std::vector<bool> & received_messages);

#endif  // SUBSCRIBE_ARRAY_TYPES_HPP_
