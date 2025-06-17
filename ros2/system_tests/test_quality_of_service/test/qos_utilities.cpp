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

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>

#include "test_quality_of_service/qos_test_publisher.hpp"
#include "test_quality_of_service/qos_test_subscriber.hpp"
#include "test_quality_of_service/qos_utilities.hpp"

std::tuple<size_t, size_t> convert_chrono_milliseconds_to_size_t(
  const std::chrono::milliseconds & milliseconds)
{
  size_t seconds = milliseconds.count() / 1000;
  size_t nanoseconds = (milliseconds.count() % 1000) * 1000000;
  return std::make_tuple(seconds, nanoseconds);
}

void BaseQosRclcppTestFixture::SetUp()
{
  rclcpp::init(0, nullptr);
  executor = std::make_shared<rclcpp::executors::SingleThreadedExecutor>();
  empty_promise = std::promise<bool>();
  dummy_future = empty_promise.get_future();
  publisher = nullptr;
  subscriber = nullptr;
}

void BaseQosRclcppTestFixture::TearDown()
{
  if (publisher) {
    publisher->teardown();
    publisher.reset();
  }
  if (subscriber) {
    subscriber->teardown();
    subscriber.reset();
  }
  if (executor) {
    executor->cancel();
    executor.reset();
  }
  rclcpp::shutdown();
}
