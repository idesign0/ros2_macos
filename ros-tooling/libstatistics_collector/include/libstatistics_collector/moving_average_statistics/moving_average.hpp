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

#ifndef LIBSTATISTICS_COLLECTOR__MOVING_AVERAGE_STATISTICS__MOVING_AVERAGE_HPP_
#define LIBSTATISTICS_COLLECTOR__MOVING_AVERAGE_STATISTICS__MOVING_AVERAGE_HPP_

#include <cmath>

#include <algorithm>
#include <limits>
#include <mutex>
#include <numeric>
#include <type_traits>

#include "types.hpp"

#include "libstatistics_collector/visibility_control.hpp"

#include "rcpputils/thread_safety_annotations.hpp"

namespace libstatistics_collector
{
namespace moving_average_statistics
{

/**
 *  A class for calculating moving average statistics. This operates in constant memory and constant time. Note:
 *  reset() must be called manually in order to start a new measurement window.
 *
 *  The statistics calculated are average, maximum, minimum, and standard deviation (population).
 *  All are calculated online without storing the observation data. Specifically, the average is a running sum
 *  and the variance is obtained by Welford's online algorithm
 *  (reference: https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford%27s_online_algorithm)
 *  for standard deviation.
 *
 *  When statistics are not available, e.g. no observations have been made, NaNs are returned.
 */
class MovingAverageStatistics
{
public:
  LIBSTATISTICS_COLLECTOR_PUBLIC
  MovingAverageStatistics() = default;

  LIBSTATISTICS_COLLECTOR_PUBLIC
  ~MovingAverageStatistics() = default;

  /**
   *  Returns the arithmetic mean of all data recorded. If no observations have been made, returns NaN.
   *
   *  @return The arithmetic mean of all data recorded, or NaN if the sample count is 0.
   */
  LIBSTATISTICS_COLLECTOR_PUBLIC
  double Average() const RCPPUTILS_TSA_REQUIRES(mutex_);

  /**
   *  Returns the maximum value recorded. If size of list is zero, returns NaN.
   *
   *  @return The maximum value recorded, or NaN if size of data is zero.
   */
  LIBSTATISTICS_COLLECTOR_PUBLIC
  double Max() const RCPPUTILS_TSA_REQUIRES(mutex_);

  /**
   *  Returns the minimum value recorded. If size of list is zero, returns NaN.
   *
   *  @return The minimum value recorded, or NaN if size of data is zero.
   */
  LIBSTATISTICS_COLLECTOR_PUBLIC
  double Min() const RCPPUTILS_TSA_REQUIRES(mutex_);

  /**
   *  Returns the standard deviation (population) of all data recorded. If size of list is zero, returns NaN.
   *
   *  Variance is obtained by Welford's online algorithm,
   *  see https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford%27s_online_algorithm
   *
   *  @return The standard deviation (population) of all data recorded, or NaN if size of data is zero.
   */
  LIBSTATISTICS_COLLECTOR_PUBLIC
  double StandardDeviation() const RCPPUTILS_TSA_REQUIRES(mutex_);

  /**
   *  Return a StatisticData object, containing average, minimum, maximum, standard deviation (population),
   *  and sample count.
   *  For the case of no observations, the average, min, max, and standard deviation are NaN.
   *
   *  @return StatisticData object, containing average, minimum, maximum, standard deviation (population),
   *  and sample count.
   */
  LIBSTATISTICS_COLLECTOR_PUBLIC
  StatisticData GetStatistics() const;

  /**
   *  Reset all calculated values. Equivalent to a new window for a moving average.
   */
  LIBSTATISTICS_COLLECTOR_PUBLIC
  void Reset();

  /**
   *  Observe a sample for the given window. The input item is used to calculate statistics.
   *  Note: any input values of NaN will be discarded and not added as a measurement.
   *
   *  @param item The item that was observed
   */
  LIBSTATISTICS_COLLECTOR_PUBLIC
  virtual void AddMeasurement(const double item);

  /**
   * Return the number of samples observed
   *
   * @return the number of samples observed
   */
  LIBSTATISTICS_COLLECTOR_PUBLIC
  uint64_t GetCount() const;

private:
  mutable std::mutex mutex_;
  double average_ RCPPUTILS_TSA_GUARDED_BY(mutex_) = 0;
  double min_ RCPPUTILS_TSA_GUARDED_BY(mutex_) = std::numeric_limits<double>::max();
  double max_ RCPPUTILS_TSA_GUARDED_BY(mutex_) = std::numeric_limits<double>::lowest();
  double sum_of_square_diff_from_mean_ RCPPUTILS_TSA_GUARDED_BY(mutex_) = 0;
  uint64_t count_ RCPPUTILS_TSA_GUARDED_BY(mutex_) = 0;
};

}  // namespace moving_average_statistics
}  // namespace libstatistics_collector

#endif  // LIBSTATISTICS_COLLECTOR__MOVING_AVERAGE_STATISTICS__MOVING_AVERAGE_HPP_
