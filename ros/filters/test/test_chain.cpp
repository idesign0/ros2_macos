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

#include "filters/filter_chain.hpp"

class ChainTest : public ::testing::Test
{
protected:
  ChainTest()
  {
    rclcpp::init(0, nullptr);
  }

  ~ChainTest() override
  {
    rclcpp::shutdown();
  }

  rclcpp::Node::SharedPtr
  make_node_with_params(const std::vector<rclcpp::Parameter> & overrides)
  {
    rclcpp::NodeOptions options;
    options.parameter_overrides() = overrides;
    return std::make_shared<rclcpp::Node>("chain_test", options);
  }
};


TEST_F(ChainTest, multi_channel_configuring) {
  double epsilon = 1e-9;
  filters::MultiChannelFilterChain<double> chain("double");

  std::vector<rclcpp::Parameter> overrides;
  overrides.emplace_back("MultiChannelMeanFilterDouble5.filter1.name", std::string("mean_test"));
  overrides.emplace_back(
    "MultiChannelMeanFilterDouble5.filter1.type",
    std::string("filters/MultiChannelMeanFilterDouble"));
  overrides.emplace_back("MultiChannelMeanFilterDouble5.filter1.params.number_of_observations", 5);
  auto node = make_node_with_params(overrides);
  ASSERT_TRUE(
    chain.configure(
      5, "MultiChannelMeanFilterDouble5",
      node->get_node_logging_interface(), node->get_node_parameters_interface()));

  double input1[] = {1., 2., 3., 4., 5.};
  double input1a[] = {9., 9., 9., 9., 9.};  // seed w/incorrect values
  std::vector<double> v1(input1, input1 + sizeof(input1) / sizeof(double));
  std::vector<double> v1a(input1a, input1a + sizeof(input1a) / sizeof(double));


  EXPECT_TRUE(chain.update(v1, v1a));

  chain.clear();

  for (size_t i = 1; i < v1.size(); ++i) {
    EXPECT_NEAR(input1[i], v1a[i], epsilon);
  }
}

TEST_F(ChainTest, configuring) {
  double epsilon = 1e-9;
  std::vector<rclcpp::Parameter> overrides;
  overrides.emplace_back("MeanFilterFloat5.filter1.name", std::string("mean_test"));
  overrides.emplace_back("MeanFilterFloat5.filter1.type", std::string("filters/MeanFilterFloat"));
  overrides.emplace_back("MeanFilterFloat5.filter1.params.number_of_observations", 5);
  auto node = make_node_with_params(overrides);

  filters::FilterChain<float> chain("float");

  ASSERT_TRUE(
    chain.configure(
      "MeanFilterFloat5", node->get_node_logging_interface(),
      node->get_node_parameters_interface()));

  float v1 = 1.;
  float v1a = 9.;

  EXPECT_TRUE(chain.update(v1, v1a));

  chain.clear();

  EXPECT_NEAR(v1, v1a, epsilon);
}

/**
 * In order to parallelize filtering, users may want to instantiate multiple filter chains using
 * one set of parameters. In this case we need to not error out if the parameters have already
 * beend declared by one of the other filter chain instances.
 */
TEST_F(ChainTest, ParallelChainConfiguring) {
  double epsilon = 1e-9;
  std::vector<rclcpp::Parameter> overrides;
  overrides.emplace_back(
    "ParallelChainMeanFilter.filter1.name",
    std::string("parallel_chain_test"));
  overrides.emplace_back(
    "ParallelChainMeanFilter.filter1.type",
    std::string("filters/MeanFilterFloat"));
  overrides.emplace_back("ParallelChainMeanFilter.filter1.params.number_of_observations", 5);
  auto node = make_node_with_params(overrides);

  std::vector<std::shared_ptr<filters::FilterChain<float>>> chains {
    std::make_shared<filters::FilterChain<float>>("float"),
    std::make_shared<filters::FilterChain<float>>("float")};

  for (auto & chain : chains) {
    ASSERT_TRUE(
      chain->configure(
        "ParallelChainMeanFilter", node->get_node_logging_interface(),
        node->get_node_parameters_interface()));
  }

  for (auto & chain : chains) {
    float v1 = 1.;
    float v1a = 9.;

    EXPECT_TRUE(chain->update(v1, v1a));

    chain->clear();

    EXPECT_NEAR(v1, v1a, epsilon);
  }
}

TEST_F(ChainTest, MisconfiguredNumberOfChannels) {
  filters::MultiChannelFilterChain<double> chain("double");

  std::vector<rclcpp::Parameter> overrides;
  overrides.emplace_back(
    "MultiChannelMedianFilterDouble5.filter1.name",
    std::string("median_test"));
  overrides.emplace_back(
    "MultiChannelMedianFilterDouble5.filter1.type",
    std::string("filters/MultiChannelMedianFilterDouble"));
  overrides.emplace_back(
    "MultiChannelMedianFilterDouble5.filter1.params.number_of_observations",
    5);
  auto node = make_node_with_params(overrides);

  ASSERT_TRUE(
    chain.configure(
      10, "MultiChannelMedianFilterDouble5",
      node->get_node_logging_interface(), node->get_node_parameters_interface()));

  double input1[] = {1., 2., 3., 4., 5.};
  double input1a[] = {1., 2., 3., 4., 5.};
  std::vector<double> v1(input1, input1 + sizeof(input1) / sizeof(double));
  std::vector<double> v1a(input1a, input1a + sizeof(input1a) / sizeof(double));

  EXPECT_FALSE(chain.update(v1, v1a));
}

TEST_F(ChainTest, MultiChannelTwoFilters) {
  double epsilon = 1e-9;
  filters::MultiChannelFilterChain<double> chain("double");

  std::vector<rclcpp::Parameter> overrides;
  overrides.emplace_back("TwoFilters.filter1.name", std::string("median_test_unique"));
  overrides.emplace_back(
    "TwoFilters.filter1.type",
    std::string("filters/MultiChannelMedianFilterDouble"));
  overrides.emplace_back("TwoFilters.filter1.params.number_of_observations", 5);
  overrides.emplace_back("TwoFilters.filter2.name", std::string("median_test2"));
  overrides.emplace_back(
    "TwoFilters.filter2.type",
    std::string("filters/MultiChannelMedianFilterDouble"));
  overrides.emplace_back("TwoFilters.filter2.params.number_of_observations", 5);
  auto node = make_node_with_params(overrides);

  ASSERT_TRUE(
    chain.configure(
      5, "TwoFilters",
      node->get_node_logging_interface(), node->get_node_parameters_interface()));

  double input1[] = {1., 2., 3., 4., 5.};
  double input1a[] = {9., 9., 9., 9., 9.};  // seed w/incorrect values
  std::vector<double> v1(input1, input1 + sizeof(input1) / sizeof(double));
  std::vector<double> v1a(input1a, input1a + sizeof(input1a) / sizeof(double));

  EXPECT_TRUE(chain.update(v1, v1a));

  chain.clear();

  for (size_t i = 1; i < v1.size(); i++) {
    EXPECT_NEAR(input1[i], v1a[i], epsilon);
  }
}


TEST_F(ChainTest, TransferFunction) {
  double epsilon = 1e-4;

  filters::MultiChannelFilterChain<double> chain("double");

  std::vector<rclcpp::Parameter> overrides;
  overrides.emplace_back("TransferFunction.filter1.name", std::string("transfer_function"));
  overrides.emplace_back(
    "TransferFunction.filter1.type",
    std::string("filters/MultiChannelTransferFunctionFilterDouble"));
  overrides.emplace_back(
    "TransferFunction.filter1.params.a",
    std::vector<double>({1.0, -1.760041880343169, 1.182893262037831}));
  overrides.emplace_back(
    "TransferFunction.filter1.params.b",
    std::vector<double>(
      {0.018098933007514, 0.054296799022543, 0.054296799022543,
        0.018098933007514}));
  auto node = make_node_with_params(overrides);
  ASSERT_TRUE(
    chain.configure(
      3, "TransferFunction",
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
  EXPECT_TRUE(chain.update(in1, in1));
  EXPECT_TRUE(chain.update(in2, in2));
  EXPECT_TRUE(chain.update(in3, in3));
  EXPECT_TRUE(chain.update(in4, in4));
  EXPECT_TRUE(chain.update(in5, in5));
  EXPECT_TRUE(chain.update(in6, in6));
  EXPECT_TRUE(chain.update(in7, in7));

  chain.clear();

  for (size_t i = 0; i < out1.size(); i++) {
    EXPECT_NEAR(out1[i], in7[i], epsilon);
  }
}

TEST_F(ChainTest, ReconfiguringChain) {
  filters::FilterChain<int> chain("int");

  int v1 = 1;
  int v1a = 9;

  std::vector<rclcpp::Parameter> overrides;
  overrides.emplace_back("OneIncrements.filter1.name", std::string("increment1"));
  overrides.emplace_back("OneIncrements.filter1.type", std::string("filters/IncrementFilterInt"));
  overrides.emplace_back("TwoIncrements.filter1.name", std::string("increment1"));
  overrides.emplace_back("TwoIncrements.filter1.type", std::string("filters/IncrementFilterInt"));
  overrides.emplace_back("TwoIncrements.filter2.name", std::string("increment2"));
  overrides.emplace_back("TwoIncrements.filter2.type", std::string("filters/IncrementFilterInt"));
  auto node = make_node_with_params(overrides);

  ASSERT_TRUE(
    chain.configure(
      "OneIncrements", node->get_node_logging_interface(), node->get_node_parameters_interface()));
  EXPECT_TRUE(chain.update(v1, v1a));
  EXPECT_EQ(2, v1a);
  chain.clear();

  ASSERT_TRUE(
    chain.configure(
      "TwoIncrements", node->get_node_logging_interface(), node->get_node_parameters_interface()));
  EXPECT_TRUE(chain.update(v1, v1a));
  EXPECT_EQ(3, v1a);
}

TEST_F(ChainTest, ThreeIncrementChains) {
  filters::FilterChain<int> chain("int");
  int v1 = 1;
  int v1a = 9;

  std::vector<rclcpp::Parameter> overrides;
  overrides.emplace_back("ThreeIncrements.filter1.name", std::string("increment1"));
  overrides.emplace_back("ThreeIncrements.filter1.type", std::string("filters/IncrementFilterInt"));
  overrides.emplace_back("ThreeIncrements.filter2.name", std::string("increment2"));
  overrides.emplace_back("ThreeIncrements.filter2.type", std::string("filters/IncrementFilterInt"));
  overrides.emplace_back("ThreeIncrements.filter3.name", std::string("increment3"));
  overrides.emplace_back("ThreeIncrements.filter3.type", std::string("filters/IncrementFilterInt"));
  auto node = make_node_with_params(overrides);

  ASSERT_TRUE(
    chain.configure(
      "ThreeIncrements", node->get_node_logging_interface(),
      node->get_node_parameters_interface()));
  EXPECT_TRUE(chain.update(v1, v1a));
  EXPECT_EQ(4, v1a);
}

TEST_F(ChainTest, TenIncrementChains) {
  filters::FilterChain<int> chain("int");
  int v1 = 1;
  int v1a = 9;

  std::vector<rclcpp::Parameter> overrides;
  overrides.emplace_back("TenIncrements.filter1.name", std::string("increment1"));
  overrides.emplace_back("TenIncrements.filter1.type", std::string("filters/IncrementFilterInt"));
  overrides.emplace_back("TenIncrements.filter2.name", std::string("increment2"));
  overrides.emplace_back("TenIncrements.filter2.type", std::string("filters/IncrementFilterInt"));
  overrides.emplace_back("TenIncrements.filter3.name", std::string("increment3"));
  overrides.emplace_back("TenIncrements.filter3.type", std::string("filters/IncrementFilterInt"));
  overrides.emplace_back("TenIncrements.filter4.name", std::string("increment4"));
  overrides.emplace_back("TenIncrements.filter4.type", std::string("filters/IncrementFilterInt"));
  overrides.emplace_back("TenIncrements.filter5.name", std::string("increment5"));
  overrides.emplace_back("TenIncrements.filter5.type", std::string("filters/IncrementFilterInt"));
  overrides.emplace_back("TenIncrements.filter6.name", std::string("increment6"));
  overrides.emplace_back("TenIncrements.filter6.type", std::string("filters/IncrementFilterInt"));
  overrides.emplace_back("TenIncrements.filter7.name", std::string("increment7"));
  overrides.emplace_back("TenIncrements.filter7.type", std::string("filters/IncrementFilterInt"));
  overrides.emplace_back("TenIncrements.filter8.name", std::string("increment8"));
  overrides.emplace_back("TenIncrements.filter8.type", std::string("filters/IncrementFilterInt"));
  overrides.emplace_back("TenIncrements.filter9.name", std::string("increment9"));
  overrides.emplace_back("TenIncrements.filter9.type", std::string("filters/IncrementFilterInt"));
  overrides.emplace_back("TenIncrements.filter10.name", std::string("increment10"));
  overrides.emplace_back("TenIncrements.filter10.type", std::string("filters/IncrementFilterInt"));
  auto node = make_node_with_params(overrides);

  ASSERT_TRUE(
    chain.configure(
      "TenIncrements", node->get_node_logging_interface(), node->get_node_parameters_interface()));
  EXPECT_TRUE(chain.update(v1, v1a));
  EXPECT_EQ(11, v1a);
}

TEST_F(ChainTest, TenMultiChannelIncrementChains) {
  filters::MultiChannelFilterChain<int> chain("int");
  std::vector<int> v1;
  v1.push_back(1);
  v1.push_back(1);
  v1.push_back(1);
  std::vector<int> v1a = v1;

  std::vector<rclcpp::Parameter> overrides;
  overrides.emplace_back("TenMultiChannelIncrements.filter1.name", std::string("increment1"));
  overrides.emplace_back(
    "TenMultiChannelIncrements.filter1.type",
    std::string("filters/MultiChannelIncrementFilterInt"));
  overrides.emplace_back("TenMultiChannelIncrements.filter2.name", std::string("increment2"));
  overrides.emplace_back(
    "TenMultiChannelIncrements.filter2.type",
    std::string("filters/MultiChannelIncrementFilterInt"));
  overrides.emplace_back("TenMultiChannelIncrements.filter3.name", std::string("increment3"));
  overrides.emplace_back(
    "TenMultiChannelIncrements.filter3.type",
    std::string("filters/MultiChannelIncrementFilterInt"));
  overrides.emplace_back("TenMultiChannelIncrements.filter4.name", std::string("increment4"));
  overrides.emplace_back(
    "TenMultiChannelIncrements.filter4.type",
    std::string("filters/MultiChannelIncrementFilterInt"));
  overrides.emplace_back("TenMultiChannelIncrements.filter5.name", std::string("increment5"));
  overrides.emplace_back(
    "TenMultiChannelIncrements.filter5.type",
    std::string("filters/MultiChannelIncrementFilterInt"));
  overrides.emplace_back("TenMultiChannelIncrements.filter6.name", std::string("increment6"));
  overrides.emplace_back(
    "TenMultiChannelIncrements.filter6.type",
    std::string("filters/MultiChannelIncrementFilterInt"));
  overrides.emplace_back("TenMultiChannelIncrements.filter7.name", std::string("increment7"));
  overrides.emplace_back(
    "TenMultiChannelIncrements.filter7.type",
    std::string("filters/MultiChannelIncrementFilterInt"));
  overrides.emplace_back("TenMultiChannelIncrements.filter8.name", std::string("increment8"));
  overrides.emplace_back(
    "TenMultiChannelIncrements.filter8.type",
    std::string("filters/MultiChannelIncrementFilterInt"));
  overrides.emplace_back("TenMultiChannelIncrements.filter9.name", std::string("increment9"));
  overrides.emplace_back(
    "TenMultiChannelIncrements.filter9.type",
    std::string("filters/MultiChannelIncrementFilterInt"));
  overrides.emplace_back("TenMultiChannelIncrements.filter10.name", std::string("increment10"));
  overrides.emplace_back(
    "TenMultiChannelIncrements.filter10.type",
    std::string("filters/MultiChannelIncrementFilterInt"));
  auto node = make_node_with_params(overrides);

  ASSERT_TRUE(
    chain.configure(
      3, "TenMultiChannelIncrements", node->get_node_logging_interface(),
      node->get_node_parameters_interface()));
  EXPECT_TRUE(chain.update(v1, v1a));
  for (size_t i = 0; i < 3; ++i) {
    EXPECT_EQ(11, v1a[i]);
  }
}
