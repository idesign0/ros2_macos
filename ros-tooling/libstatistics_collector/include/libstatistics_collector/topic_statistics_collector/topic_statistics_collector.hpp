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


#ifndef LIBSTATISTICS_COLLECTOR__TOPIC_STATISTICS_COLLECTOR__TOPIC_STATISTICS_COLLECTOR_HPP_
#define LIBSTATISTICS_COLLECTOR__TOPIC_STATISTICS_COLLECTOR__TOPIC_STATISTICS_COLLECTOR_HPP_

#include <chrono>
#include <string>

#include "rcl/time.h"

#include "libstatistics_collector/collector/collector.hpp"

namespace libstatistics_collector
{
namespace topic_statistics_collector
{

/**
 * Interface to collect and perform measurements for ROS2 topic statistics.
 *
 * @tparam T the ROS2 message type to collect
 */
template<typename T>
class TopicStatisticsCollector : public collector::Collector
{
public:
  TopicStatisticsCollector() = default;

  virtual ~TopicStatisticsCollector() = default;

  /**
   * Handle receiving a single message of type T.
   *
   * @param received_message T the ROS2 message type to collect
   * @param now_nanoseconds nanoseconds the time the message was received. Any metrics using this time assumes the
   * following 1). the time provided is strictly monotonic 2). the time provided uses the same source
   * as time obtained from the message header.
   */
  virtual void OnMessageReceived(
    const T & received_message,
    const rcl_time_point_value_t now_nanoseconds) = 0;
};

}  // namespace topic_statistics_collector
}  // namespace libstatistics_collector

#endif  // LIBSTATISTICS_COLLECTOR__TOPIC_STATISTICS_COLLECTOR__TOPIC_STATISTICS_COLLECTOR_HPP_
