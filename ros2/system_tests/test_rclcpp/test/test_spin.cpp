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

#include <chrono>
#include <future>
#include <iostream>
#include <memory>
#include <thread>

#include "gtest/gtest.h"

#include "rclcpp/rclcpp.hpp"

#include "test_rclcpp/msg/u_int32.hpp"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

using namespace std::chrono_literals;

class CLASSNAME (test_spin, RMW_IMPLEMENTATION) : public ::testing::Test
{
public:
  void SetUp()
  {
    rclcpp::init(0, nullptr);
  }

  void TearDown()
  {
    if (rclcpp::ok()) {
      rclcpp::shutdown();
    }
  }
};

/*
   Ensures that the timeout behavior of spin_until_future_complete is correct.
 */
TEST_F(CLASSNAME(test_spin, RMW_IMPLEMENTATION), test_spin_until_future_complete_timeout) {
  using rclcpp::FutureReturnCode;
  rclcpp::executors::SingleThreadedExecutor executor;

  // Try passing an already complete future, it should succeed.
  {
    std::promise<void> already_set_promise;
    std::shared_future<void> already_complete_future = already_set_promise.get_future();
    already_set_promise.set_value();
    auto ret = executor.spin_until_future_complete(already_complete_future, 1s);
    EXPECT_EQ(FutureReturnCode::SUCCESS, ret);
    // Also try blocking with no timeout (default timeout of -1).
    ret = executor.spin_until_future_complete(already_complete_future);
    EXPECT_EQ(FutureReturnCode::SUCCESS, ret);
  }

  // Try to trigger the timeout by passing a never completed future.
  {
    std::promise<void> never_set_promise;
    std::shared_future<void> never_complete_future = never_set_promise.get_future();
    // Set the timeout just long enough to make sure it isn't incorrectly set.
    auto ret = executor.spin_until_future_complete(never_complete_future, 50ms);
    EXPECT_EQ(FutureReturnCode::TIMEOUT, ret);
    // Also try with zero timeout.
    ret = executor.spin_until_future_complete(never_complete_future, 0s);
    EXPECT_EQ(FutureReturnCode::TIMEOUT, ret);
  }

  // Try to complete the future asynchronously, but not from within spinning.
  {
    std::shared_future<void> async_future = std::async(
      std::launch::async,
      []() {
        std::this_thread::sleep_for(50ms);
      });
    auto ret = executor.spin_until_future_complete(async_future, 100ms);
    EXPECT_EQ(FutureReturnCode::SUCCESS, ret);
  }

  auto node = rclcpp::Node::make_shared("test_spin");
  executor.add_node(node);
  // Try trigger a timeout while spinning events are being handled.
  {
    std::promise<void> never_set_promise;
    auto timer = node->create_wall_timer(
      7ms,
      []() {
        // Do nothing.
      });
    auto timer2 = node->create_wall_timer(
      50ms,
      []() {
        // Do nothing.
      });
    std::shared_future<void> never_completed_future = never_set_promise.get_future();
    // Try with a timeout long enough for both timers to fire at least once.
    auto ret = executor.spin_until_future_complete(never_completed_future, 75ms);
    EXPECT_EQ(FutureReturnCode::TIMEOUT, ret);
    // Also try with a timeout of zero (nonblocking).
    ret = executor.spin_until_future_complete(never_completed_future, 0s);
    EXPECT_EQ(FutureReturnCode::TIMEOUT, ret);
  }

  // Try to complete a future from within a spinning callback, in the presence of other events.
  {
    std::promise<void> timer_fired_promise;
    auto timer = node->create_wall_timer(
      50ms,
      [&timer_fired_promise]() {
        timer_fired_promise.set_value();
      });
    auto timer2 = node->create_wall_timer(
      1ms,
      []() {
        // Do nothing.
      });
    std::shared_future<void> timer_fired_future = timer_fired_promise.get_future();
    auto ret = executor.spin_until_future_complete(timer_fired_future, 100ms);
    EXPECT_EQ(FutureReturnCode::SUCCESS, ret);
    // Also try again with blocking spin_until_future_complete.
    timer_fired_promise = std::promise<void>();
    timer_fired_future = timer_fired_promise.get_future();
    ret = executor.spin_until_future_complete(timer_fired_future);
    EXPECT_EQ(FutureReturnCode::SUCCESS, ret);
  }
}

TEST_F(CLASSNAME(test_spin, RMW_IMPLEMENTATION), spin_until_future_complete) {
  auto node = rclcpp::Node::make_shared("test_spin");

  // Construct a fake future to wait on
  std::promise<bool> promise;
  std::shared_future<bool> future(promise.get_future());

  // Make a timer to complete the promise in the future
  auto callback = [&promise](rclcpp::TimerBase & timer) {
      promise.set_value(true);
      timer.cancel();
    };
  auto timer = node->create_wall_timer(std::chrono::milliseconds(25), callback);

  rclcpp::executors::SingleThreadedExecutor executor;
  executor.add_node(node);
  ASSERT_EQ(
    executor.spin_until_future_complete(future),
    rclcpp::FutureReturnCode::SUCCESS);
  EXPECT_EQ(future.get(), true);
}

TEST_F(CLASSNAME(test_spin, RMW_IMPLEMENTATION), spin_until_future_complete_timeout) {
  auto node = rclcpp::Node::make_shared("test_spin");

  // Construct a fake future to wait on
  std::promise<bool> promise;
  std::shared_future<bool> future(promise.get_future());

  // Make a timer to complete the promise in the future
  auto callback = [&promise]() {
      promise.set_value(true);
    };
  auto timer = node->create_wall_timer(std::chrono::milliseconds(50), callback);

  ASSERT_EQ(
    rclcpp::spin_until_future_complete(node, future, std::chrono::milliseconds(25)),
    rclcpp::FutureReturnCode::TIMEOUT);

  // If we wait a little longer, we should complete the future
  ASSERT_EQ(
    rclcpp::spin_until_future_complete(node, future, std::chrono::milliseconds(50)),
    rclcpp::FutureReturnCode::SUCCESS);

  EXPECT_EQ(future.get(), true);
}

TEST_F(CLASSNAME(test_spin, RMW_IMPLEMENTATION), spin_until_future_complete_interrupted) {
  auto node = rclcpp::Node::make_shared("test_spin");

  // Construct a fake future to wait on
  std::promise<bool> promise;
  std::shared_future<bool> future(promise.get_future());

  // Make a timer to complete the promise in the future
  auto callback = [&promise]() {
      promise.set_value(true);
    };
  auto timer = node->create_wall_timer(std::chrono::milliseconds(50), callback);

  // Create a timer that will shut down rclcpp before
  auto shutdown_callback = []() {
      rclcpp::shutdown();
    };
  auto shutdown_timer = node->create_wall_timer(std::chrono::milliseconds(25), shutdown_callback);

  ASSERT_EQ(
    rclcpp::spin_until_future_complete(node, future, std::chrono::milliseconds(50)),
    rclcpp::FutureReturnCode::INTERRUPTED);
}

TEST_F(CLASSNAME(test_spin, RMW_IMPLEMENTATION), cancel) {
  auto node = rclcpp::Node::make_shared("cancel");
  rclcpp::executors::SingleThreadedExecutor executor;
  auto pub = node->create_publisher<test_rclcpp::msg::UInt32>("cancel", 10);

  auto subscription_callback = [](test_rclcpp::msg::UInt32::ConstSharedPtr)
    {
      fprintf(stderr, "Failure: subscription callback received before cancel\n");
      FAIL();
    };
  auto subscription = node->create_subscription<test_rclcpp::msg::UInt32>(
    "cancel", 10, subscription_callback);

  auto cancel_callback = [&executor, &pub]()
    {
      executor.cancel();
      // Try to publish after canceling. The callback should never trigger.
      pub->publish(test_rclcpp::msg::UInt32());
    };
  auto timer = node->create_wall_timer(std::chrono::milliseconds(5), cancel_callback);
  executor.add_node(node);
  executor.spin();
}
