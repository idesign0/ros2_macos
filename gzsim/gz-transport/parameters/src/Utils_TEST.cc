/*
 * Copyright (C) 2022 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include "Utils.hh"

#include <ignition/msgs/boolean.pb.h>
#include <ignition/msgs/stringmsg.pb.h>

#include "gtest/gtest.h"

using namespace ignition;
using namespace ignition::transport;
using namespace ignition::transport::parameters;

//////////////////////////////////////////////////
TEST(ParametersUtils, addIgnMsgsPrefix)
{
  EXPECT_EQ(addIgnMsgsPrefix("asd"), "ign_msgs.asd");
}

//////////////////////////////////////////////////
TEST(ParametersUtils, getIgnTypeFromAnyProto)
{
  google::protobuf::Any any;

  ignition::msgs::Boolean boolMsg;
  any.PackFrom(boolMsg);
  EXPECT_EQ(*getIgnTypeFromAnyProto(any), "Boolean");
  ignition::msgs::StringMsg strMsg;
  any.PackFrom(strMsg);
  EXPECT_EQ(*getIgnTypeFromAnyProto(any), "StringMsg");
}
