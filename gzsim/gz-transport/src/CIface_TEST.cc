/*
 * Copyright (C) 2019 Open Source Robotics Foundation
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
#include <gz/msgs/stringmsg.pb.h>

#include "gtest/gtest.h"
#include "gz/transport/CIface.h"
#include "gz/transport/test_config.h"

static int count;

//////////////////////////////////////////////////
/// \brief Function called each time a topic update is received.
void cb(const char *_data, size_t _size, const char *_msgType, void *_userData)
{
  int *userData = static_cast<int*>(_userData);

  ASSERT_NE(nullptr, userData);
  EXPECT_EQ(42, *userData);

  gz::msgs::StringMsg msg;
  msg.ParseFromArray(_data, _size);
  EXPECT_STREQ("ignition.msgs.StringMsg", _msgType);
  EXPECT_EQ(msg.data(), "HELLO");
  ++count;
}

//////////////////////////////////////////////////
/// \brief Function called each time a topic update is received.
void cbNonConst(char *_data, size_t _size, char *_msgType, void *_userData)
{
  int *userData = static_cast<int*>(_userData);

  ASSERT_NE(nullptr, userData);
  EXPECT_EQ(42, *userData);

  gz::msgs::StringMsg msg;
  msg.ParseFromArray(_data, _size);
  EXPECT_STREQ("ignition.msgs.StringMsg", _msgType);
  EXPECT_EQ(msg.data(), "HELLO");
  ++count;
}

//////////////////////////////////////////////////
TEST(CIfaceTest, PubSub)
{
  count = 0;
  IgnTransportNode *node = ignTransportNodeCreate(nullptr);
  EXPECT_NE(nullptr, node);

  const char *topic = "/foo";

  int userData = 42;

  // Subscribe
  ASSERT_EQ(0, ignTransportSubscribe(node, topic, cb, &userData));

  // Subscribe
  ASSERT_EQ(0, ignTransportSubscribeNonConst(node,
        const_cast<char *>(topic), cbNonConst, &userData));


  // Prepare the message.
  gz::msgs::StringMsg msg;
  msg.set_data("HELLO");

  // Get the size of the serialized message
#if GOOGLE_PROTOBUF_VERSION >= 3004000
  int size = msg.ByteSizeLong();
#else
  int size = msg.ByteSize();
#endif

  // Allocate space for the serialized message
  void *buffer = malloc(size);

  ASSERT_NE(nullptr, buffer);

  // Serialize the message.
  msg.SerializeToArray(buffer, size);

  EXPECT_EQ(0,
    ignTransportPublish(node, topic, buffer, msg.GetTypeName().data()));
  EXPECT_EQ(2, count);

  count = 0;

  // Unsubscribe
  ASSERT_EQ(0, ignTransportUnsubscribe(node, topic));
  EXPECT_EQ(0,
    ignTransportPublish(node, topic, buffer, msg.GetTypeName().data()));
  EXPECT_EQ(0, count);

  free(buffer);
  ignTransportNodeDestroy(&node);
  EXPECT_EQ(nullptr, node);
}

//////////////////////////////////////////////////
TEST(CIfaceTest, PubSubPartitions)
{
  count = 0;
  IgnTransportNode *node = ignTransportNodeCreate(nullptr);
  IgnTransportNode *nodeBar = ignTransportNodeCreate("bar");
  EXPECT_NE(nullptr, node);

  const char *topic = "/foo";

  int userData = 42;

  // Subscribe on "bar" topic
  ASSERT_EQ(0, ignTransportSubscribe(nodeBar, topic, cb, &userData));

  // Subscribe
  ASSERT_EQ(0, ignTransportSubscribeNonConst(node,
        const_cast<char *>(topic), cbNonConst, &userData));

  // Prepare the message.
  gz::msgs::StringMsg msg;
  msg.set_data("HELLO");

  // Get the size of the serialized message
#if GOOGLE_PROTOBUF_VERSION >= 3004000
  int size = msg.ByteSizeLong();
#else
  int size = msg.ByteSize();
#endif

  // Allocate space for the serialized message
  void *buffer = malloc(size);

  ASSERT_NE(nullptr, buffer);

  // Serialize the message.
  msg.SerializeToArray(buffer, size);

  // Publish on "bar" partition
  EXPECT_EQ(0,
    ignTransportPublish(nodeBar, topic, buffer, msg.GetTypeName().data()));
  EXPECT_EQ(1, count);

  // Publish on default partition
  EXPECT_EQ(0,
    ignTransportPublish(nodeBar, topic, buffer, msg.GetTypeName().data()));
  EXPECT_EQ(2, count);

  count = 0;

  // Unsubscribe
  ASSERT_EQ(0, ignTransportUnsubscribe(nodeBar, topic));
  EXPECT_EQ(0,
    ignTransportPublish(nodeBar, topic, buffer, msg.GetTypeName().data()));
  EXPECT_EQ(0, count);

  free(buffer);
  ignTransportNodeDestroy(&node);
  EXPECT_EQ(nullptr, node);
  ignTransportNodeDestroy(&nodeBar);
  EXPECT_EQ(nullptr, nodeBar);
}

//////////////////////////////////////////////////
int main(int argc, char **argv)
{
  // Get a random partition name.
  std::string partition = testing::getRandomNumber();

  // Set the partition name for this process.
  setenv("IGN_PARTITION", partition.c_str(), 1);

  // Enable verbose mode.
  // setenv("IGN_VERBOSE", "1", 1);

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
