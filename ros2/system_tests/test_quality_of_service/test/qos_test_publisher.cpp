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

#include <string>

#include "test_quality_of_service/qos_test_publisher.hpp"
#include "test_quality_of_service/qos_test_node.hpp"

QosTestPublisher::QosTestPublisher(
  const std::string & name,
  const std::string & topic,
  const rclcpp::QoS & qos_options,
  const std::chrono::milliseconds & publish_period)
: QosTestNode(name, topic, qos_options),
  publish_period_(publish_period),
  timer_(nullptr),
  publisher_(nullptr)
{
  RCLCPP_INFO(this->get_logger(), "created publisher %s %s\n", name.c_str(), topic.c_str());
}

void QosTestPublisher::publish_message()
{
  auto message = std_msgs::msg::String();
  std::ostringstream message_buffer;
  message_buffer << this->name_ << ": testing " << std::to_string(this->increment_count());
  message.data = message_buffer.str();

  RCLCPP_INFO(
    this->get_logger(), "%s: publishing '%s'", this->name_.c_str(),
    message.data.c_str());
  this->publisher_->publish(message);
}

void QosTestPublisher::setup_start()
{
  if (!publisher_) {
    publisher_ = this->create_publisher<std_msgs::msg::String>(
      topic_,
      qos_options_,
      publisher_options_);
  }
  if (!timer_) {
    timer_ = this->create_wall_timer(
      publish_period_,
      std::bind(&QosTestPublisher::publish_message, this));
  }
}

void QosTestPublisher::setup_stop()
{
  if (timer_) {
    timer_->cancel();
    timer_ = nullptr;
  }
}

void QosTestPublisher::teardown()
{
  setup_stop();

  if (publisher_) {
    publisher_.reset();
  }
}
