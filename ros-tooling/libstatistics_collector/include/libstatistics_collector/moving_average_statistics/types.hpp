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

#ifndef LIBSTATISTICS_COLLECTOR__MOVING_AVERAGE_STATISTICS__TYPES_HPP_
#define LIBSTATISTICS_COLLECTOR__MOVING_AVERAGE_STATISTICS__TYPES_HPP_

#include <cmath>
#include <cstdint>
#include <sstream>
#include <string>

#include "libstatistics_collector/visibility_control.hpp"

namespace libstatistics_collector
{
namespace moving_average_statistics
{

/**
 *  A container for statistics data results for a set of recorded observations.
 */
struct LIBSTATISTICS_COLLECTOR_PUBLIC StatisticData
{
  /// average of the observations
  double average = std::nan("");
  /// min value of the observations
  double min = std::nan("");
  /// max value of the observations
  double max = std::nan("");
  /// standard_deviation of the observations
  double standard_deviation = std::nan("");
  /// number of samples of the observation
  uint64_t sample_count = 0;
};

/**
 * Function which pretty prints the contents of a StatisticData struct.
 *
 * @param results the StatisticData to pretty print
 * @return std::string formatted struct contents in an easily readable format, e.g.,
 * /"avg=1, min=2, max=3, std_dev=4, count=5/"
 */
LIBSTATISTICS_COLLECTOR_PUBLIC
std::string StatisticsDataToString(const StatisticData & results);

}  // namespace moving_average_statistics
}  // namespace libstatistics_collector

#endif  // LIBSTATISTICS_COLLECTOR__MOVING_AVERAGE_STATISTICS__TYPES_HPP_
