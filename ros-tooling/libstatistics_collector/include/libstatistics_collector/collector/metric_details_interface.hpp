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

#ifndef LIBSTATISTICS_COLLECTOR__COLLECTOR__METRIC_DETAILS_INTERFACE_HPP_
#define LIBSTATISTICS_COLLECTOR__COLLECTOR__METRIC_DETAILS_INTERFACE_HPP_

#include <string>

#include "libstatistics_collector/visibility_control.hpp"

namespace libstatistics_collector
{
namespace collector
{

/**
 * Interface to represent a single metric's name and unit,
 * which are used for metric message generation and publication.
 */
class LIBSTATISTICS_COLLECTOR_PUBLIC MetricDetailsInterface
{
public:
  virtual ~MetricDetailsInterface() = default;

  /**
   * Return a single metric's name.
   *
   * @return a string representing the metric name
   */
  virtual std::string GetMetricName() const = 0;

  /**
   * Return a single metric's measurement unit.
   *
   * @return a string representing the metric unit
   */
  virtual std::string GetMetricUnit() const = 0;
};

}  // namespace collector
}  // namespace libstatistics_collector

#endif  // LIBSTATISTICS_COLLECTOR__COLLECTOR__METRIC_DETAILS_INTERFACE_HPP_
