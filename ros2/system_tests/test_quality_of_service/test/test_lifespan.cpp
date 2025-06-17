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

TEST_F(QosRclcppTestFixture, test_lifespan) {
  const int history = 2;
  // Bump lifespan duration when testing against rmw_connextdds to
  // cope with the longer discovery times it entails.
  const std::chrono::milliseconds lifespan_duration =
    this_rmw_implementation.find("connext") != std::string::npos ? 2s : 1s;
  const std::chrono::milliseconds subscriber_toggling_period = lifespan_duration * 2;
  const int active_subscriber_num_periods = 3;
  const int inactive_subscriber_num_periods = active_subscriber_num_periods - 1;
  const int subscriber_toggling_num_periods =
    active_subscriber_num_periods + inactive_subscriber_num_periods;
  const std::chrono::milliseconds max_test_length =
    subscriber_toggling_period * subscriber_toggling_num_periods;
  // Publish fast enough for lifespan QoS effects to be observable despite
  // the "noise" that discovery introduces.
  const std::chrono::milliseconds publish_period = lifespan_duration / 8;

  // define qos profile
  rclcpp::QoS qos_profile(history);
  qos_profile.durability(RMW_QOS_POLICY_DURABILITY_TRANSIENT_LOCAL);
  qos_profile.lifespan(lifespan_duration);

  std::string topic = "test_lifespan";

  publisher = std::make_shared<QosTestPublisher>(
    "publisher", topic, qos_profile, publish_period);
  subscriber = std::make_shared<QosTestSubscriber>(
    "subscriber", topic, qos_profile);

  int timer_fired_count = 0;
  // toggle subscription on and off to exercise lifespan
  rclcpp::TimerBase::SharedPtr toggle_subscriber_timer = subscriber->create_wall_timer(
    lifespan_duration * 2,
    [this, &timer_fired_count]() -> void {
      // start / stop publishing subscribing at a rate slower than lifespan
      subscriber->toggle();
      timer_fired_count++;
    });

  executor->add_node(publisher);
  publisher->start();

  executor->add_node(subscriber);
  subscriber->start();

  // the future will never be resolved, so simply time out to force the experiment to stop
  executor->spin_until_future_complete(dummy_future, max_test_length);
  toggle_subscriber_timer->cancel();

  EXPECT_GT(timer_fired_count, 0);
  EXPECT_GT(publisher->get_count(), 0);  // check if we published anything
  EXPECT_GT(subscriber->get_count(), 0);  // check if we received anything

  // check if lifespan simply worked
  const int sub_count_upper_bound = publisher->get_count();
  // skip the first activity period as discovery often skews it
  const int sub_count_lower_bound =
    (publisher->get_count() * (active_subscriber_num_periods - 1)) /
    subscriber_toggling_num_periods;
  EXPECT_LT(subscriber->get_count(), sub_count_upper_bound);
  EXPECT_GT(subscriber->get_count(), sub_count_lower_bound);
}
