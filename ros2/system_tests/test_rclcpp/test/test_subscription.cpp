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
#include <string>

#include "gtest/gtest.h"

#include "rclcpp/rclcpp.hpp"

#include "test_rclcpp/utils.hpp"

#include "test_rclcpp/msg/u_int32.hpp"

#include "./pub_sub_fixtures.hpp"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)

#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

using namespace std::chrono_literals;

template<typename DurationT>
void wait_for_future(
  rclcpp::Executor & executor,
  std::shared_future<void> & future,
  const DurationT & timeout)
{
  using rclcpp::FutureReturnCode;
  rclcpp::FutureReturnCode future_ret;
  auto start_time = std::chrono::steady_clock::now();
  future_ret = executor.spin_until_future_complete(future, timeout);
  auto elapsed_time = std::chrono::steady_clock::now() - start_time;
  EXPECT_EQ(FutureReturnCode::SUCCESS, future_ret) <<
    "future failed to be set after: " <<
    std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time).count() <<
    " milliseconds\n";
}

class CLASSNAME (test_subscription, RMW_IMPLEMENTATION) : public ::testing::Test
{
public:
  static void SetUpTestCase()
  {
    rclcpp::init(0, nullptr);
  }

  static void TearDownTestCase()
  {
    rclcpp::shutdown();
  }
};

TEST_F(CLASSNAME(test_subscription, RMW_IMPLEMENTATION), subscription_and_spinning) {
  auto node = rclcpp::Node::make_shared("test_subscription");

  std::string topic = "test_subscription";

  auto publisher = node->create_publisher<test_rclcpp::msg::UInt32>(topic, 10);

  int counter = 0;
  std::promise<void> sub_called;
  std::shared_future<void> sub_called_future(sub_called.get_future());
  auto fail_after_timeout = 5s;
  auto callback =
    [&counter, &sub_called](const test_rclcpp::msg::UInt32::ConstSharedPtr msg) -> void
    {
      ++counter;
      printf("  callback() %d with message data %u\n", counter, msg->data);
      ASSERT_GE(counter, 0);
      ASSERT_EQ(static_cast<unsigned int>(counter), msg->data);
      sub_called.set_value();
    };

  test_rclcpp::msg::UInt32 msg;
  msg.data = 0;
  rclcpp::executors::SingleThreadedExecutor executor;
  executor.add_node(node);

  {
    auto subscriber = node->create_subscription<test_rclcpp::msg::UInt32>(topic, 10, callback);

    // wait for discovery and the subscriber to connect
    test_rclcpp::wait_for_subscriber(node, topic);

    // start condition
    ASSERT_EQ(0, counter);

    // nothing should be pending here
    printf("spin_once(nonblocking) - no callback expected\n");
    executor.spin_once(0s);
    ASSERT_EQ(0, counter);
    printf("spin_some() - no callback expected\n");
    executor.spin_some();
    ASSERT_EQ(0, counter);

    msg.data = 1;
    publisher->publish(msg);
    ASSERT_EQ(0, counter);

    // spin until the subscription is called or a timeout occurs
    printf("spin_until_future_complete(sub_called_future) - callback (1) expected\n");
    wait_for_future(executor, sub_called_future, fail_after_timeout);
    ASSERT_EQ(1, counter);

    // no additional calls to the subscription should be pending here
    printf("spin_once(nonblocking) - no callback expected\n");
    executor.spin_once(0s);
    ASSERT_EQ(1, counter);
    printf("spin_some() - no callback expected\n");
    executor.spin_some();
    ASSERT_EQ(1, counter);

    msg.data = 2;
    publisher->publish(msg);
    msg.data = 3;
    publisher->publish(msg);
    msg.data = 4;
    publisher->publish(msg);
    msg.data = 5;
    publisher->publish(msg);
    ASSERT_EQ(1, counter);

    // while four messages have been published one callback should be triggered here
    printf("spin_until_future_complete(short timeout) - callback (2) expected\n");
    sub_called = std::promise<void>();
    sub_called_future = sub_called.get_future();
    wait_for_future(executor, sub_called_future, 10ms);
    ASSERT_EQ(2, counter);

    // check for next pending call
    printf("spin_until_future_complete(short timeout) - callback (3) expected\n");
    sub_called = std::promise<void>();
    sub_called_future = sub_called.get_future();
    wait_for_future(executor, sub_called_future, 10ms);
    ASSERT_EQ(3, counter);

    // check for next pending call
    printf("spin_until_future_complete(short timeout) - callback (4) expected\n");
    sub_called = std::promise<void>();
    sub_called_future = sub_called.get_future();
    wait_for_future(executor, sub_called_future, 10ms);
    ASSERT_EQ(4, counter);

    // check for last pending call (blocking)
    printf("spin_until_future_complete() - callback (5) expected\n");
    sub_called = std::promise<void>();
    sub_called_future = sub_called.get_future();
    wait_for_future(executor, sub_called_future, fail_after_timeout);
    ASSERT_EQ(5, counter);
  }
  // the subscriber goes out of scope and should be not receive any callbacks anymore

  msg.data = 6;
  publisher->publish(msg);

  // check that no further callbacks have been invoked
  printf("spin_until_future_complete(short timeout) - no callbacks expected\n");
  sub_called = std::promise<void>();
  sub_called_future = sub_called.get_future();
  using rclcpp::FutureReturnCode;
  FutureReturnCode future_ret = executor.spin_until_future_complete(sub_called_future, 100ms);
  EXPECT_EQ(FutureReturnCode::TIMEOUT, future_ret);
  ASSERT_EQ(5, counter);
}

// Shortened version of the test for the ConstSharedPtr callback signature
TEST_F(CLASSNAME(test_subscription, RMW_IMPLEMENTATION), subscription_shared_ptr_const) {
  std::string topic_name = "test_subscription_subscription_shared_ptr_const";
  // create the callback and subscription
  int counter = 0;
  auto callback =
    [&counter](test_rclcpp::msg::UInt32::ConstSharedPtr msg) -> void
    {
      ++counter;
      printf("  callback() %d with message data %u\n", counter, msg->data);
    };
  auto create_subscription_func =
    [&callback](
    rclcpp::Node::SharedPtr node,
    const std::string & topic_name) -> rclcpp::Subscription<test_rclcpp::msg::UInt32>::SharedPtr
    {
      auto subscriber = node->create_subscription<test_rclcpp::msg::UInt32>(
        topic_name, 10, callback);
      return subscriber;
    };
  // do the publish function
  auto publish_func =
    [](
    rclcpp::Publisher<test_rclcpp::msg::UInt32>::SharedPtr publisher,
    test_rclcpp::msg::UInt32 msg)
    {
      publisher->publish(msg);
    };
  // call the test template
  single_message_pub_sub_fixture<test_rclcpp::msg::UInt32>(
    topic_name, counter, create_subscription_func, publish_func);
}

class CallbackHolder
{
public:
  int counter;
  void callback(test_rclcpp::msg::UInt32::ConstSharedPtr msg)
  {
    ++counter;
    printf("  callback() %d with message data %u\n", counter, msg->data);
  }
  CallbackHolder()
  : counter(0)
  {}
};

// Shortened version of the test for the ConstSharedPtr callback signature in a method
TEST_F(
  CLASSNAME(test_subscription, RMW_IMPLEMENTATION),
  subscription_shared_ptr_const_method_std_function
) {
  std::string topic_name = "test_subscription_shared_ptr_const_method_std_function";
  // create the callback and subscription
  CallbackHolder cb_holder;
  std::function<void(test_rclcpp::msg::UInt32::ConstSharedPtr)> cb_std_function = std::bind(
    &CallbackHolder::callback, &cb_holder, std::placeholders::_1);
  auto create_subscription_func =
    [&cb_std_function](
    rclcpp::Node::SharedPtr node,
    const std::string & topic_name) -> rclcpp::Subscription<test_rclcpp::msg::UInt32>::SharedPtr
    {
      auto subscriber = node->create_subscription<test_rclcpp::msg::UInt32>(
        topic_name, 10, cb_std_function);
      return subscriber;
    };
  // do the publish function
  auto publish_func =
    [](
    rclcpp::Publisher<test_rclcpp::msg::UInt32>::SharedPtr publisher,
    test_rclcpp::msg::UInt32 msg)
    {
      publisher->publish(msg);
    };
  // call the test template
  single_message_pub_sub_fixture<test_rclcpp::msg::UInt32>(
    topic_name, cb_holder.counter, create_subscription_func, publish_func);
}

// Shortened version of the test for the ConstSharedPtr callback signature in a method
TEST_F(
  CLASSNAME(test_subscription, RMW_IMPLEMENTATION),
  subscription_shared_ptr_const_method_direct
) {
  std::string topic_name = "test_subscription_shared_ptr_const_method_direct";
  // create the callback and subscription
  CallbackHolder cb_holder;
  auto create_subscription_func =
    [&cb_holder](
    rclcpp::Node::SharedPtr node,
    const std::string & topic_name) -> rclcpp::Subscription<test_rclcpp::msg::UInt32>::SharedPtr
    {
      auto subscriber = node->create_subscription<test_rclcpp::msg::UInt32>(
        topic_name,
        10,
        std::bind(&CallbackHolder::callback, &cb_holder, std::placeholders::_1));
      return subscriber;
    };
  // do the publish function
  auto publish_func =
    [](
    rclcpp::Publisher<test_rclcpp::msg::UInt32>::SharedPtr publisher,
    test_rclcpp::msg::UInt32 msg)
    {
      publisher->publish(msg);
    };
  // call the test template
  single_message_pub_sub_fixture<test_rclcpp::msg::UInt32>(
    topic_name, cb_holder.counter, create_subscription_func, publish_func);
}

// Shortened version of the test for the ConstSharedPtr with info callback signature
TEST_F(CLASSNAME(test_subscription, RMW_IMPLEMENTATION), subscription_shared_ptr_const_with_info) {
  std::string topic_name = "test_subscription_shared_ptr_const_method_direct";
  // create the callback and subscription
  int counter = 0;
  auto callback =
    [&counter](test_rclcpp::msg::UInt32::ConstSharedPtr msg,
      const rclcpp::MessageInfo & info) -> void
    {
      ++counter;
      printf("  callback() %d with message data %u\n", counter, msg->data);
      ASSERT_FALSE(info.get_rmw_message_info().from_intra_process);
    };
  auto create_subscription_func =
    [&callback](
    rclcpp::Node::SharedPtr node,
    const std::string & topic_name) -> rclcpp::Subscription<test_rclcpp::msg::UInt32>::SharedPtr
    {
      auto subscriber = node->create_subscription<test_rclcpp::msg::UInt32>(
        topic_name, 10, callback);
      return subscriber;
    };
  // do the publish function
  auto publish_func =
    [](
    rclcpp::Publisher<test_rclcpp::msg::UInt32>::SharedPtr publisher,
    test_rclcpp::msg::UInt32 msg)
    {
      publisher->publish(msg);
    };
  // call the test template
  single_message_pub_sub_fixture<test_rclcpp::msg::UInt32>(
    topic_name, counter, create_subscription_func, publish_func);
}

// Shortened version of the test for subscribing after spinning has started.
TEST_F(CLASSNAME(test_subscription, RMW_IMPLEMENTATION), spin_before_subscription) {
  std::string topic_name = "test_spin_before_subscription";
  // create the callback and subscription
  int counter = 0;
  auto callback =
    [&counter](test_rclcpp::msg::UInt32::ConstSharedPtr msg) -> void
    {
      ++counter;
      printf("  callback() %d with message data %u\n", counter, msg->data);
    };
  auto create_subscription_func =
    [&callback](
    rclcpp::Node::SharedPtr node,
    const std::string & topic_name) -> rclcpp::Subscription<test_rclcpp::msg::UInt32>::SharedPtr
    {
      auto subscriber = node->create_subscription<test_rclcpp::msg::UInt32>(
        topic_name, 10, callback);
      return subscriber;
    };
  // do the publish function
  auto publish_func =
    [](
    rclcpp::Publisher<test_rclcpp::msg::UInt32>::SharedPtr publisher,
    test_rclcpp::msg::UInt32 msg)
    {
      publisher->publish(msg);
    };
  // code for custom "pre subscription" hook
  auto pre_subscription_hook =
    [](rclcpp::executors::SingleThreadedExecutor & executor)
    {
      // this call is the point of this test, i.e. we spin the executor before
      // creating the subscription
      executor.spin_some();
    };
  // call the test template
  single_message_pub_sub_fixture<test_rclcpp::msg::UInt32>(
    topic_name, counter, create_subscription_func, publish_func,
    10, pre_subscription_hook);
}

// Test of the queue size create_subscription signature.
TEST_F(CLASSNAME(test_subscription, RMW_IMPLEMENTATION), create_subscription_with_queue_size) {
  auto node = rclcpp::Node::make_shared("test_subscription");

  auto callback = [](test_rclcpp::msg::UInt32::ConstSharedPtr) -> void {};

  auto subscriber = node->create_subscription<test_rclcpp::msg::UInt32>(
    "test_subscription", 10, callback);
}
