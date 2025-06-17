// Copyright 2019 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

#include <gtest/gtest.h>

#include <iostream>
#include <memory>
#include <string>

#include "libstatistics_collector/moving_average_statistics/types.hpp"

#include "libstatistics_collector/collector/collector.hpp"

namespace
{
constexpr const char kTestMetricName[] = "test_metric_name";
constexpr const char kTestMetricUnit[] = "test_metric_unit";
}

/**
 * Simple extension to test basic functionality
 */
class TestCollector : public libstatistics_collector::collector::Collector
{
public:
  TestCollector() = default;
  ~TestCollector() override = default;

  /**
   * Overrides to make public for testing.
   * @return
   */
  bool Start() override
  {
    return Collector::Start();
  }
  /**
   * Overrides to make public for testing.
   * @return
   */
  bool Stop() override
  {
    return Collector::Stop();
  }

  bool SetupStart() override
  {
    return true;
  }
  bool SetupStop() override
  {
    this->ClearCurrentMeasurements();
    return true;
  }

  std::string GetMetricName() const override
  {
    return kTestMetricName;
  }

  std::string GetMetricUnit() const override
  {
    return kTestMetricUnit;
  }
};

/**
 * Test fixture
 */
class CollectorTestFixure : public ::testing::Test
{
public:
  void SetUp() override
  {
    test_collector_ = std::make_unique<TestCollector>();
    ASSERT_FALSE(test_collector_->IsStarted());
  }

  void TearDown() override
  {
    test_collector_->Stop();  // don't assert as tests can call stop
    ASSERT_FALSE(test_collector_->IsStarted());
    test_collector_.reset();
  }

protected:
  std::unique_ptr<TestCollector> test_collector_{};
};

TEST_F(CollectorTestFixure, Sanity) {
  ASSERT_NE(test_collector_, nullptr);
}

TEST_F(CollectorTestFixure, TestAlreadyStarted) {
  ASSERT_NE(test_collector_, nullptr);
  EXPECT_TRUE(test_collector_->Start()) << "Expected true upon first call to start";
  EXPECT_FALSE(test_collector_->Start()) << "Expected false upon subsequent calls to start";
}

TEST_F(CollectorTestFixure, TestAddAndClearMeasurement) {
  test_collector_->AcceptData(1);
  auto stats = test_collector_->GetStatisticsResults();
  ASSERT_EQ(1, stats.sample_count);
  ASSERT_EQ(1, stats.average);

  test_collector_->ClearCurrentMeasurements();

  stats = test_collector_->GetStatisticsResults();
  ASSERT_TRUE(std::isnan(stats.average));
  ASSERT_TRUE(std::isnan(stats.min));
  ASSERT_TRUE(std::isnan(stats.max));
  ASSERT_TRUE(std::isnan(stats.standard_deviation));
  ASSERT_EQ(0, stats.sample_count);
}

TEST_F(CollectorTestFixure, TestStartAndStop) {
  ASSERT_FALSE(test_collector_->IsStarted());
  ASSERT_EQ(
    "started=false, avg=nan, min=nan, max=nan, std_dev=nan, count=0",
    test_collector_->GetStatusString());

  ASSERT_TRUE(test_collector_->Start());
  ASSERT_TRUE(test_collector_->IsStarted());
  ASSERT_EQ(
    "started=true, avg=nan, min=nan, max=nan, std_dev=nan, count=0",
    test_collector_->GetStatusString());

  ASSERT_TRUE(test_collector_->Stop());
  ASSERT_FALSE(test_collector_->IsStarted());
}

TEST_F(CollectorTestFixure, TestGetMetricNameAndUnit) {
  EXPECT_FALSE(test_collector_->GetMetricName().empty());
  EXPECT_FALSE(test_collector_->GetMetricUnit().empty());
}
