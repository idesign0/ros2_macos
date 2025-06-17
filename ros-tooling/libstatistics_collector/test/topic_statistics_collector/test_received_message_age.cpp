// Copyright 2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

#include <chrono>
#include <string>

#include "libstatistics_collector/msg/dummy_message.hpp"
#include "libstatistics_collector/msg/dummy_custom_header_message.hpp"
#include "libstatistics_collector/topic_statistics_collector/constants.hpp"
#include "libstatistics_collector/topic_statistics_collector/received_message_age.hpp"

#include "rcl/time.h"

namespace
{
using DummyMessage = libstatistics_collector::msg::DummyMessage;
using ReceivedDummyMessageAgeCollector = libstatistics_collector::
  topic_statistics_collector::ReceivedMessageAgeCollector<DummyMessage>;
using ReceivedIntMessageAgeCollector = libstatistics_collector::
  topic_statistics_collector::ReceivedMessageAgeCollector<int>;
using DummyCustomHeaderMessage = libstatistics_collector::msg::DummyCustomHeaderMessage;
using ReceivedDummyCustomHeaderMessageAgeCollector = libstatistics_collector::
  topic_statistics_collector::ReceivedMessageAgeCollector<DummyCustomHeaderMessage>;

constexpr const std::chrono::seconds kDefaultDurationSeconds{1};
constexpr const double kExpectedAverageMilliseconds{2000.0};
constexpr const double kExpectedMinMilliseconds{1000.0};
constexpr const double kExpectedMaxMilliseconds{3000.0};
constexpr const double kExpectedStandardDeviation{816.49658092772597};
constexpr const int kDefaultTimesToTest{10};
constexpr const int64_t kDefaultTimeMessageReceived{1000};
constexpr const rcl_time_point_value_t kStartTime{123456789};
constexpr const int kRandomIntMessage{7};
}  // namespace


TEST(ReceivedMessageAgeTest, TestOnlyMessagesWithHeaderGetSampled) {
  ReceivedIntMessageAgeCollector int_msg_collector{};

  libstatistics_collector::moving_average_statistics::StatisticData stats;

  for (int i = 0; i < kDefaultTimesToTest; ++i) {
    int_msg_collector.OnMessageReceived(kRandomIntMessage, kDefaultTimeMessageReceived);
    stats = int_msg_collector.GetStatisticsResults();
    EXPECT_EQ(0, stats.sample_count) << "Expect 0 samples to be collected";
  }

  ReceivedDummyMessageAgeCollector dummy_msg_collector{};
  auto msg = DummyMessage{};
  msg.header.stamp.sec = kDefaultTimeMessageReceived;
  for (int i = 0; i < kDefaultTimesToTest; ++i) {
    dummy_msg_collector.OnMessageReceived(msg, kDefaultTimeMessageReceived);
    stats = dummy_msg_collector.GetStatisticsResults();
    EXPECT_EQ(i + 1, stats.sample_count) << "Expect " << i + 1 << " samples to be collected";
  }

  ReceivedDummyCustomHeaderMessageAgeCollector dummy_custom_header_msg_collector{};
  auto msg_custom_header = DummyCustomHeaderMessage{};
  for (int i = 0; i < kDefaultTimesToTest; ++i) {
    dummy_custom_header_msg_collector.OnMessageReceived(
      msg_custom_header,
      kDefaultTimeMessageReceived);
    stats = dummy_custom_header_msg_collector.GetStatisticsResults();
    EXPECT_EQ(0, stats.sample_count) << "Expect 0 samples to be collected";
  }
}

TEST(ReceivedMessageAgeTest, TestMeasurementOnlyMadeForInitializedHeaderValue) {
  ReceivedDummyMessageAgeCollector dummy_msg_collector{};

  // Don't initialize `header.stamp`
  auto msg = DummyMessage{};

  dummy_msg_collector.OnMessageReceived(msg, kDefaultTimeMessageReceived);
  auto stats = dummy_msg_collector.GetStatisticsResults();
  EXPECT_EQ(0, stats.sample_count) << "Expect 0 samples to be collected";

  // Set `header.stamp` to 0
  msg.header.stamp.sec = 0;
  dummy_msg_collector.OnMessageReceived(msg, kDefaultTimeMessageReceived);
  stats = dummy_msg_collector.GetStatisticsResults();
  EXPECT_EQ(0, stats.sample_count) << "Expect 0 samples to be collected";

  // Set `header.stamp` to non-zero value
  msg.header.stamp.sec = kDefaultTimeMessageReceived;
  dummy_msg_collector.OnMessageReceived(msg, kDefaultTimeMessageReceived);
  stats = dummy_msg_collector.GetStatisticsResults();
  EXPECT_EQ(1, stats.sample_count) << "Expect 1 sample to be collected";
}

TEST(ReceivedMessageAgeTest, TestAgeMeasurement) {
  ReceivedDummyMessageAgeCollector test_collector{};

  EXPECT_FALSE(test_collector.IsStarted()) << "Expect to be not started after constructed";

  EXPECT_TRUE(test_collector.Start()) << "Expect Start() to be successful";
  EXPECT_TRUE(test_collector.IsStarted()) << "Expect to be started";

  rcl_time_point_value_t fake_now_nanos_{kStartTime};

  auto msg = DummyMessage{};
  msg.header.stamp.sec = static_cast<std::int32_t>(RCL_NS_TO_S(fake_now_nanos_));
  msg.header.stamp.nanosec = static_cast<std::uint32_t>(fake_now_nanos_ % (1000 * 1000 * 1000));

  fake_now_nanos_ +=
    std::chrono::duration_cast<std::chrono::nanoseconds>(kDefaultDurationSeconds).count();

  test_collector.OnMessageReceived(msg, fake_now_nanos_);
  auto stats = test_collector.GetStatisticsResults();
  EXPECT_EQ(1, stats.sample_count);

  fake_now_nanos_ +=
    std::chrono::duration_cast<std::chrono::nanoseconds>(kDefaultDurationSeconds).count();

  test_collector.OnMessageReceived(msg, fake_now_nanos_);
  stats = test_collector.GetStatisticsResults();
  EXPECT_EQ(2, stats.sample_count);

  fake_now_nanos_ +=
    std::chrono::duration_cast<std::chrono::nanoseconds>(kDefaultDurationSeconds).count();

  test_collector.OnMessageReceived(msg, fake_now_nanos_);
  stats = test_collector.GetStatisticsResults();
  EXPECT_EQ(3, stats.sample_count);

  EXPECT_EQ(kExpectedAverageMilliseconds, stats.average);
  EXPECT_EQ(kExpectedMinMilliseconds, stats.min);
  EXPECT_EQ(kExpectedMaxMilliseconds, stats.max);
  EXPECT_DOUBLE_EQ(kExpectedStandardDeviation, stats.standard_deviation);
}

TEST(ReceivedMessageAgeTest, TestGetStatNameAndUnit) {
  ReceivedDummyMessageAgeCollector test_collector{};

  EXPECT_FALSE(test_collector.GetMetricName().empty());
  EXPECT_FALSE(test_collector.GetMetricUnit().empty());
}
