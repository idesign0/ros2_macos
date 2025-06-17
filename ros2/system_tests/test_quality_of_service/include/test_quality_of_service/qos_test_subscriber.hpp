// Copyright 2019 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

#ifndef TEST_QUALITY_OF_SERVICE__QOS_TEST_SUBSCRIBER_HPP_
#define TEST_QUALITY_OF_SERVICE__QOS_TEST_SUBSCRIBER_HPP_

#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

#include "test_quality_of_service/qos_test_node.hpp"

/// Simple subscriber node used for system tests
class QosTestSubscriber : public QosTestNode
{
public:
  QosTestSubscriber(
    const std::string & name,
    const std::string & topic,
    const rclcpp::QoS & qos_options);

  virtual ~QosTestSubscriber() = default;
  QosTestSubscriber(QosTestSubscriber const &) = delete;
  QosTestSubscriber & operator=(QosTestSubscriber const &) = delete;

  rclcpp::SubscriptionOptions & options() {return sub_options_;}

  void teardown() override;

private:
  void listen_to_message(std_msgs::msg::String::ConstSharedPtr);
  void setup_start() override;
  void setup_stop() override;

  /// subscription options needed for QoS callbacks
  rclcpp::SubscriptionOptions sub_options_;

  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
};

#endif  // TEST_QUALITY_OF_SERVICE__QOS_TEST_SUBSCRIBER_HPP_
