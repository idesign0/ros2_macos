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

#include "rmw_dds_common/gid_utils.hpp"

using rmw_dds_common::Compare_rmw_gid_t;
using rmw_dds_common::operator==;
using rmw_dds_common::operator<<;

TEST(test_gid_utils, test_gid_operators)
{
  rmw_gid_t gid_1 = {0, {0}};
  rmw_gid_t gid_2 = {0, {0}};

  struct Compare_rmw_gid_t compare;
  EXPECT_FALSE(compare(gid_1, gid_2));

  EXPECT_TRUE(gid_1 == gid_2);

  rmw_gid_t gid_3 = {"foo", {0}};

  std::ostringstream stream;
  stream << gid_3;
  ASSERT_STREQ(stream.str().c_str(), "0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0");
}
