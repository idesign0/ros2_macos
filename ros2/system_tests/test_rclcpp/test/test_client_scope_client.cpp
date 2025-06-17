// Copyright 2016 Open Source Robotics Foundation, Inc.
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
#include <iostream>
#include <memory>

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

class CLASSNAME (service_client, RMW_IMPLEMENTATION) : public ::testing::Test
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

TEST_F(CLASSNAME(service_client, RMW_IMPLEMENTATION), client_scope_regression_test) {
  auto node = rclcpp::Node::make_shared("client_scope_regression_test");

  // Extra scope so the first client will be deleted afterwards
  {
    printf("creating first client\n");
    std::cout.flush();
    auto client1 = node->create_client<test_rclcpp::srv::AddTwoInts>("client_scope");
    if (!client1->wait_for_service(20s)) {
      ASSERT_TRUE(false) << "service not available after waiting";
    }
    auto request1 = std::make_shared<test_rclcpp::srv::AddTwoInts::Request>();
    request1->a = 1;
    request1->b = 2;

    printf("sending first request\n");
    std::cout.flush();
    auto result1 = client1->async_send_request(request1);
    if (
      rclcpp::spin_until_future_complete(node, result1) !=
      rclcpp::FutureReturnCode::SUCCESS)
    {
      FAIL();
    }

    printf("received first result\n");
    std::cout.flush();
    EXPECT_EQ(result1.get()->sum, 3);

    printf("destroying first client\n");
    std::cout.flush();
  }
  {
    printf("creating second client\n");
    std::cout.flush();
    auto client2 = node->create_client<test_rclcpp::srv::AddTwoInts>("client_scope");
    if (!client2->wait_for_service(20s)) {
      ASSERT_TRUE(false) << "service not available after waiting";
    }
    auto request2 = std::make_shared<test_rclcpp::srv::AddTwoInts::Request>();
    request2->a = 2;
    request2->b = 3;

    printf("sending second request\n");
    std::cout.flush();
    auto result2 = client2->async_send_request(request2);
    if (rclcpp::spin_until_future_complete(node, result2) !=
      rclcpp::FutureReturnCode::SUCCESS)
    {
      FAIL();
    }

    printf("received second result\n");
    std::cout.flush();
    EXPECT_EQ(result2.get()->sum, 5);

    printf("destroying second client\n");
    std::cout.flush();
  }
}
