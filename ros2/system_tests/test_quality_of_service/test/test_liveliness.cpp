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
#include <exception>
#include <iostream>
#include <memory>
#include <stack>
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

/// Test Automatic Liveliness with a single publishing node and single subscriber node
TEST_F(QosRclcppTestFixture, test_automatic_liveliness_changed) {
  const std::chrono::milliseconds max_test_length = 8s;
  const std::chrono::milliseconds kill_publisher_after = 2s;
  const std::chrono::milliseconds publish_period = 200ms;
  int number_of_published_messages = 0;
  const std::chrono::milliseconds lease_duration = 1s;

  int total_number_of_liveliness_events = 0;

  // define qos profile
  rclcpp::QoS qos_profile(10);
  qos_profile.liveliness(RMW_QOS_POLICY_LIVELINESS_AUTOMATIC);
  qos_profile.liveliness_lease_duration(lease_duration);

  std::string topic("test_automatic_liveliness_changed");

  publisher = std::make_shared<QosTestPublisher>(
    "publisher", topic, qos_profile, publish_period);
  subscriber = std::make_shared<QosTestSubscriber>(
    "subscriber", topic, qos_profile);

  // publisher options
  publisher->options().event_callbacks.liveliness_callback =
    [this](rclcpp::QOSLivelinessLostInfo & /*event*/) -> void
    {
      RCLCPP_INFO(publisher->get_logger(), "=====QOSLivelinessLostInfo callback fired");
    };

  // subscriber Liveliness callback event
  subscriber->options().event_callbacks.liveliness_callback =
    [this, &total_number_of_liveliness_events](
    rclcpp::QOSLivelinessChangedInfo & event) -> void
    {
      RCLCPP_INFO(subscriber->get_logger(), "QOSLivelinessChangedInfo callback");
      total_number_of_liveliness_events++;

      // strict checking for expected events
      if (total_number_of_liveliness_events == 1) {
        // publisher came alive
        ASSERT_EQ(1, event.alive_count);
        ASSERT_EQ(0, event.not_alive_count);
        ASSERT_EQ(1, event.alive_count_change);
        ASSERT_EQ(0, event.not_alive_count_change);
      } else if (total_number_of_liveliness_events == 2) {
        // publisher died
        ASSERT_EQ(0, event.alive_count);
        ASSERT_EQ(0, event.not_alive_count);
        ASSERT_EQ(-1, event.alive_count_change);
        ASSERT_EQ(0, event.not_alive_count_change);
      }
    };

  int timer_fired_count = 0;
  // kill the publisher after a predetermined amount of time
  rclcpp::TimerBase::SharedPtr kill_publisher_timer = nullptr;
  kill_publisher_timer = subscriber->create_wall_timer(
    kill_publisher_after,
    [this, &number_of_published_messages, &kill_publisher_timer, &timer_fired_count]() -> void {
      executor->remove_node(publisher);
      number_of_published_messages = publisher->get_count();

      publisher->teardown();
      publisher.reset();  // force liveliness lost for automatic

      kill_publisher_timer->cancel();
      timer_fired_count++;
    });

  executor->add_node(subscriber);
  subscriber->start();

  executor->add_node(publisher);
  publisher->start();

  // the future will never be resolved, so simply time out to force the experiment to stop
  executor->spin_until_future_complete(dummy_future, max_test_length);
  kill_publisher_timer->cancel();

  EXPECT_EQ(1, timer_fired_count);
  EXPECT_EQ(2, total_number_of_liveliness_events);  // check expected number of liveliness events
  EXPECT_GT(number_of_published_messages, 0);  // check if we published anything
  EXPECT_GT(subscriber->get_count(), 0);  // check if we received anything
}
