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

#include <inttypes.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "gtest/gtest.h"

#include "rclcpp/exceptions.hpp"
#include "rclcpp/rclcpp.hpp"

#include "test_rclcpp/srv/add_two_ints.hpp"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

using namespace std::chrono_literals;

void handle_add_two_ints(
  const std::shared_ptr<test_rclcpp::srv::AddTwoInts::Request> request,
  std::shared_ptr<test_rclcpp::srv::AddTwoInts::Response> response)
{
  response->sum = request->a + request->b;
}

class CLASSNAME (test_two_service_calls, RMW_IMPLEMENTATION) : public ::testing::Test
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

TEST_F(CLASSNAME(test_two_service_calls, RMW_IMPLEMENTATION), two_service_calls) {
  auto node = rclcpp::Node::make_shared("test_two_service_calls");

  auto service = node->create_service<test_rclcpp::srv::AddTwoInts>(
    "test_two_service_calls", handle_add_two_ints);

  auto client = node->create_client<test_rclcpp::srv::AddTwoInts>("test_two_service_calls");
  if (!client->wait_for_service(20s)) {
    ASSERT_TRUE(false) << "service not available after waiting";
  }

  auto request1 = std::make_shared<test_rclcpp::srv::AddTwoInts::Request>();
  request1->a = 1;
  request1->b = 0;

  auto request2 = std::make_shared<test_rclcpp::srv::AddTwoInts::Request>();
  request2->a = 2;
  request2->b = 0;

  printf("Sending two requests...\n");
  fflush(stdout);

  auto result1 = client->async_send_request(request1);
  auto result2 = client->async_send_request(request2);

  printf("Waiting for first reply...\n");
  fflush(stdout);
  rclcpp::spin_until_future_complete(node, result1);
  printf("Received first reply\n");
  EXPECT_EQ(1, result1.get()->sum);

  printf("Waiting for second reply...\n");
  fflush(stdout);
  rclcpp::spin_until_future_complete(node, result2);
  printf("Received second reply\n");
  EXPECT_EQ(2, result2.get()->sum);
}

// Regression test for async client not being able to queue another request in a response callback.
// See https://github.com/ros2/rclcpp/pull/415
TEST_F(CLASSNAME(test_two_service_calls, RMW_IMPLEMENTATION), recursive_service_call) {
  auto node = rclcpp::Node::make_shared("test_recursive_service_call");

  auto service = node->create_service<test_rclcpp::srv::AddTwoInts>(
    "test_recursive_service_call", handle_add_two_ints);

  auto client = node->create_client<test_rclcpp::srv::AddTwoInts>("test_recursive_service_call");
  if (!client->wait_for_service(20s)) {
    ASSERT_TRUE(false) << "service not available after waiting";
  }

  auto request1 = std::make_shared<test_rclcpp::srv::AddTwoInts::Request>();
  request1->a = 1;
  request1->b = 0;

  auto request2 = std::make_shared<test_rclcpp::srv::AddTwoInts::Request>();
  request2->a = 2;
  request2->b = 0;

  bool second_result_received = false;
  using AddTwoIntsSharedFuture = rclcpp::Client<test_rclcpp::srv::AddTwoInts>::SharedFuture;

  auto first_response_received_callback =
    [&client, &request2, &second_result_received](AddTwoIntsSharedFuture first_future) {
      EXPECT_EQ(1, first_future.get()->sum);
      // Trigger another service request from within this callback
      auto second_response_received_callback =
        [&second_result_received](AddTwoIntsSharedFuture second_future) {
          EXPECT_EQ(2, second_future.get()->sum);
          second_result_received = true;
        };
      printf("Sending second request...\n");
      fflush(stdout);
      client->async_send_request(request2, second_response_received_callback);
    };
  printf("Sending first request...\n");
  fflush(stdout);
  client->async_send_request(request1, first_response_received_callback);

  printf("Waiting for reply...\n");
  fflush(stdout);
  while (!second_result_received) {
    rclcpp::spin_some(node);
  }
  EXPECT_TRUE(second_result_received);
}

class CLASSNAME (test_multiple_service_calls, RMW_IMPLEMENTATION) : public ::testing::Test
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

TEST_F(CLASSNAME(test_multiple_service_calls, RMW_IMPLEMENTATION), multiple_clients) {
  const uint32_t n = 5;

  auto node = rclcpp::Node::make_shared("test_multiple_clients");
  rclcpp::executors::SingleThreadedExecutor executor;

  auto service = node->create_service<test_rclcpp::srv::AddTwoInts>(
    "test_multiple_clients", handle_add_two_ints);

  using ClientRequestPair = std::pair<
    rclcpp::Client<test_rclcpp::srv::AddTwoInts>::SharedPtr,
    test_rclcpp::srv::AddTwoInts::Request::SharedPtr>;
  using SharedFuture = rclcpp::Client<test_rclcpp::srv::AddTwoInts>::SharedFuture;

  std::vector<ClientRequestPair> client_request_pairs;
  for (uint32_t i = 0; i < n; ++i) {
    auto client = node->create_client<test_rclcpp::srv::AddTwoInts>("test_multiple_clients");
    auto request = std::make_shared<test_rclcpp::srv::AddTwoInts::Request>();
    request->a = i;
    request->b = i + 1;
    client_request_pairs.push_back(ClientRequestPair(client, request));
  }

  std::vector<SharedFuture> results;

  printf("Sending %u requests...\n", n);
  fflush(stdout);
  // Send all the requests
  for (auto & pair : client_request_pairs) {
    if (!pair.first->wait_for_service(20s)) {
      ASSERT_TRUE(false) << "service not available after waiting";
    }
    results.push_back(pair.first->async_send_request(pair.second).future);
  }

  auto timer_callback = [&executor, &results]() {
      bool all_ready = true;
      for (auto & result : results) {
        all_ready &= result.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
      }
      if (all_ready) {
        executor.cancel();
      }
    };
  auto timer = node->create_wall_timer(std::chrono::milliseconds(3), timer_callback);

  executor.add_node(node);

  executor.spin();

  // Check the status of all futures
  for (uint32_t i = 0; i < results.size(); ++i) {
    ASSERT_EQ(std::future_status::ready, results[i].wait_for(std::chrono::seconds(0)));
    auto response = results.at(i).get();
    EXPECT_EQ(response->sum, 2 * i + 1);
    printf("Got response #%u with value %" PRId64 "\n", i, response->sum);
    fflush(stdout);
  }
}
