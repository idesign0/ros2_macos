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

#ifndef LIBSTATISTICS_COLLECTOR__COLLECTOR__COLLECTOR_HPP_
#define LIBSTATISTICS_COLLECTOR__COLLECTOR__COLLECTOR_HPP_

#include <mutex>
#include <string>

#include "libstatistics_collector/visibility_control.hpp"
#include "libstatistics_collector/moving_average_statistics/moving_average.hpp"
#include "libstatistics_collector/moving_average_statistics/types.hpp"

#include "metric_details_interface.hpp"

#include "rcpputils/thread_safety_annotations.hpp"

namespace libstatistics_collector
{
namespace collector
{

/**
 * Simple class in order to collect observed data and generate statistics for the given observations.
 */
class Collector : public MetricDetailsInterface
{
public:
  LIBSTATISTICS_COLLECTOR_PUBLIC
  Collector() = default;

  LIBSTATISTICS_COLLECTOR_PUBLIC
  virtual ~Collector() = default;

  /**
   * Add an observed measurement. This aggregates the measurement and calculates statistics
   * via the moving_average class.
   *
   * @param measurement the measurement observed
   */
  LIBSTATISTICS_COLLECTOR_PUBLIC
  virtual void AcceptData(const double measurement);

  /**
   * Return the statistics for all of the observed data.
   *
   * @return the StatisticData for all the observed measurements
   */
  LIBSTATISTICS_COLLECTOR_PUBLIC
  virtual moving_average_statistics::StatisticData GetStatisticsResults() const;

  /**
   * Clear / reset all current measurements.
   */
  LIBSTATISTICS_COLLECTOR_PUBLIC
  virtual void ClearCurrentMeasurements();

  /**
   * Return true is start has been called, false otherwise.
   *
   * @return the started state of this collector
   */
  LIBSTATISTICS_COLLECTOR_PUBLIC
  bool IsStarted() const;

  /**
   * Return a pretty printed status representation of this class
   *
   * @return a string detailing the current status
   */
  LIBSTATISTICS_COLLECTOR_PUBLIC
  virtual std::string GetStatusString() const;

  // TODO(dabonnie): uptime (once start has been called)

  /**
   * Start collecting data. Meant to be called after construction. Note: this locks the recursive mutex class
   * member 'mutex'. This method is public in order for the caller to manually manage starting and
   * stopping this collector.
   *
   * @return true if started, false if an error occurred
   */
  LIBSTATISTICS_COLLECTOR_PUBLIC
  virtual bool Start();

  /**
   * Stop collecting data. Meant to be a teardown method (before destruction, but should place the
   * class in a restartable state, i.e., start can be called to be able to resume collection. This method
   * is public in order for the caller to manually manage starting and stopping this collector.
   *
   * This calls ClearCurrentMeasurements.
   *
   * @return true if stopped, false if an error occurred
   */
  LIBSTATISTICS_COLLECTOR_PUBLIC
  virtual bool Stop();

private:
  /**
   * Override in order to perform necessary starting steps.
   *
   * @return true if setup was successful, false otherwise.
   */
  virtual bool SetupStart() RCPPUTILS_TSA_REQUIRES(mutex_) = 0;

  /**
   * Override in order to perform necessary teardown.
   *
   * @return true if teardown was successful, false otherwise.
   */
  virtual bool SetupStop() RCPPUTILS_TSA_REQUIRES(mutex_) = 0;

  mutable std::mutex mutex_;

  moving_average_statistics::MovingAverageStatistics collected_data_;

  bool started_ RCPPUTILS_TSA_GUARDED_BY(mutex_) = false;
};

}  // namespace collector
}  // namespace libstatistics_collector

#endif  // LIBSTATISTICS_COLLECTOR__COLLECTOR__COLLECTOR_HPP_
