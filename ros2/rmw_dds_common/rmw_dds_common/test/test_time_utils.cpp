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

#include <gtest/gtest.h>
#include <climits>

#include "rmw_dds_common/time_utils.hpp"

static bool
operator==(rmw_time_t t1, rmw_time_t t2)
{
  return t1.sec == t2.sec && t1.nsec == t2.nsec;
}

TEST(test_time_utils, test_unmodified_zeros)
{
  const rmw_time_t zeros {0, 0};
  auto t1 = rmw_dds_common::clamp_rmw_time_to_dds_time(zeros);
  EXPECT_TRUE(t1 == zeros);
}

TEST(test_time_utils, test_unmodified_max)
{
  // Maximum time length supported by DDS is { INT_MAX, 10^9 - 1 }
  const rmw_time_t max_dds_time {0x7FFFFFFF, 999999999ULL};
  auto t2 = rmw_dds_common::clamp_rmw_time_to_dds_time(max_dds_time);
  EXPECT_TRUE(t2 == max_dds_time);
}

TEST(test_time_utils, test_seconds_overflow)
{
  // Maximum time length supported by DDS is { INT_MAX, 10^9 - 1 }
  const rmw_time_t slightly_too_large {0x80000000, 0};
  auto t3_sec = rmw_dds_common::clamp_rmw_time_to_dds_time(slightly_too_large);
  const rmw_time_t result3 {0x7FFFFFFF, 999999999ULL};
  EXPECT_TRUE(t3_sec == result3);
  // Set the whole length via the nanosec field.
  const rmw_time_t slightly_too_large_ns {0, 0x80000000 * 1000000000ULL};
  auto t3_ns = rmw_dds_common::clamp_rmw_time_to_dds_time(slightly_too_large_ns);
  EXPECT_TRUE(t3_ns == result3);

  const rmw_time_t slightly_too_large_both_1 {0x7FFFFFFF, 1000000000ULL};
  auto t3_both_1 = rmw_dds_common::clamp_rmw_time_to_dds_time(slightly_too_large_both_1);
  EXPECT_TRUE(t3_both_1 == result3);

  const rmw_time_t slightly_too_large_both_2 {0x80000000, 9999999998ULL};
  auto t3_both_2 = rmw_dds_common::clamp_rmw_time_to_dds_time(slightly_too_large_both_2);
  EXPECT_TRUE(t3_both_2 == result3);
}

TEST(test_time_utils, test_saturation)
{
  const rmw_time_t max_64 {LLONG_MAX, ULLONG_MAX};
  // Maximum time length supported by DDS is { INT_MAX, 10^9 - 1 }
  const rmw_time_t max_dds_time {0x7FFFFFFF, 999999999ULL};
  auto t4 = rmw_dds_common::clamp_rmw_time_to_dds_time(max_64);
  EXPECT_TRUE(t4 == max_dds_time);
}

TEST(test_time_utils, test_normalize)
{
  // The DDS Spec requires that the nanoseconds fields be < 1s.
  const rmw_time_t already_normalized {1, 999999999ULL};
  auto already_normalized_res = rmw_dds_common::clamp_rmw_time_to_dds_time(already_normalized);
  EXPECT_TRUE(already_normalized_res == already_normalized);

  const rmw_time_t unnormalized_min {0, 1000000000ULL};
  auto normalized_min = rmw_dds_common::clamp_rmw_time_to_dds_time(unnormalized_min);
  const rmw_time_t normalized_min_expected {1, 0};
  EXPECT_TRUE(normalized_min == normalized_min_expected);

  const rmw_time_t unnormalized_mid {0, 0x5FFFFFFF * 1000000000ULL + 999999999ULL};
  auto normalized_mid = rmw_dds_common::clamp_rmw_time_to_dds_time(unnormalized_mid);
  const rmw_time_t normalized_mid_expected {0x5FFFFFFF, 999999999ULL};
  EXPECT_TRUE(normalized_mid == normalized_mid_expected);

  const rmw_time_t unnormalized_max {0, 0x7FFFFFFF * 1000000000ULL + 999999999ULL};
  auto normalized_max = rmw_dds_common::clamp_rmw_time_to_dds_time(unnormalized_max);
  const rmw_time_t normalized_max_expected {0x7FFFFFFF, 999999999ULL};
  EXPECT_TRUE(normalized_max == normalized_max_expected);

  const rmw_time_t unnormalized_max_2 {0x7FFFFFFE, 1999999999ULL};
  auto normalized_max_2 = rmw_dds_common::clamp_rmw_time_to_dds_time(unnormalized_max_2);
  EXPECT_TRUE(normalized_max_2 == normalized_max_expected);
}
