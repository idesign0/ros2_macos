// Copyright 2020 Open Source Robotics Foundation, Inc.
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

#ifndef RTTEST__MATH_UTILS_HPP_
#define RTTEST__MATH_UTILS_HPP_

#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

template<typename container>
double calculate_stddev(const container & vec)
{
  double n = vec.size();
  double mean = std::accumulate(
    vec.begin(),
    vec.end(), 0.0) / n;

  std::vector<double> diff(n);
  std::transform(
    vec.begin(), vec.end(), diff.begin(),
    [mean](auto x) -> double {return x - mean;});

  // first divide by sqrt(n)
  std::vector<double> div(n);
  std::transform(
    diff.begin(), diff.end(), div.begin(),
    [n](auto x) -> double {return x / std::sqrt(n);});

  // inner_product
  double sq_sum = std::inner_product(
    div.begin(), div.end(), div.begin(), 0.0);

  return std::sqrt(sq_sum);
}

#endif  // RTTEST__MATH_UTILS_HPP_
