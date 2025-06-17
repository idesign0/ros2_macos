/*
 * Copyright (C) 2016 Open Source Robotics Foundation
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
#include <cstdlib>
#include <string>
#include <ignition/msgs.hh>

#include "gz/transport/Node.hh"
#include "gtest/gtest.h"
#include "gz/transport/test_config.h"

using namespace gz;

static std::string g_partition; // NOLINT(*)
static std::string g_topic = "/foo"; // NOLINT(*)

//////////////////////////////////////////////////
/// \brief This test spawns a service that doesn't accept input parameters. The
/// synchronous requester uses a wrong service's name. The test should verify
/// that the service call does not succeed and the elapsed time was close to
/// the timeout.
TEST(twoProcSrvCallWithoutInputSync1, SrvTwoProcs)
{
  std::string responser_path = testing::portablePathUnion(
     IGN_TRANSPORT_TEST_DIR,
     "INTEGRATION_twoProcsSrvCallWithoutInputReplier_aux");

  testing::forkHandlerType pi = testing::forkAndRun(responser_path.c_str(),
    g_partition.c_str());

  int64_t timeout = 500;
  msgs::Int32 rep;
  bool result;

  transport::Node node;

  // Make sure that the address of the service call provider is known.
  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  ASSERT_TRUE(node.Request(g_topic, static_cast<unsigned int>(timeout), rep,
    result));
  EXPECT_TRUE(result);

  auto t1 = std::chrono::steady_clock::now();
  EXPECT_FALSE(node.Request("unknown_service",
    static_cast<unsigned int>(timeout), rep, result));
  auto t2 = std::chrono::steady_clock::now();

  int64_t elapsed =
    std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

  // Check if the elapsed time was close to the timeout.
  auto diff = std::max(elapsed, timeout) - std::min(elapsed, timeout);
  EXPECT_LT(diff, 200);

  // Wait for the child process to return.
  testing::waitAndCleanupFork(pi);
}

//////////////////////////////////////////////////
int main(int argc, char **argv)
{
  // Get a random partition name.
  g_partition = testing::getRandomNumber();

  // Set the partition name for this process.
  setenv("IGN_PARTITION", g_partition.c_str(), 1);

  // Enable verbose mode.
  setenv("IGN_VERBOSE", "1", 1);

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
