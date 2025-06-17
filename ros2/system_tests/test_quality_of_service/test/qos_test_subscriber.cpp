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

#include <mutex>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

#include "test_quality_of_service/qos_test_node.hpp"
#include "test_quality_of_service/qos_test_subscriber.hpp"

using std::placeholders::_1;

QosTestSubscriber::QosTestSubscriber(
  const std::string & name,
  const std::string & topic,
  const rclcpp::QoS & qos_options)
: QosTestNode(name, topic, qos_options),
  subscription_(nullptr)
{
  RCLCPP_INFO(this->get_logger(), "created subscriber %s %s", name.c_str(), topic.c_str());
}

void QosTestSubscriber::listen_to_message(
  const std_msgs::msg::String::ConstSharedPtr received_message)
{
  RCLCPP_INFO(
    this->get_logger(), "%s: subscriber heard [%s]", this->name_.c_str(),
    received_message->data.c_str());
  this->increment_count();
}

void QosTestSubscriber::setup_start()
{
  if (!subscription_) {
    subscription_ = create_subscription<std_msgs::msg::String>(
      topic_,
      qos_options_,
      std::bind(&QosTestSubscriber::listen_to_message, this, _1),
      sub_options_);
  }
}

void QosTestSubscriber::setup_stop()
{
  if (subscription_) {
    subscription_.reset();
  }
}

void QosTestSubscriber::teardown()
{
  stop();
}
