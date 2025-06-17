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

#include <sstream>
#include <string>

#include "libstatistics_collector/moving_average_statistics/types.hpp"

namespace libstatistics_collector
{
namespace moving_average_statistics
{

std::string StatisticsDataToString(const StatisticData & results)
{
  std::stringstream ss;
  ss << "avg=" << std::to_string(results.average) << ", min=" << std::to_string(results.min) <<
    ", max=" << std::to_string(results.max) << ", std_dev=" << std::to_string(
    results.standard_deviation) << ", count=" << std::to_string(results.sample_count);
  return ss.str();
}

}  // namespace moving_average_statistics
}  // namespace libstatistics_collector
