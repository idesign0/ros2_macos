// Copyright 2015 Open Source Robotics Foundation, Inc.
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

#include <vector>

#include "gtest/gtest.h"
#include "rttest/math_utils.hpp"

TEST(MathUtils, calculate_stddev) {
  std::vector<uint64_t> v_int = {1, 2, 3, 4};
  EXPECT_NEAR(1.118, calculate_stddev(v_int), 0.001);

  std::vector<double> v_double = {1.1, 2.2, 3.3, 4.9};
  EXPECT_NEAR(1.404, calculate_stddev(v_double), 0.001);

  // #95
  // example_loop shows
  // Standard deviation: 2594.930828
  // but numpy.std says 36581.054213911055.
  std::vector<uint64_t> v_failed = {
    58184, 18661, 83459, 80252, 80355, 16540, 80091, 79467,
    80551, 19769, 95537, 46263, 37242, 17908, 17411, 15006,
    14815, 14725, 15595, 15115, 16292, 22865, 36376, 36681,
    36564, 22203, 42201, 36765, 58167, 27846, 19891, 17952,
    18629, 18075, 20265, 19984, 18502, 18918, 18027, 44307,
    43990, 28412, 45483, 45293, 43955, 54190, 103671, 100835,
    100665, 25228, 101552, 103844, 102738, 25395, 97951, 103043,
    100565, 52475, 100227, 102139, 100906, 25417, 99640, 98187,
    101581, 25608, 100494, 107581, 103422, 25416, 102073, 100450,
    101185, 25779, 102311, 99956, 101441, 25473, 102454, 100402,
    100403, 25335, 103525, 100091, 104371, 25156, 100897, 103183,
    99581, 25203, 100824, 99812, 106011, 29428, 103198, 102470,
    99901, 25536, 100327, 100932};
  EXPECT_NEAR(36581.05, calculate_stddev(v_failed), 0.01);
}
