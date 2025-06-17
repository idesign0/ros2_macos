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


#include <mutex>
#include <sstream>
#include <string>

#include "libstatistics_collector/collector/collector.hpp"
#include "libstatistics_collector/moving_average_statistics/moving_average.hpp"
#include "libstatistics_collector/moving_average_statistics/types.hpp"

namespace libstatistics_collector
{
namespace collector
{

bool Collector::Start()
{
  std::unique_lock<std::mutex> ulock{mutex_};
  if (started_) {
    return false;
  }
  started_ = true;
  return SetupStart();
}

bool Collector::Stop()
{
  bool ret = false;
  {
    std::unique_lock<std::mutex> ulock{mutex_};
    if (!started_) {
      return false;
    }
    started_ = false;

    ret = SetupStop();
  }
  ClearCurrentMeasurements();
  return ret;
}

void Collector::AcceptData(const double measurement)
{
  collected_data_.AddMeasurement(measurement);
}

moving_average_statistics::StatisticData Collector::GetStatisticsResults() const
{
  return collected_data_.GetStatistics();
}

void Collector::ClearCurrentMeasurements()
{
  collected_data_.Reset();
}

bool Collector::IsStarted() const
{
  std::unique_lock<std::mutex> ulock{mutex_};
  return started_;
}

std::string Collector::GetStatusString() const
{
  std::stringstream ss;
  ss << "started=" << (IsStarted() ? "true" : "false") <<
    ", " << StatisticsDataToString(GetStatisticsResults());
  return ss.str();
}

}  // namespace collector
}  // namespace libstatistics_collector
