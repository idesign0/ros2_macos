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

#include <atomic>
#include <chrono>
#include <cinttypes>
#include <future>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "gtest/gtest.h"

#include "rclcpp/exceptions.hpp"
#include "rclcpp/rclcpp.hpp"

#include "test_rclcpp/utils.hpp"

#include "test_rclcpp/msg/u_int32.hpp"
#include "test_rclcpp/srv/add_two_ints.hpp"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

using namespace std::chrono_literals;

class CLASSNAME (test_executor, RMW_IMPLEMENTATION) : public ::testing::Test
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

TEST_F(CLASSNAME(test_executor, RMW_IMPLEMENTATION), recursive_spin_call) {
  rclcpp::executors::SingleThreadedExecutor executor;
  auto node = rclcpp::Node::make_shared("recursive_spin_call");
  auto timer = node->create_wall_timer(
    0s,
    [&executor]() {
      ASSERT_THROW(executor.spin_some(), std::runtime_error);
      ASSERT_THROW(executor.spin_once(), std::runtime_error);
      ASSERT_THROW(executor.spin(), std::runtime_error);
      executor.cancel();
    });
  executor.add_node(node);
  executor.spin();
}

TEST_F(CLASSNAME(test_executor, RMW_IMPLEMENTATION), spin_some_max_duration) {
  rclcpp::executors::SingleThreadedExecutor executor;
  auto node = rclcpp::Node::make_shared("spin_some_max_duration");
  auto lambda = []() {
      std::this_thread::sleep_for(100ms);
    };
  std::vector<std::shared_ptr<rclcpp::WallTimer<decltype(lambda)>>> timers;
  // creating 20 timers which will try to do 100 ms of work each
  // only about 1s worth of them should actually be performed
  for (int i = 0; i < 20; i++) {
    auto timer = node->create_wall_timer(0s, lambda);
    timers.push_back(timer);
  }
  executor.add_node(node);

  const auto max_duration = 1s;
  const auto start = std::chrono::steady_clock::now();
  executor.spin_some(max_duration);
  const auto end = std::chrono::steady_clock::now();
  ASSERT_LT(max_duration, end - start);
  ASSERT_GT(max_duration + 500ms, end - start);
}

TEST_F(CLASSNAME(test_executor, RMW_IMPLEMENTATION), multithreaded_spin_call) {
  rclcpp::executors::SingleThreadedExecutor executor;
  auto node = rclcpp::Node::make_shared("multithreaded_spin_call");
  std::mutex m;
  bool ready = false;
  std::condition_variable cv;
  std::thread t(
    [&executor, &m, &cv, &ready]() {
      std::unique_lock<std::mutex> lock(m);
      cv.wait(lock, [&ready] {return ready;});
      ASSERT_THROW(executor.spin_some(), std::runtime_error);
      ASSERT_THROW(executor.spin_once(), std::runtime_error);
      ASSERT_THROW(executor.spin(), std::runtime_error);
      executor.cancel();
    });
  auto timer = node->create_wall_timer(
    0s,
    [&m, &cv, &ready]() {
      if (!ready) {
        {
          std::lock_guard<std::mutex> lock(m);
          ready = true;
        }
        cv.notify_one();
      }
    });
  executor.add_node(node);
  executor.spin();
  t.join();
}

// Try spinning 2 single-threaded executors in two separate threads.
TEST_F(CLASSNAME(test_executor, RMW_IMPLEMENTATION), multiple_executors) {
  std::atomic_uint counter1;
  counter1 = 0;
  std::atomic_uint counter2;
  counter2 = 0;
  const uint32_t counter_goal = 20;

  // Initialize executor 1.
  rclcpp::executors::SingleThreadedExecutor executor1;
  // I'm not a huge fan of unspecified capture variables in a lambda, but it
  // is necessary in this case.  On MacOS High Sierra and later, clang
  // complains if we try to pass "counter_goal" as a specific lambda capture
  // since it is a const.  On the other hand, MSVC 2017 (19.12.25834.0)
  // complains if you do *not* have the capture.  To let both compilers be
  // happy, we just let the compiler figure out the captures it wants; this
  // is doubly OK because this is just for a test.
  auto callback1 = [&]() {
      if (counter1 == counter_goal) {
        executor1.cancel();
        return;
      }
      ++counter1;
    };
  auto node1 = rclcpp::Node::make_shared("multiple_executors_1");
  auto timer1 = node1->create_wall_timer(1ms, callback1);
  executor1.add_node(node1);

  // Initialize executor 2.
  rclcpp::executors::SingleThreadedExecutor executor2;

  // This lambda has the same problem & solution as the callback1 lambda above.
  auto callback2 = [&]() {
      if (counter2 == counter_goal) {
        executor2.cancel();
        return;
      }
      ++counter2;
    };
  auto node2 = rclcpp::Node::make_shared("multiple_executors_2");
  auto timer2 = node2->create_wall_timer(1ms, callback2);
  executor2.add_node(node2);

  auto spin_executor2 = [&executor2]() {
      executor2.spin();
    };

  // Launch both executors
  std::thread execution_thread(spin_executor2);

  executor1.spin();
  execution_thread.join();
  EXPECT_EQ(counter1.load(), counter_goal);
  EXPECT_EQ(counter2.load(), counter_goal);

  // Try to add node1 to executor2. It should throw, since node1 was already added to executor1.
  ASSERT_THROW(executor2.add_node(node1), std::runtime_error);
}

// Check that the executor is notified when a node adds a new timer, publisher, subscription,
// service or client.
TEST_F(CLASSNAME(test_executor, RMW_IMPLEMENTATION), notify) {
  rclcpp::executors::SingleThreadedExecutor executor;
  auto executor_spin_lambda = [&executor]() {
      executor.spin();
    };
  auto node = rclcpp::Node::make_shared("test_executor_notify");
  executor.add_node(node);
  {
    std::thread spin_thread(executor_spin_lambda);
    std::promise<void> timer_promise;
    std::shared_future<void> timer_future(timer_promise.get_future());

    auto timer = node->create_wall_timer(
      1ms,
      [&timer_promise](rclcpp::TimerBase & timer)
      {
        timer_promise.set_value();
        timer.cancel();
      });
    EXPECT_EQ(std::future_status::ready, timer_future.wait_for(50ms));
    executor.cancel();

    spin_thread.join();
  }

  {
    std::thread spin_thread(executor_spin_lambda);
    bool subscription_triggered = false;
    auto sub_callback =
      [&executor, &subscription_triggered](test_rclcpp::msg::UInt32::ConstSharedPtr msg) -> void
      {
        subscription_triggered = true;
        EXPECT_EQ(msg->data, 42u);
        executor.cancel();
      };

    auto subscription = node->create_subscription<test_rclcpp::msg::UInt32>(
      "test_executor_notify_subscription",
      10,
      sub_callback);
    test_rclcpp::wait_for_subscriber(node, "test_executor_notify_subscription");


    auto publisher = node->create_publisher<test_rclcpp::msg::UInt32>(
      "test_executor_notify_subscription", 10);
    auto timer = node->create_wall_timer(
      1ms,
      [&publisher]()
      {
        test_rclcpp::msg::UInt32 pub_msg;
        pub_msg.data = 42;
        publisher->publish(pub_msg);
      });

    spin_thread.join();

    EXPECT_TRUE(subscription_triggered);
  }

  {
    std::thread spin_thread(executor_spin_lambda);

    auto service = node->create_service<test_rclcpp::srv::AddTwoInts>(
      "test_executor_notify_service",
      [](
        test_rclcpp::srv::AddTwoInts::Request::SharedPtr request,
        test_rclcpp::srv::AddTwoInts::Response::SharedPtr response)
      {
        response->sum = request->a + request->b;
      });

    auto client = node->create_client<test_rclcpp::srv::AddTwoInts>(
      "test_executor_notify_service"
    );
    if (!client->wait_for_service(20s)) {
      ASSERT_TRUE(false) << "service not available after waiting";
    }
    auto request = std::make_shared<test_rclcpp::srv::AddTwoInts::Request>();
    request->a = 4;
    request->b = 2;
    auto future_result = client->async_send_request(request);
    EXPECT_EQ(future_result.get()->sum, 6);
    executor.cancel();
    spin_thread.join();
  }
}

// test removing a node

// test notify with multiple nodes
