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

#include "libstatistics_collector/collector/generate_statistics_message.hpp"

#include <string>
#include <utility>

#include "statistics_msgs/msg/statistic_data_type.hpp"

namespace libstatistics_collector
{
namespace collector
{

using statistics_msgs::msg::MetricsMessage;
using statistics_msgs::msg::StatisticDataPoint;
using statistics_msgs::msg::StatisticDataType;

MetricsMessage GenerateStatisticMessage(
  const std::string & node_name,
  const std::string & metric_name,
  const std::string & unit,
  const builtin_interfaces::msg::Time window_start,
  const builtin_interfaces::msg::Time window_stop,
  const libstatistics_collector::moving_average_statistics::StatisticData & data)
{
  MetricsMessage msg;

  msg.measurement_source_name = node_name;
  msg.metrics_source = metric_name;
  msg.unit = unit;
  msg.window_start = window_start;
  msg.window_stop = window_stop;

  msg.statistics.reserve(5);

  msg.statistics.emplace_back();
  msg.statistics.back().data_type = StatisticDataType::STATISTICS_DATA_TYPE_AVERAGE;
  msg.statistics.back().data = data.average;

  msg.statistics.emplace_back();
  msg.statistics.back().data_type = StatisticDataType::STATISTICS_DATA_TYPE_MAXIMUM;
  msg.statistics.back().data = data.max;

  msg.statistics.emplace_back();
  msg.statistics.back().data_type = StatisticDataType::STATISTICS_DATA_TYPE_MINIMUM;
  msg.statistics.back().data = data.min;

  msg.statistics.emplace_back();
  msg.statistics.back().data_type = StatisticDataType::STATISTICS_DATA_TYPE_SAMPLE_COUNT;
  msg.statistics.back().data = static_cast<double>(data.sample_count);

  msg.statistics.emplace_back();
  msg.statistics.back().data_type = StatisticDataType::STATISTICS_DATA_TYPE_STDDEV;
  msg.statistics.back().data = data.standard_deviation;

  return msg;
}

}  // namespace collector
}  // namespace libstatistics_collector
