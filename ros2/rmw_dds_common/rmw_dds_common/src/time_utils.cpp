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

#include <climits>

#include "rmw/types.h"
#include "rcutils/logging_macros.h"

#include "rmw_dds_common/time_utils.hpp"

rmw_time_t
rmw_dds_common::clamp_rmw_time_to_dds_time(const rmw_time_t & time)
{
  rmw_time_t t = time;

  // Normalize rmw_time_t::nsec to be < 1s, so that it may be safely converted
  // to a DDS Duration or Time (see DDS v1.4 section 2.3.2).
  // If the total length in seconds cannot be represented by DDS (which is
  // limited to INT_MAX seconds + (10^9 - 1) nanoseconds) we must truncate
  // the seconds component at INT_MAX, while also normalizing nanosec to < 1s.
  constexpr uint64_t sec_to_ns = 1000000000ULL;
  uint64_t ns_sec_adjust = t.nsec / sec_to_ns;
  bool overflow_nsec = false;
  bool overflow_sec = false;

  if (ns_sec_adjust > INT_MAX) {
    ns_sec_adjust = INT_MAX;
    overflow_nsec = true;
  }

  if (t.sec > INT_MAX - ns_sec_adjust) {
    t.sec = INT_MAX;
    overflow_sec = true;
  } else {
    t.sec += ns_sec_adjust;
  }

  if (overflow_nsec || overflow_sec) {
    // The nsec component must be "saturated" if we are overflowing INT_MAX
    t.nsec = sec_to_ns - 1;
    RCUTILS_LOG_DEBUG_NAMED(
      "rmw_dds_common",
      "rmw_time_t length cannot be represented by DDS, truncated at "
      "INT_MAX seconds + (10^9 - 1) nanoseconds");
  } else {
    t.nsec = t.nsec - ns_sec_adjust * sec_to_ns;
  }

  return t;
}
