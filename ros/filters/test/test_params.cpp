/*
 * Copyright (c) 2008, Willow Garage, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Willow Garage, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "filters/param_test.hpp"

class ParametersTest : public ::testing::Test
{
protected:
  ParametersTest()
  {
    rclcpp::init(0, nullptr);
  }

  ~ParametersTest() override
  {
    rclcpp::shutdown();
  }

  template<typename T>
  rclcpp::Node::SharedPtr
  make_node_with_one_param(const T & value)
  {
    rclcpp::NodeOptions options;
    options.parameter_overrides().emplace_back("dummy.prefix.key", value);
    return std::make_shared<rclcpp::Node>("param_test", options);
  }
};

TEST_F(ParametersTest, Double)
{
  double epsilon = 1e-6;

  auto node = make_node_with_one_param(4.0);
  std::shared_ptr<filters::FilterBase<double>> filter =
    std::make_shared<filters::ParamTest<double>>();
  ASSERT_TRUE(
    filter->configure(
      "dummy.prefix", "TestDouble",
      node->get_node_logging_interface(), node->get_node_parameters_interface()));
  double out;
  filter->update(out, out);
  EXPECT_NEAR(4, out, epsilon);
}

TEST_F(ParametersTest, Int)
{
  auto node = make_node_with_one_param(static_cast<int>(4));
  std::shared_ptr<filters::FilterBase<int>> filter =
    std::make_shared<filters::ParamTest<int>>();
  ASSERT_TRUE(
    filter->configure(
      "dummy.prefix", "TestInt",
      node->get_node_logging_interface(), node->get_node_parameters_interface()));
  int out;
  filter->update(out, out);
  EXPECT_EQ(4, out);
}

TEST_F(ParametersTest, UInt)
{
  auto node = make_node_with_one_param(static_cast<int>(4));  // int because no unsigned param type
  std::shared_ptr<filters::FilterBase<unsigned int>> filter =
    std::make_shared<filters::ParamTest<unsigned int>>();
  ASSERT_TRUE(
    filter->configure(
      "dummy.prefix", "TestUInt",
      node->get_node_logging_interface(), node->get_node_parameters_interface()));
  unsigned int out;
  filter->update(out, out);
  EXPECT_EQ(4u, out);
}

TEST_F(ParametersTest, String)
{
  auto node = make_node_with_one_param(std::string("four"));
  std::shared_ptr<filters::FilterBase<std::string>> filter =
    std::make_shared<filters::ParamTest<std::string>>();
  ASSERT_TRUE(
    filter->configure(
      "dummy.prefix", "TestString",
      node->get_node_logging_interface(), node->get_node_parameters_interface()));
  std::string out;
  filter->update(out, out);
  EXPECT_STREQ("four", out.c_str());
}

TEST_F(ParametersTest, DoubleVector)
{
  double epsilon = 1e-6;

  auto node = make_node_with_one_param(std::vector<double>({4.0, 4.0, 4.0, 4.0}));
  std::shared_ptr<filters::FilterBase<std::vector<double>>> filter =
    std::make_shared<filters::ParamTest<std::vector<double>>>();
  ASSERT_TRUE(
    filter->configure(
      "dummy.prefix", "TestDoubleVector",
      node->get_node_logging_interface(), node->get_node_parameters_interface()));
  std::vector<double> out;
  filter->update(out, out);
  for (std::vector<double>::iterator it = out.begin(); it != out.end(); ++it) {
    EXPECT_NEAR(4, *it, epsilon);
  }
}

TEST_F(ParametersTest, StringVector)
{
  auto node = make_node_with_one_param(std::vector<std::string>({"four", "four", "four", "four"}));
  std::shared_ptr<filters::FilterBase<std::vector<std::string>>> filter =
    std::make_shared<filters::ParamTest<std::vector<std::string>>>();
  ASSERT_TRUE(
    filter->configure(
      "dummy.prefix", "TestStringVector",
      node->get_node_logging_interface(), node->get_node_parameters_interface()));
  std::vector<std::string> out;
  filter->update(out, out);
  for (std::vector<std::string>::iterator it = out.begin(); it != out.end(); ++it) {
    EXPECT_STREQ("four", it->c_str());
  }
}
