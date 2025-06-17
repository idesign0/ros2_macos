// Copyright 2019 Open Source Robotics Foundation, Inc.
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
#include <tuple>

#include "gtest/gtest.h"

#include "rclcpp/rclcpp.hpp"
#include "rclcpp/executors/single_threaded_executor.hpp"

#include "std_msgs/msg/string.hpp"

#include "test_quality_of_service/qos_test_publisher.hpp"
#include "test_quality_of_service/qos_test_subscriber.hpp"
#include "test_quality_of_service/qos_utilities.hpp"

using namespace std::chrono_literals;

/// Test Deadline with a single publishing node and single subscriber node
TEST_F(QosRclcppTestFixture, test_deadline) {
  int expected_number_of_events = 4;
  // Bump deadline duration when testing against rmw_connextdds to
  // cope with the longer discovery times it entails.
  const std::chrono::milliseconds deadline_duration =
    this_rmw_implementation.find("connext") != std::string::npos ? 2s : 1s;
  const std::chrono::milliseconds publisher_toggling_period = deadline_duration * 2;
  const std::chrono::milliseconds max_test_length =
    publisher_toggling_period * (expected_number_of_events + 1);
  const std::chrono::milliseconds publish_rate = deadline_duration / expected_number_of_events;

  // used for lambda capture
  int total_number_of_publisher_deadline_events = 0;
  int total_number_of_subscriber_deadline_events = 0;
  int last_pub_count = 0;
  int last_sub_count = 0;

  rclcpp::QoS qos_profile(10);
  qos_profile.deadline(deadline_duration);

  const std::string topic("test_deadline");

  publisher = std::make_shared<QosTestPublisher>(
    "publisher", topic, qos_profile, publish_rate);
  subscriber = std::make_shared<QosTestSubscriber>(
    "subscriber", topic, qos_profile);

  // setup publishing options and callback
  publisher->options().event_callbacks.deadline_callback =
    [this, &last_pub_count, &total_number_of_publisher_deadline_events](
    rclcpp::QOSDeadlineOfferedInfo & event) -> void
    {
      RCLCPP_INFO(publisher->get_logger(), "QOSDeadlineOfferedInfo callback");
      total_number_of_publisher_deadline_events++;
      // assert the correct value on a change
      ASSERT_EQ(1, event.total_count_change);
      last_pub_count = event.total_count;
    };

  // setup subscription options and callback
  subscriber->options().event_callbacks.deadline_callback =
    [this, &last_sub_count, &total_number_of_subscriber_deadline_events](
    rclcpp::QOSDeadlineRequestedInfo & event) -> void
    {
      RCLCPP_INFO(subscriber->get_logger(), "QOSDeadlineRequestedInfo callback");
      total_number_of_subscriber_deadline_events++;
      // assert the correct value on a change
      ASSERT_EQ(1, event.total_count_change);
      last_sub_count = event.total_count;
    };

  // toggle publishing on and off to force deadline events
  rclcpp::TimerBase::SharedPtr toggle_publisher_timer = subscriber->create_wall_timer(
    publisher_toggling_period,
    [this]() -> void {
      // start / stop publishing to trigger deadline
      publisher->toggle();
    });

  executor->add_node(publisher);
  executor->add_node(subscriber);

  publisher->start();
  subscriber->start();

  // the future will never be resolved, so simply time out to force the experiment to stop
  executor->spin_until_future_complete(dummy_future, max_test_length);
  toggle_publisher_timer->cancel();

  EXPECT_GT(publisher->get_count(), 0);  // check if we published anything
  EXPECT_GT(subscriber->get_count(), 0);  // check if we received anything

  // check to see if callbacks fired as expected
  EXPECT_EQ(expected_number_of_events, total_number_of_subscriber_deadline_events);
  EXPECT_EQ(expected_number_of_events, total_number_of_publisher_deadline_events);

  // check values reported by the callback
  EXPECT_EQ(expected_number_of_events, last_pub_count);
  EXPECT_EQ(expected_number_of_events, last_sub_count);
}
