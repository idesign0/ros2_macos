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

#include <array>
#include <atomic>
#include <memory>
#include <thread>

#include "libstatistics_collector/moving_average_statistics/moving_average.hpp"

namespace
{
using libstatistics_collector::moving_average_statistics::MovingAverageStatistics;

// Useful testing constants
constexpr const uint64_t kExpectedSize = 9;
constexpr const double kExpectedAvg = 3.4;
constexpr const double kExpectedMin = -3.5;
constexpr const double kExpectedMax = 11.0;
constexpr const double kExpectedStd = 4.997999599839919955173;
const std::array<double, kExpectedSize> kTestData{-3.5, -2.1, -1.1, 0.0, 4.7, 5.0,
  6.7, 9.9, 11.0};
}  // namespace

/**
 * Test fixture
 */
class MovingAverageStatisticsTestFixture : public ::testing::Test
{
public:
  void SetUp() override
  {
    moving_average_statistics_ = std::make_unique<MovingAverageStatistics>();

    for (double d : kTestData) {
      moving_average_statistics_->AddMeasurement(d);
      ASSERT_EQ(++expected_count_, moving_average_statistics_->GetCount());
    }
  }

  void TearDown() override
  {
    moving_average_statistics_->Reset();
    moving_average_statistics_.reset();
  }

protected:
  std::unique_ptr<MovingAverageStatistics> moving_average_statistics_;
  int expected_count_ = 0;
};

TEST_F(MovingAverageStatisticsTestFixture, TestAddNanIgnore) {
  const auto stats1 = StatisticsDataToString(moving_average_statistics_->GetStatistics());
  moving_average_statistics_->AddMeasurement(std::nan(""));
  const auto stats2 = StatisticsDataToString(moving_average_statistics_->GetStatistics());
  ASSERT_EQ(stats1, stats2);
}

TEST_F(MovingAverageStatisticsTestFixture, Sanity) {
  ASSERT_NE(moving_average_statistics_, nullptr);
}

TEST_F(MovingAverageStatisticsTestFixture, TestAverage) {
  EXPECT_DOUBLE_EQ(moving_average_statistics_->Average(), kExpectedAvg);
}

TEST_F(MovingAverageStatisticsTestFixture, TestMaximum) {
  EXPECT_EQ(moving_average_statistics_->Max(), kExpectedMax);
}

TEST_F(MovingAverageStatisticsTestFixture, TestMinimum) {
  EXPECT_EQ(moving_average_statistics_->Min(), kExpectedMin);
}

TEST_F(MovingAverageStatisticsTestFixture, TestStandardDeviation) {
  EXPECT_DOUBLE_EQ(moving_average_statistics_->StandardDeviation(), kExpectedStd);
}

TEST_F(MovingAverageStatisticsTestFixture, TestAverageEmpty) {
  MovingAverageStatistics empty;
  ASSERT_TRUE(std::isnan(empty.Average()));
}

TEST_F(MovingAverageStatisticsTestFixture, TestMaximumEmpty) {
  MovingAverageStatistics empty;
  ASSERT_TRUE(std::isnan(empty.Max()));
}

TEST_F(MovingAverageStatisticsTestFixture, TestMinimumEmpty) {
  MovingAverageStatistics empty;
  ASSERT_TRUE(std::isnan(empty.Min()));
}

TEST_F(MovingAverageStatisticsTestFixture, TestStddevEmpty) {
  MovingAverageStatistics empty;
  ASSERT_TRUE(std::isnan(empty.StandardDeviation()));
}

TEST_F(MovingAverageStatisticsTestFixture, TestCountEmpty) {
  MovingAverageStatistics empty;
  ASSERT_EQ(0, empty.GetCount());
}

TEST_F(MovingAverageStatisticsTestFixture, TestGetStatistics) {
  const auto result = moving_average_statistics_->GetStatistics();
  EXPECT_DOUBLE_EQ(result.average, kExpectedAvg);
  EXPECT_DOUBLE_EQ(result.min, kExpectedMin);
  EXPECT_DOUBLE_EQ(result.max, kExpectedMax);
  EXPECT_DOUBLE_EQ(result.standard_deviation, kExpectedStd);
  EXPECT_EQ(result.sample_count, kExpectedSize);
}

TEST_F(MovingAverageStatisticsTestFixture, TestGetStatisticsInt) {
  moving_average_statistics_->Reset();

  const auto data_int = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  const double kExpectedAverage = 5.5;
  const double kExpectedMinimum = 1;
  const double kExpectedMaximum = 10;
  const double kExpectedStd = 2.8722813232690143;
  const int kExpectedSize = 10;

  for (int d : data_int) {
    moving_average_statistics_->AddMeasurement(d);
  }

  auto result = moving_average_statistics_->GetStatistics();
  EXPECT_DOUBLE_EQ(result.average, kExpectedAverage);
  EXPECT_DOUBLE_EQ(result.min, kExpectedMinimum);
  EXPECT_DOUBLE_EQ(result.max, kExpectedMaximum);
  EXPECT_DOUBLE_EQ(result.standard_deviation, kExpectedStd);
  EXPECT_EQ(result.sample_count, kExpectedSize);
}

TEST_F(MovingAverageStatisticsTestFixture, TestGetStatisticsAllNegative) {
  moving_average_statistics_->Reset();

  constexpr double data_negative[] = {-1.f, -2.f, -3.f, -4.f, -5.f, -6.f, -7.f, -8.f, -9.f, -10.f};

  constexpr double kExpectedAverage = -5.5;
  constexpr double kExpectedMinimum = -10;
  constexpr double kExpectedMaximum = -1;
  constexpr double kExpectedStd = 2.8722813232690143;
  constexpr int kExpectedSize = 10;

  for (auto d : data_negative) {
    moving_average_statistics_->AddMeasurement(d);
  }

  auto result = moving_average_statistics_->GetStatistics();
  EXPECT_DOUBLE_EQ(result.average, kExpectedAverage);
  EXPECT_DOUBLE_EQ(result.min, kExpectedMinimum);
  EXPECT_DOUBLE_EQ(result.max, kExpectedMaximum);
  EXPECT_DOUBLE_EQ(result.standard_deviation, kExpectedStd);
  EXPECT_EQ(result.sample_count, kExpectedSize);
}

TEST_F(MovingAverageStatisticsTestFixture, TestReset) {
  moving_average_statistics_->AddMeasurement(0.6);
  moving_average_statistics_->Reset();
  ASSERT_TRUE(std::isnan(moving_average_statistics_->Average()));
  moving_average_statistics_->AddMeasurement(1.5);
  EXPECT_EQ(moving_average_statistics_->Average(), 1.5);
}

TEST_F(MovingAverageStatisticsTestFixture, TestThreadSafe) {
  moving_average_statistics_->Reset();

  std::atomic<int> total_sum{0};
  std::atomic<int> count{0};

  std::thread t1([this, &count, &total_sum]() {
      for (int i = 1; i < 1101; i++) {
        moving_average_statistics_->AddMeasurement(static_cast<double>(i));
        count++;
        total_sum += i;
      }
    });
  std::thread t2([this, &count, &total_sum]() {
      for (int i = 1; i < 2101; i++) {
        moving_average_statistics_->AddMeasurement(static_cast<double>(i));
        count++;
        total_sum += i;
      }
    });
  std::thread t3([this, &count, &total_sum]() {
      for (int i = 1; i < 3101; i++) {
        moving_average_statistics_->AddMeasurement(static_cast<double>(i));
        count++;
        total_sum += i;
      }
    });

  t1.join();
  t2.join();
  t3.join();

  const double control = static_cast<double>(total_sum.load()) / static_cast<double>(count.load());
  const double var = 1e-11;

  ASSERT_NEAR(moving_average_statistics_->Average(), control, var);
}

TEST(MovingAverageStatisticsTest, TestPrettyPrinting) {
  libstatistics_collector::moving_average_statistics::StatisticData data;
  ASSERT_EQ("avg=nan, min=nan, max=nan, std_dev=nan, count=0", StatisticsDataToString(data));

  MovingAverageStatistics stats;
  stats.AddMeasurement(1);
  ASSERT_EQ(
    "avg=1.000000, min=1.000000, max=1.000000, std_dev=0.000000, count=1",
    libstatistics_collector::moving_average_statistics::StatisticsDataToString(
      stats.GetStatistics()));
}
