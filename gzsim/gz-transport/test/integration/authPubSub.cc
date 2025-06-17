/*
 * Copyright (C) 2017 Open Source Robotics Foundation
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
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251)
#endif
#include <ignition/msgs/int32.pb.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include "gtest/gtest.h"
#include "gz/transport/Node.hh"
#include "gz/transport/TransportTypes.hh"
#include "gz/transport/test_config.h"

using namespace gz;

static std::string partition; // NOLINT(*)
static std::string g_topic = "/foo"; // NOLINT(*)

//////////////////////////////////////////////////
TEST(authPubSub, InvalidAuth)
{
  // Setup the username and password for this test
  setenv("IGN_TRANSPORT_USERNAME", "admin", 1);
  setenv("IGN_TRANSPORT_PASSWORD", "test", 1);

  transport::Node node;
  auto pub = node.Advertise<msgs::Int32>(g_topic);
  EXPECT_TRUE(pub);

  // No subscribers yet.
  EXPECT_FALSE(pub.HasConnections());

  std::string subscriberPath = testing::portablePathUnion(
     IGN_TRANSPORT_TEST_DIR,
     "INTEGRATION_authPubSubSubscriberInvalid_aux");

  // Start the subscriber in another process with incorrect credentials.
  testing::forkHandlerType pi = testing::forkAndRun(subscriberPath.c_str(),
    partition.c_str(), "bad", "invalid");

  msgs::Int32 msg;
  msg.set_data(1);

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  // \todo: Fix this when Node::Publisher::HasConnections takes into account
  // authentication.
  // We should still have no subscribers.
  // EXPECT_FALSE(pub.HasConnections());

  // Publish messages for a few seconds
  for (auto i = 0; i < 5; ++i)
  {
    EXPECT_TRUE(pub.Publish(msg));
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  // The other process should exit without receiving any of the messages.
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
