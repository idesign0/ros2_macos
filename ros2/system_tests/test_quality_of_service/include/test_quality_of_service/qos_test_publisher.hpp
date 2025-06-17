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

#ifndef TEST_QUALITY_OF_SERVICE__QOS_TEST_PUBLISHER_HPP_
#define TEST_QUALITY_OF_SERVICE__QOS_TEST_PUBLISHER_HPP_

#include <chrono>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

#include "test_quality_of_service/qos_test_node.hpp"

/// Simple publishing node used for system tests
class QosTestPublisher : public QosTestNode
{
public:
  QosTestPublisher(
    const std::string & name,
    const std::string & topic,
    const rclcpp::QoS & qos_options,
    const std::chrono::milliseconds & publish_period);

  virtual ~QosTestPublisher() = default;
  QosTestPublisher(QosTestPublisher const &) = delete;
  QosTestPublisher & operator=(QosTestPublisher const &) = delete;

  rclcpp::PublisherOptions & options() {return publisher_options_;}

  void teardown() override;

private:
  void publish_message();
  void setup_start() override;
  void setup_stop() override;

  /// publisher options needed for QoS callbacks
  rclcpp::PublisherOptions publisher_options_;
  /// publishing period
  const std::chrono::milliseconds publish_period_;
  /// the timer of this publisher
  rclcpp::TimerBase::SharedPtr timer_;

  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
};

#endif  // TEST_QUALITY_OF_SERVICE__QOS_TEST_PUBLISHER_HPP_
