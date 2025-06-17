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

#ifndef LIBSTATISTICS_COLLECTOR__COLLECTOR__GENERATE_STATISTICS_MESSAGE_HPP_
#define LIBSTATISTICS_COLLECTOR__COLLECTOR__GENERATE_STATISTICS_MESSAGE_HPP_

#include <string>

#include "builtin_interfaces/msg/time.hpp"
#include "statistics_msgs/msg/metrics_message.hpp"

#include "libstatistics_collector/visibility_control.hpp"
#include "libstatistics_collector/moving_average_statistics/types.hpp"

namespace libstatistics_collector
{
namespace collector
{

/**
 * Return a valid MetricsMessage ready to be published to a ROS topic
 *
 * @param node_name the name of the node that the data originates from
 * @param metric_name the name of the metric ("cpu_usage", "memory_usage", etc.)
 * @param unit name of the unit ("percentage", "mb", etc.)
 * @param window_start measurement window start time
 * @param window_stop measurement window end time
 * @param data statistics derived from the measurements made in the window
 * @return a MetricsMessage containing the statistics in the data parameter
 */
LIBSTATISTICS_COLLECTOR_PUBLIC
statistics_msgs::msg::MetricsMessage GenerateStatisticMessage(
  const std::string & node_name,
  const std::string & metric_name,
  const std::string & unit,
  const builtin_interfaces::msg::Time window_start,
  const builtin_interfaces::msg::Time window_stop,
  const libstatistics_collector::moving_average_statistics::StatisticData & data
);

}  // namespace collector
}  // namespace libstatistics_collector

#endif  // LIBSTATISTICS_COLLECTOR__COLLECTOR__GENERATE_STATISTICS_MESSAGE_HPP_
