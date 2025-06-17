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

#ifndef LIBSTATISTICS_COLLECTOR__TOPIC_STATISTICS_COLLECTOR__RECEIVED_MESSAGE_AGE_HPP_
#define LIBSTATISTICS_COLLECTOR__TOPIC_STATISTICS_COLLECTOR__RECEIVED_MESSAGE_AGE_HPP_

#include <chrono>
#include <string>
#include <sstream>
#include <type_traits>
#include <utility>

#include "constants.hpp"

#include "libstatistics_collector/topic_statistics_collector/topic_statistics_collector.hpp"

#include "builtin_interfaces/msg/time.hpp"
#include "rcl/time.h"
#include "rcutils/logging_macros.h"

namespace libstatistics_collector
{
namespace topic_statistics_collector
{

/**
 * False if the message does not have a header
 * @tparam M
 */
template<typename M, typename = void>
struct HasHeader : public std::false_type {};

/**
 * True if the message has a field named 'header' with a subfield named 'stamp' of
 * type builtin_interfaces::msg::Time
 * @tparam M
 */
template<typename M>
struct HasHeader<M, typename std::enable_if<std::is_same<builtin_interfaces::msg::Time,
  decltype(M().header.stamp)>::value>::type>: public std::true_type {};

/**
 * Return a boolean flag indicating the timestamp is not set
 * and zero if the message does not have a header
 */
template<typename M, typename Enable = void>
struct TimeStamp
{
  /**
   * @tparam M the message to extract the header from
   * @tparam Enable
   */
  static std::pair<bool, int64_t> value(const M &)
  {
    return std::make_pair(false, 0);
  }
};

/**
 * Returns a message header's timestamp, in nanoseconds, if the message's
 * header exists.
 */
template<typename M>
struct TimeStamp<M, typename std::enable_if<HasHeader<M>::value>::type>
{
  /**
   * @tparam M the message to extract the header timestamp from
   */
  static std::pair<bool, int64_t> value(const M & m)
  {
    const auto stamp = m.header.stamp;
    const auto nanos = RCL_S_TO_NS(static_cast<int64_t>(stamp.sec)) + stamp.nanosec;
    return std::make_pair(true, nanos);
  }
};

/**
 * Class used to measure the received messsage, tparam T, age from a ROS2 subscriber.
 *
 * @tparam T the message type to receive from the subscriber / listener
*/
template<typename T>
class ReceivedMessageAgeCollector : public TopicStatisticsCollector<T>
{
public:
  ReceivedMessageAgeCollector() = default;

  virtual ~ReceivedMessageAgeCollector() = default;

  /**
  * Handle a new incoming message. Calculate message age if a valid Header is present.
  *
  * @param received_message the message to calculate age of.
  * @param now_nanoseconds time the message was received in nanoseconds
  */
  void OnMessageReceived(
    const T & received_message,
    const rcl_time_point_value_t now_nanoseconds) override
  {
    const auto timestamp_from_header = TimeStamp<T>::value(received_message);

    if (timestamp_from_header.first) {
      // only compare if non-zero
      if (timestamp_from_header.second && now_nanoseconds) {
        const std::chrono::nanoseconds age_nanos{now_nanoseconds - timestamp_from_header.second};
        const auto age_millis = std::chrono::duration<double, std::milli>(age_nanos);

        collector::Collector::AcceptData(static_cast<double>(age_millis.count()));
      }  // else no valid time to compute age
    }
  }

  /**
   * Return message age metric name
   *
   * @return a string representing message age metric name
   */
  std::string GetMetricName() const override
  {
    return topic_statistics_constants::kMsgAgeStatName;
  }

  /**
   * Return messge age metric unit
   *
   * @return a string representing messager age metric unit
   */
  std::string GetMetricUnit() const override
  {
    return topic_statistics_constants::kMillisecondUnitName;
  }

protected:
  bool SetupStart() override
  {
    return true;
  }

  bool SetupStop() override
  {
    return true;
  }
};

}  // namespace topic_statistics_collector
}  // namespace libstatistics_collector

#endif  // LIBSTATISTICS_COLLECTOR__TOPIC_STATISTICS_COLLECTOR__RECEIVED_MESSAGE_AGE_HPP_
