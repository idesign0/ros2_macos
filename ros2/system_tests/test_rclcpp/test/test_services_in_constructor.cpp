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

#include <memory>

#include "gtest/gtest.h"

#include "rclcpp/rclcpp.hpp"

#include "test_rclcpp/srv/add_two_ints.hpp"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

/* The purpose of these tests is to ensure that Services and Clients can be
 * created within the constructor of a class which inherits from Node.
 * This is important as it is a common thing users will try to do and was not
 * possible at some point, but should be now.
 */

class MyNodeWithService : public rclcpp::Node
{
public:
  MyNodeWithService()
  : rclcpp::Node("node_with_service")
  {
    service_ = this->create_service<test_rclcpp::srv::AddTwoInts>(
      "test",
      [](
        const std::shared_ptr<test_rclcpp::srv::AddTwoInts::Request> request,
        std::shared_ptr<test_rclcpp::srv::AddTwoInts::Response> response)
      {
        response->sum = request->a + request->b;
      });
  }

private:
  rclcpp::ServiceBase::SharedPtr service_;
};

class CLASSNAME (test_services_in_constructor, RMW_IMPLEMENTATION) : public ::testing::Test
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

TEST_F(CLASSNAME(test_services_in_constructor, RMW_IMPLEMENTATION), service_in_constructor) {
  auto n = std::make_shared<MyNodeWithService>();
}

class MyNodeWithClient : public rclcpp::Node
{
public:
  MyNodeWithClient()
  : rclcpp::Node("node_with_client")
  {
    client_ = this->create_client<test_rclcpp::srv::AddTwoInts>("test");
  }

private:
  rclcpp::ClientBase::SharedPtr client_;
};

TEST_F(CLASSNAME(test_services_in_constructor, RMW_IMPLEMENTATION), client_in_constructor) {
  auto n = std::make_shared<MyNodeWithClient>();
}
