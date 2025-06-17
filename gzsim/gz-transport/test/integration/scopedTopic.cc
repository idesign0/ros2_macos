/*
 * Copyright (C) 2014 Open Source Robotics Foundation
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

#include <chrono>
#include <string>
#include <ignition/msgs.hh>

#include "gz/transport/AdvertiseOptions.hh"
#include "gz/transport/Node.hh"
#include "gtest/gtest.h"
#include "gz/transport/test_config.h"

using namespace gz;

static std::string partition; // NOLINT(*)
static std::string g_topic = "/foo"; // NOLINT(*)
static int data = 5;

//////////////////////////////////////////////////
/// \brief Two different nodes, each one running in a different process. The
/// publisher advertises the topic as "process". This test checks that the topic
/// is not seen by the other node running in a different process.
TEST(ScopedTopicTest, ProcessTest)
{
  std::string subscriber_path = testing::portablePathUnion(
     IGN_TRANSPORT_TEST_DIR,
     "INTEGRATION_scopedTopicSubscriber_aux");

  testing::forkHandlerType pi = testing::forkAndRun(subscriber_path.c_str(),
    partition.c_str());

  msgs::Int32 msg;
  msg.set_data(data);

  transport::Node node;
  transport::AdvertiseMessageOptions opts;
  opts.SetScope(transport::Scope_t::PROCESS);

  auto pub = node.Advertise<msgs::Int32>(g_topic, opts);
  EXPECT_TRUE(pub);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  EXPECT_TRUE(pub.Publish(msg));
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  EXPECT_TRUE(pub.Publish(msg));

  testing::waitAndCleanupFork(pi);
}

//////////////////////////////////////////////////
int main(int argc, char **argv)
{
  // Get a random partition name.
  partition = testing::getRandomNumber();

  // Set the partition name for this process.
  setenv("IGN_PARTITION", partition.c_str(), 1);

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
