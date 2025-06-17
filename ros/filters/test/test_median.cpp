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

#include "filters/median.hpp"

class MedianFilterTest : public ::testing::Test
{
protected:
  MedianFilterTest()
  {
    rclcpp::init(0, nullptr);
    rclcpp::NodeOptions options;
    options.parameter_overrides().emplace_back("dummy.prefix.number_of_observations", 5);
    node_ = std::make_shared<rclcpp::Node>("median_filter_test", options);
  }

  ~MedianFilterTest() override
  {
    node_.reset();
    rclcpp::shutdown();
  }

  rclcpp::Node::SharedPtr node_;
};

TEST_F(MedianFilterTest, MultiChannelDoubleConfirmIdentityNRows)
{
  double epsilon = 1e-6;
  size_t length = 5;
  size_t rows = 5;

  std::shared_ptr<filters::MultiChannelFilterBase<double>> filter =
    std::make_shared<filters::MultiChannelMedianFilter<double>>();
  ASSERT_TRUE(
    filter->configure(
      rows, "dummy.prefix", "MultiChannelMedianFilterDouble5",
      node_->get_node_logging_interface(), node_->get_node_parameters_interface()));

  double input1[] = {1., 2., 3., 4., 5.};
  double input1a[] = {11., 12., 13., 14., 15.};
  std::vector<double> v1(input1, input1 + sizeof(input1) / sizeof(double));
  std::vector<double> v1a(input1a, input1a + sizeof(input1a) / sizeof(double));

  for (size_t i = 0; i < rows * 10; ++i) {
    EXPECT_TRUE(filter->update(v1, v1a));

    for (size_t j = 1; j < length; ++j) {
      EXPECT_NEAR(input1[j], v1a[j], epsilon);
    }
  }
}

TEST_F(MedianFilterTest, MultiChannelDoubleThreeRows)
{
  double epsilon = 1e-6;
  size_t length = 5;
  size_t rows = 5;

  std::shared_ptr<filters::MultiChannelFilterBase<double>> filter =
    std::make_shared<filters::MultiChannelMedianFilter<double>>();
  ASSERT_TRUE(
    filter->configure(
      rows, "dummy.prefix", "MultiChannelMedianFilterDouble5",
      node_->get_node_logging_interface(), node_->get_node_parameters_interface()));

  double input1[] = {0., 1., 2., 3., 4.};
  std::vector<double> v1(input1, input1 + sizeof(input1) / sizeof(double));
  double input2[] = {1., 2., 3., 4., 5.};
  std::vector<double> v2(input2, input2 + sizeof(input2) / sizeof(double));
  double input3[] = {2., 3., 4., 5., 6.};
  std::vector<double> v3(input3, input3 + sizeof(input3) / sizeof(double));
  double input1a[] = {1., 2., 3., 4., 5.};
  std::vector<double> v1a(input1a, input1a + sizeof(input1a) / sizeof(double));

  EXPECT_TRUE(filter->update(v1, v1a));
  EXPECT_TRUE(filter->update(v2, v1a));
  EXPECT_TRUE(filter->update(v3, v1a));

  for (size_t i = 1; i < length; i++) {
    EXPECT_NEAR(v2[i], v1a[i], epsilon);
  }
}

TEST_F(MedianFilterTest, MultiChannelFloatConfirmIdentityNRows)
{
  float epsilon = 1e-6;
  size_t length = 5;
  size_t rows = 5;

  std::shared_ptr<filters::MultiChannelFilterBase<float>> filter =
    std::make_shared<filters::MultiChannelMedianFilter<float>>();
  ASSERT_TRUE(
    filter->configure(
      rows, "dummy.prefix", "MultiChannelMedianFilterFloat5",
      node_->get_node_logging_interface(), node_->get_node_parameters_interface()));

  float input1[] = {1., 2., 3., 4., 5.};
  float input1a[] = {1., 2., 3., 4., 5.};
  std::vector<float> v1(input1, input1 + sizeof(input1) / sizeof(float));
  std::vector<float> v1a(input1a, input1a + sizeof(input1a) / sizeof(float));

  for (size_t i = 0; i < rows * 10; ++i) {
    EXPECT_TRUE(filter->update(v1, v1a));

    for (size_t j = 1; j < length; ++j) {
      EXPECT_NEAR(input1[j], v1a[j], epsilon);
    }
  }
}

TEST_F(MedianFilterTest, MultiChannelFloatThreeRows)
{
  float epsilon = 1e-6;
  size_t length = 5;
  size_t rows = 5;

  std::shared_ptr<filters::MultiChannelFilterBase<float>> filter =
    std::make_shared<filters::MultiChannelMedianFilter<float>>();
  ASSERT_TRUE(
    filter->configure(
      rows, "dummy.prefix", "MultiChannelMedianFilterFloat5",
      node_->get_node_logging_interface(), node_->get_node_parameters_interface()));

  float input1[] = {0., 1., 2., 3., 4.};
  std::vector<float> v1(input1, input1 + sizeof(input1) / sizeof(float));
  float input2[] = {1., 2., 3., 4., 5.};
  std::vector<float> v2(input2, input2 + sizeof(input2) / sizeof(float));
  float input3[] = {2., 3., 4., 5., 6.};
  std::vector<float> v3(input3, input3 + sizeof(input3) / sizeof(float));
  float input1a[] = {1., 2., 3., 4., 5.};
  std::vector<float> v1a(input1a, input1a + sizeof(input1a) / sizeof(float));

  EXPECT_TRUE(filter->update(v1, v1a));
  EXPECT_TRUE(filter->update(v2, v1a));
  EXPECT_TRUE(filter->update(v3, v1a));

  for (size_t i = 1; i < length; ++i) {
    EXPECT_NEAR(v2[i], v1a[i], epsilon);
  }
}
