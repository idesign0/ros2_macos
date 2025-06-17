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
#include <vector>

#include "filters/transfer_function.hpp"

class TransferFunctionTest : public ::testing::Test
{
protected:
  TransferFunctionTest()
  {
    rclcpp::init(0, nullptr);
  }

  ~TransferFunctionTest() override
  {
    rclcpp::shutdown();
  }

  rclcpp::Node::SharedPtr
  make_node_with_params(const std::vector<double> & a, const std::vector<double> & b)
  {
    rclcpp::NodeOptions options;
    options.parameter_overrides().emplace_back("dummy.prefix.a", a);
    options.parameter_overrides().emplace_back("dummy.prefix.b", b);
    return std::make_shared<rclcpp::Node>("transfer_function_test", options);
  }
};

TEST_F(TransferFunctionTest, LowPass)
{
  const size_t channels = 1;
  double epsilon = 1e-4;

  auto node = make_node_with_params(
    {1.0, -0.509525449494429},
    {0.245237275252786, 0.245237275252786});
  std::shared_ptr<filters::MultiChannelFilterBase<double>> filter =
    std::make_shared<filters::MultiChannelTransferFunctionFilter<double>>();
  ASSERT_TRUE(
    filter->configure(
      channels, "dummy.prefix", "LowPass",
      node->get_node_logging_interface(), node->get_node_parameters_interface()));


  std::vector<double> in1, in2, in3, in4, in5, in6, in7;
  std::vector<double> out1;

  in1.push_back(10.0);
  in2.push_back(70.0);
  in3.push_back(10.0);
  in4.push_back(44.0);
  in5.push_back(10.0);
  in6.push_back(5.0);
  in7.push_back(6.0);
  out1.push_back(11.8008);
  EXPECT_TRUE(filter->update(in1, in1));
  EXPECT_TRUE(filter->update(in2, in2));
  EXPECT_TRUE(filter->update(in3, in3));
  EXPECT_TRUE(filter->update(in4, in4));
  EXPECT_TRUE(filter->update(in5, in5));
  EXPECT_TRUE(filter->update(in6, in6));
  EXPECT_TRUE(filter->update(in7, in7));

  EXPECT_NEAR(out1[0], in7[0], epsilon);
}

TEST_F(TransferFunctionTest, SingleLowPass)
{
  double epsilon = 1e-4;

  auto node = make_node_with_params(
    {1.0, -0.509525449494429},
    {0.245237275252786, 0.245237275252786});
  std::shared_ptr<filters::FilterBase<double>> filter =
    std::make_shared<filters::SingleChannelTransferFunctionFilter<double>>();
  ASSERT_TRUE(
    filter->configure(
      "dummy.prefix", "LowPassSingle",
      node->get_node_logging_interface(), node->get_node_parameters_interface()));


  double in1, in2, in3, in4, in5, in6, in7;
  double out1;

  in1 = 10.0;
  in2 = 70.0;
  in3 = 10.0;
  in4 = 44.0;
  in5 = 10.0;
  in6 = 5.0;
  in7 = 6.0;
  out1 = 11.8008;
  EXPECT_TRUE(filter->update(in1, in1));
  EXPECT_TRUE(filter->update(in2, in2));
  EXPECT_TRUE(filter->update(in3, in3));
  EXPECT_TRUE(filter->update(in4, in4));
  EXPECT_TRUE(filter->update(in5, in5));
  EXPECT_TRUE(filter->update(in6, in6));
  EXPECT_TRUE(filter->update(in7, in7));

  EXPECT_NEAR(out1, in7, epsilon);
}


TEST_F(TransferFunctionTest, LowPassNonUnity)
{
  const size_t channels = 1;
  double epsilon = 1e-4;

  auto node = make_node_with_params(
    {2.0, -0.509525449494429},
    {0.245237275252786, 0.245237275252786});
  std::shared_ptr<filters::MultiChannelFilterBase<double>> filter =
    std::make_shared<filters::MultiChannelTransferFunctionFilter<double>>();
  ASSERT_TRUE(
    filter->configure(
      channels, "dummy.prefix", "LowPassNonUnity",
      node->get_node_logging_interface(), node->get_node_parameters_interface()));

  std::vector<double> in1, in2, in3, in4, in5, in6, in7;
  std::vector<double> out1;

  in1.push_back(10.0);
  in2.push_back(70.0);
  in3.push_back(10.0);
  in4.push_back(44.0);
  in5.push_back(10.0);
  in6.push_back(5.0);
  in7.push_back(6.0);
  out1.push_back(2.4088);
  EXPECT_TRUE(filter->update(in1, in1));
  EXPECT_TRUE(filter->update(in2, in2));
  EXPECT_TRUE(filter->update(in3, in3));
  EXPECT_TRUE(filter->update(in4, in4));
  EXPECT_TRUE(filter->update(in5, in5));
  EXPECT_TRUE(filter->update(in6, in6));
  EXPECT_TRUE(filter->update(in7, in7));

  EXPECT_NEAR(out1[0], in7[0], epsilon);
}

TEST_F(TransferFunctionTest, LowPassMulti)
{
  const size_t channels = 3;
  double epsilon = 1e-4;

  auto node = make_node_with_params(
    {1.0, -1.760041880343169, 1.182893262037831, -0.278059917634546},
    {0.018098933007514, 0.245237275252786, 0.054296799022543, 0.018098933007514});
  std::shared_ptr<filters::MultiChannelFilterBase<double>> filter =
    std::make_shared<filters::MultiChannelTransferFunctionFilter<double>>();
  ASSERT_TRUE(
    filter->configure(
      channels, "dummy.prefix", "LowPassMulti",
      node->get_node_logging_interface(), node->get_node_parameters_interface()));

  std::vector<double> in1, in2, in3, in4, in5, in6, in7;
  std::vector<double> out1;

  in1.push_back(10.0);
  in1.push_back(10.0);
  in1.push_back(10.0);
  //
  in2.push_back(70.0);
  in2.push_back(30.0);
  in2.push_back(8.0);
  //
  in3.push_back(-1.0);
  in3.push_back(5.0);
  in3.push_back(22.0);
  //
  in4.push_back(44.0);
  in4.push_back(23.0);
  in4.push_back(8.0);
  //
  in5.push_back(10.0);
  in5.push_back(10.0);
  in5.push_back(10.0);
  //
  in6.push_back(5.0);
  in6.push_back(-1.0);
  in6.push_back(5.0);
  //
  in7.push_back(6.0);
  in7.push_back(-30.0);
  in7.push_back(2.0);
  //
  out1.push_back(60.6216);
  out1.push_back(33.9829);
  out1.push_back(28.1027);
  EXPECT_TRUE(filter->update(in1, in1));
  EXPECT_TRUE(filter->update(in2, in2));
  EXPECT_TRUE(filter->update(in3, in3));
  EXPECT_TRUE(filter->update(in4, in4));
  EXPECT_TRUE(filter->update(in5, in5));
  EXPECT_TRUE(filter->update(in6, in6));
  EXPECT_TRUE(filter->update(in7, in7));

  for (size_t i = 0; i < out1.size(); i++) {
    EXPECT_NEAR(out1[i], in7[i], epsilon);
  }
}

TEST_F(TransferFunctionTest, LowPassIrrational)
{
  const size_t channels = 3;
  double epsilon = 1e-4;

  auto node = make_node_with_params(
    {1.0, -1.760041880343169, 1.182893262037831},
    {0.018098933007514, 0.054296799022543, 0.054296799022543, 0.018098933007514});
  std::shared_ptr<filters::MultiChannelFilterBase<double>> filter =
    std::make_shared<filters::MultiChannelTransferFunctionFilter<double>>();
  ASSERT_TRUE(
    filter->configure(
      channels, "dummy.prefix", "LowPassIrrational",
      node->get_node_logging_interface(), node->get_node_parameters_interface()));

  std::vector<double> in1, in2, in3, in4, in5, in6, in7;
  std::vector<double> out1;

  in1.push_back(10.0);
  in1.push_back(10.0);
  in1.push_back(10.0);
  //
  in2.push_back(70.0);
  in2.push_back(30.0);
  in2.push_back(8.0);
  //
  in3.push_back(-1.0);
  in3.push_back(5.0);
  in3.push_back(22.0);
  //
  in4.push_back(44.0);
  in4.push_back(23.0);
  in4.push_back(8.0);
  //
  in5.push_back(10.0);
  in5.push_back(10.0);
  in5.push_back(10.0);
  //
  in6.push_back(5.0);
  in6.push_back(-1.0);
  in6.push_back(5.0);
  //
  in7.push_back(6.0);
  in7.push_back(-30.0);
  in7.push_back(2.0);
  //
  out1.push_back(17.1112);
  out1.push_back(9.0285);
  out1.push_back(8.3102);
  EXPECT_TRUE(filter->update(in1, in1));
  EXPECT_TRUE(filter->update(in2, in2));
  EXPECT_TRUE(filter->update(in3, in3));
  EXPECT_TRUE(filter->update(in4, in4));
  EXPECT_TRUE(filter->update(in5, in5));
  EXPECT_TRUE(filter->update(in6, in6));
  EXPECT_TRUE(filter->update(in7, in7));

  for (size_t i = 0; i < out1.size(); i++) {
    EXPECT_NEAR(out1[i], in7[i], epsilon);
  }
}
