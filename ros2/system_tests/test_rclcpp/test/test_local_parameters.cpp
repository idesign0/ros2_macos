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
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <cstdint>
#include <vector>
#include "gtest/gtest.h"

#include "rclcpp/rclcpp.hpp"

#include "parameter_fixtures.hpp"

using namespace std::chrono_literals;

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

class CLASSNAME (test_local_parameters, RMW_IMPLEMENTATION) : public ::testing::Test
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

TEST_F(CLASSNAME(test_local_parameters, RMW_IMPLEMENTATION), to_string) {
  rclcpp::Parameter pv("foo", "bar");
  rclcpp::Parameter pv2("foo2", "bar2");
  std::string json_dict = std::to_string(pv);
  EXPECT_STREQ(
    "{\"name\": \"foo\", \"type\": \"string\", \"value\": \"bar\"}",
    json_dict.c_str());
  json_dict = rclcpp::_to_json_dict_entry(pv);
  EXPECT_STREQ(
    "\"foo\": {\"type\": \"string\", \"value\": \"bar\"}",
    json_dict.c_str());
  std::vector<rclcpp::Parameter> vpv;
  vpv.push_back(pv);
  vpv.push_back(pv2);
  json_dict = std::to_string(vpv);
  EXPECT_STREQ(
    "{\"foo\": {\"type\": \"string\", \"value\": \"bar\"}, "
    "\"foo2\": {\"type\": \"string\", \"value\": \"bar2\"}}",
    json_dict.c_str());

  pv = rclcpp::Parameter("foo", 2.1);
  // TODO(tfoote) convert the value to a float and use epsilon test.
  EXPECT_STREQ(
    "{\"name\": \"foo\", \"type\": \"double\", \"value\": \"2.100000\"}",
    std::to_string(pv).c_str());
  pv = rclcpp::Parameter("foo", 8);
  EXPECT_STREQ(
    "{\"name\": \"foo\", \"type\": \"integer\", \"value\": \"8\"}",
    std::to_string(pv).c_str());
}

TEST_F(CLASSNAME(test_local_parameters, RMW_IMPLEMENTATION), local_synchronous) {
  auto node = rclcpp::Node::make_shared("test_parameters_local_synchronous");
  declare_test_parameters(node);
  auto parameters_client = std::make_shared<rclcpp::SyncParametersClient>(node);
  if (!parameters_client->wait_for_service(20s)) {
    ASSERT_TRUE(false) << "service not available after waiting";
  }
  test_set_parameters_sync(parameters_client);
  test_get_parameters_sync(parameters_client);
}

TEST_F(CLASSNAME(test_local_parameters, RMW_IMPLEMENTATION), local_synchronous_repeated) {
  auto node = rclcpp::Node::make_shared("test_parameters_local_synchronous_repeated");
  declare_test_parameters(node);
  auto parameters_client = std::make_shared<rclcpp::SyncParametersClient>(node);
  if (!parameters_client->wait_for_service(20s)) {
    ASSERT_TRUE(false) << "service not available after waiting";
  }
  test_set_parameters_sync(parameters_client);
  for (int i = 0; i < 10; ++i) {
    printf("iteration: %d\n", i);
    test_get_parameters_sync(parameters_client);
  }
}

TEST_F(CLASSNAME(test_local_parameters, RMW_IMPLEMENTATION), local_asynchronous) {
  auto node = rclcpp::Node::make_shared(std::string("test_parameters_local_asynchronous"));
  declare_test_parameters(node);
  auto parameters_client = std::make_shared<rclcpp::AsyncParametersClient>(node);
  if (!parameters_client->wait_for_service(20s)) {
    ASSERT_TRUE(false) << "service not available after waiting";
  }
  test_set_parameters_async(node, parameters_client);
  test_get_parameters_async(node, parameters_client);
}

class ParametersAsyncNode : public rclcpp::Node
{
public:
  ParametersAsyncNode()
  : Node("test_local_parameters_async_with_callback")
  {
    this->declare_parameter("foo", 0);
    this->declare_parameter("bar", "");
    this->declare_parameter("barstr", "");
    this->declare_parameter("baz", 0.);
    this->declare_parameter("foobar", false);
    this->declare_parameter("barfoo", std::vector<uint8_t>{});

    parameters_client_ =
      std::make_shared<rclcpp::AsyncParametersClient>(this);
  }

  void queue_set_parameter_request(rclcpp::executors::SingleThreadedExecutor & executor)
  {
    using rclcpp::Parameter;
    using SetParametersResult =
      std::shared_future<std::vector<rcl_interfaces::msg::SetParametersResult>>;
    auto parameters = {
      Parameter("foo", 2),
      Parameter("bar", "hello"),
      Parameter("barstr", std::string("hello_str")),
      Parameter("baz", 1.45),
      Parameter("foobar", true),
      Parameter("barfoo", std::vector<uint8_t>{3, 4, 5}),
    };
    auto set_parameters_results = parameters_client_->set_parameters(
      parameters,
      [&executor](SetParametersResult future)
      {
        printf("Got set_parameters result\n");
        // Check to see if they were set.
        for (auto & result : future.get()) {
          ASSERT_TRUE(result.successful);
        }
        executor.cancel();
      }
    );
  }

  rclcpp::AsyncParametersClient::SharedPtr parameters_client_;
};

// Regression test for calling parameter client async services, but having the specified callback
// go out of scope before it gets called: see https://github.com/ros2/rclcpp/pull/414
TEST_F(CLASSNAME(test_local_parameters, RMW_IMPLEMENTATION), local_async_with_callback) {
  auto node = std::make_shared<ParametersAsyncNode>();
  if (!node->parameters_client_->wait_for_service(20s)) {
    ASSERT_TRUE(false) << "service not available after waiting";
  }
  rclcpp::executors::SingleThreadedExecutor executor;
  executor.add_node(node);
  node->queue_set_parameter_request(executor);
  // When the parameters client receives its response it will cancel the executor.
  executor.spin();
}

TEST_F(CLASSNAME(test_local_parameters, RMW_IMPLEMENTATION), helpers) {
  auto node = rclcpp::Node::make_shared("test_parameters_local_helpers");
  node->declare_parameter("foo", 0);
  node->declare_parameter("bar", "");
  node->declare_parameter("barstr", "");
  node->declare_parameter("baz", 0.);
  node->declare_parameter("foobar", false);
  node->declare_parameter("barfoo", std::vector<uint8_t>{});

  auto parameters_client = std::make_shared<rclcpp::SyncParametersClient>(node);
  if (!parameters_client->wait_for_service(20s)) {
    ASSERT_TRUE(false) << "service not available after waiting";
  }
  auto set_parameters_results = parameters_client->set_parameters(
  {
    rclcpp::Parameter("foo", 2),
    rclcpp::Parameter("bar", "hello"),
    rclcpp::Parameter("barstr", std::string("hello_str")),
    rclcpp::Parameter("baz", 1.45),
    rclcpp::Parameter("foobar", true),
    rclcpp::Parameter("barfoo", std::vector<uint8_t>{0, 1, 2}),
  }, 1s);
  printf("Got set_parameters result\n");

  // Check to see if they were set.
  for (auto & result : set_parameters_results) {
    ASSERT_TRUE(result.successful);
  }

  int foo = 0;
  std::string bar, barstr;
  double baz = 0.;
  bool foobar = false;
  std::vector<uint8_t> barfoo;

  // Test variables that are set, verifying that types are obeyed, and defaults not used.
  EXPECT_TRUE(parameters_client->has_parameter("foo"));
  EXPECT_THROW(
    baz = parameters_client->get_parameter<double>("foo"),
    rclcpp::exceptions::InvalidParameterTypeException);
  EXPECT_NO_THROW(foo = parameters_client->get_parameter<int>("foo"));
  EXPECT_EQ(foo, 2);
  EXPECT_NO_THROW(foo = parameters_client->get_parameter("foo", 42));
  EXPECT_EQ(foo, 2);

  EXPECT_TRUE(parameters_client->has_parameter("bar"));
  EXPECT_THROW(
    foo = parameters_client->get_parameter<int>("bar"),
    rclcpp::exceptions::InvalidParameterTypeException);
  EXPECT_NO_THROW(bar = parameters_client->get_parameter<std::string>("bar"));
  EXPECT_EQ(bar, "hello");
  EXPECT_NO_THROW(bar = parameters_client->get_parameter<std::string>("bar", "goodbye"));
  EXPECT_EQ(bar, "hello");

  EXPECT_TRUE(parameters_client->has_parameter("barstr"));
  EXPECT_THROW(
    foobar = parameters_client->get_parameter<bool>("barstr"),
    rclcpp::exceptions::InvalidParameterTypeException);
  EXPECT_NO_THROW(barstr = parameters_client->get_parameter<std::string>("barstr"));
  EXPECT_EQ(barstr, "hello_str");
  EXPECT_NO_THROW(barstr = parameters_client->get_parameter("barstr", std::string("heya")));
  EXPECT_EQ(barstr, "hello_str");

  EXPECT_TRUE(parameters_client->has_parameter("baz"));
  EXPECT_THROW(
    foobar = parameters_client->get_parameter<bool>("baz"),
    rclcpp::exceptions::InvalidParameterTypeException);
  EXPECT_NO_THROW(baz = parameters_client->get_parameter<double>("baz"));
  EXPECT_DOUBLE_EQ(baz, 1.45);
  EXPECT_NO_THROW(baz = parameters_client->get_parameter("baz", -4.2));
  EXPECT_DOUBLE_EQ(baz, 1.45);

  EXPECT_TRUE(parameters_client->has_parameter("foobar"));
  EXPECT_THROW(
    baz = parameters_client->get_parameter<double>("foobar"),
    rclcpp::exceptions::InvalidParameterTypeException);
  EXPECT_NO_THROW(foobar = parameters_client->get_parameter<bool>("foobar"));
  EXPECT_EQ(foobar, true);
  EXPECT_NO_THROW(foobar = parameters_client->get_parameter("foobar", false));
  EXPECT_EQ(foobar, true);

  EXPECT_TRUE(parameters_client->has_parameter("barfoo"));
  EXPECT_THROW(
    bar = parameters_client->get_parameter<std::string>("barfoo"),
    rclcpp::exceptions::InvalidParameterTypeException);
  EXPECT_NO_THROW(barfoo = parameters_client->get_parameter<std::vector<uint8_t>>("barfoo"));
  EXPECT_EQ(barfoo[0], 0);
  EXPECT_EQ(barfoo[1], 1);
  EXPECT_EQ(barfoo[2], 2);
  EXPECT_NO_THROW(
    barfoo = parameters_client->get_parameter("barfoo", std::vector<uint8_t>{3, 4, 5}));
  EXPECT_EQ(barfoo[0], 0);
  EXPECT_EQ(barfoo[1], 1);
  EXPECT_EQ(barfoo[2], 2);

  // Test a variable that's not set, checking that we throw when asking for its value without
  // specifying a default.
  EXPECT_FALSE(parameters_client->has_parameter("not_there"));
  EXPECT_THROW(parameters_client->get_parameter<int>("not_there"), std::runtime_error);
  EXPECT_THROW(parameters_client->get_parameter<std::string>("not_there"), std::runtime_error);
  EXPECT_THROW(parameters_client->get_parameter<double>("not_there"), std::runtime_error);
  EXPECT_THROW(parameters_client->get_parameter<bool>("not_there"), std::runtime_error);
  EXPECT_THROW(
    parameters_client->get_parameter<std::vector<uint8_t>>("not_there"), std::runtime_error);

  // Test a variable that's not set, checking that we correctly get the specified default.
  EXPECT_NO_THROW(foo = parameters_client->get_parameter("not_there", 42));
  EXPECT_EQ(foo, 42);
  EXPECT_NO_THROW(bar = parameters_client->get_parameter<std::string>("not_there", "goodbye"));
  EXPECT_EQ(bar, "goodbye");
  EXPECT_NO_THROW(barstr = parameters_client->get_parameter("not_there", std::string("heya")));
  EXPECT_EQ(barstr, "heya");
  EXPECT_NO_THROW(baz = parameters_client->get_parameter("not_there", -4.2));
  EXPECT_DOUBLE_EQ(baz, -4.2);
  EXPECT_NO_THROW(foobar = parameters_client->get_parameter("not_there", false));
  EXPECT_EQ(foobar, false);
  EXPECT_NO_THROW(
    barfoo = parameters_client->get_parameter("not_there", std::vector<uint8_t>{3, 4, 5}));
  EXPECT_EQ(barfoo[0], 3);
  EXPECT_EQ(barfoo[1], 4);
  EXPECT_EQ(barfoo[2], 5);
}

TEST_F(CLASSNAME(test_local_parameters, RMW_IMPLEMENTATION), get_from_node_primitive_type) {
  auto node = rclcpp::Node::make_shared("test_parameters_local_helpers");
  node->declare_parameter("foo", 0);
  node->declare_parameter("bar", "");
  node->declare_parameter("barstr", "");
  node->declare_parameter("baz", 0.);
  node->declare_parameter("foobar", false);
  node->declare_parameter("barfoo", std::vector<uint8_t>{});

  auto parameters_client = std::make_shared<rclcpp::SyncParametersClient>(node);
  if (!parameters_client->wait_for_service(20s)) {
    ASSERT_TRUE(false) << "service not available after waiting";
  }
  auto set_parameters_results = parameters_client->set_parameters(
  {
    rclcpp::Parameter("foo", 2),
    rclcpp::Parameter("bar", "hello"),
    rclcpp::Parameter("barstr", std::string("hello_str")),
    rclcpp::Parameter("baz", 1.45),
    rclcpp::Parameter("foobar", true),
    rclcpp::Parameter("barfoo", std::vector<uint8_t>{3, 4, 5}),
  }, 1s);
  printf("Got set_parameters result\n");

  // Check to see if they were set.
  for (auto & result : set_parameters_results) {
    ASSERT_TRUE(result.successful);
  }

  bool got_param = false;

  int64_t foo = 0;
  std::string foostr;

  std::string bar = "bar";
  double baz = 0.0;
  bool foobar = false;
  std::vector<uint8_t> barfoo(3);

  EXPECT_NO_THROW(got_param = node->get_parameter("foo", foo));
  EXPECT_EQ(true, got_param);
  EXPECT_EQ(2, foo);

  // Throw on type error
  EXPECT_THROW(
    got_param = node->get_parameter("foo", foostr),
    rclcpp::exceptions::InvalidParameterTypeException);

  // No throw on non-existent param, param shouldn't change
  foo = 1000;
  EXPECT_NO_THROW(got_param = node->get_parameter("no_such_param", foo));
  EXPECT_FALSE(got_param);
  EXPECT_EQ(1000, foo);

  EXPECT_NO_THROW(got_param = node->get_parameter("bar", bar));
  EXPECT_EQ(true, got_param);
  EXPECT_EQ("hello", bar);

  EXPECT_NO_THROW(got_param = node->get_parameter("baz", baz));
  EXPECT_EQ(true, got_param);
  EXPECT_DOUBLE_EQ(1.45, baz);

  EXPECT_NO_THROW(got_param = node->get_parameter("foobar", foobar));
  EXPECT_EQ(true, got_param);
  EXPECT_EQ(true, foobar);

  EXPECT_NO_THROW(got_param = node->get_parameter("barfoo", barfoo));
  EXPECT_EQ(true, got_param);
  EXPECT_EQ(barfoo[0], 3);
  EXPECT_EQ(barfoo[1], 4);
  EXPECT_EQ(barfoo[2], 5);
}

TEST_F(CLASSNAME(test_local_parameters, RMW_IMPLEMENTATION), get_from_node_variant_type) {
  using rclcpp::Parameter;

  auto node = rclcpp::Node::make_shared("test_parameters_local_helpers");
  node->declare_parameter("foo", 0);
  node->declare_parameter("bar", "");
  node->declare_parameter("barstr", "");
  node->declare_parameter("baz", 0.);
  node->declare_parameter("foobar", false);
  node->declare_parameter("barfoo", std::vector<uint8_t>{});

  auto parameters_client = std::make_shared<rclcpp::SyncParametersClient>(node);
  if (!parameters_client->wait_for_service(20s)) {
    ASSERT_TRUE(false) << "service not available after waiting";
  }
  auto set_parameters_results = parameters_client->set_parameters(
  {
    Parameter("foo", 2),
    Parameter("bar", "hello"),
    Parameter("barstr", std::string("hello_str")),
    Parameter("baz", 1.45),
    Parameter("foobar", true),
    Parameter("barfoo", std::vector<uint8_t>{3, 4, 5}),
  }, 1s);
  printf("Got set_parameters result\n");

  // Check to see if they were set.
  for (auto & result : set_parameters_results) {
    ASSERT_TRUE(result.successful);
  }

  bool got_param = false;

  Parameter foo;
  Parameter foostr;

  Parameter bar;
  Parameter baz;
  Parameter foobar;
  Parameter barfoo;

  EXPECT_NO_THROW(got_param = node->get_parameter("foo", foo));
  EXPECT_EQ(true, got_param);

  // No throw on non-existent param for reference passed version
  EXPECT_NO_THROW(got_param = node->get_parameter("no_such_param", foo));
  EXPECT_FALSE(got_param);

  // Throw on non-existent param for returning version
  EXPECT_THROW(
    node->get_parameter("no_such_param"),
    rclcpp::exceptions::ParameterNotDeclaredException);

  EXPECT_NO_THROW(got_param = node->get_parameter("bar", bar));
  EXPECT_EQ(true, got_param);

  EXPECT_NO_THROW(got_param = node->get_parameter("baz", baz));
  EXPECT_EQ(true, got_param);

  EXPECT_NO_THROW(got_param = node->get_parameter("foobar", foobar));
  EXPECT_EQ(true, got_param);

  EXPECT_NO_THROW(got_param = node->get_parameter("barfoo", barfoo));
  EXPECT_EQ(true, got_param);
}

int main(int argc, char ** argv)
{
  setvbuf(stdout, NULL, _IONBF, BUFSIZ);
  ::testing::InitGoogleTest(&argc, argv);
  int ret = RUN_ALL_TESTS();
  return ret;
}
