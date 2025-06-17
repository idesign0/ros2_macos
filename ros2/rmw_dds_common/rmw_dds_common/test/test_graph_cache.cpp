// Copyright 2019 Open Source Robotics Foundation, Inc.
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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string.h>

#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "osrf_testing_tools_cpp/scope_exit.hpp"

#include "rcutils/testing/fault_injection.h"
#include "rmw/qos_profiles.h"
#include "rmw/topic_endpoint_info.h"
#include "rmw/topic_endpoint_info_array.h"
#include "./allocator_testing_utils.h"

#include "rmw_dds_common/gid_utils.hpp"
#include "rmw_dds_common/graph_cache.hpp"

using rmw_dds_common::GraphCache;
using rmw_dds_common::operator==;

struct NameAndNamespace
{
  std::string namespace_;
  std::string name;
};

MATCHER_P2(IsNameAndNamespace, namespace_, name, "")
{
  return (arg.namespace_ == namespace_) && (arg.name == name);
}

void
check_names_and_namespace(
  const rcutils_string_array_t & names,
  const rcutils_string_array_t & namespaces,
  const std::vector<NameAndNamespace> & expected)
{
  ASSERT_EQ(names.size, namespaces.size);
  ASSERT_EQ(expected.size(), names.size);
  for (size_t i = 0; i < expected.size(); i++) {
    ASSERT_THAT(
      expected,
      testing::Contains(IsNameAndNamespace(namespaces.data[i], names.data[i])));
  }
}

struct NameAndTypes
{
  std::string name;
  std::vector<std::string> types;
};

void check_names_and_types(
  const rmw_names_and_types_t & names_and_types,
  const std::vector<NameAndTypes> & expected)
{
  ASSERT_EQ(names_and_types.names.size, expected.size());

  for (size_t i = 0; i < expected.size(); i++) {
    const auto & item = expected[i];
    EXPECT_EQ(item.name, names_and_types.names.data[i]);
    const auto & expected_types = item.types;
    const auto & types = names_and_types.types[i];
    ASSERT_EQ(expected_types.size(), types.size);
    for (size_t j = 0; j < expected_types.size(); j++) {
      EXPECT_EQ(expected_types[j], types.data[j]);
    }
  }
}

std::string
identity_demangle(const std::string & name)
{
  return name;
}

using DemangleFunctionT = GraphCache::DemangleFunctionT;

void
check_results(
  const GraphCache & graph_cache,
  const std::vector<NameAndNamespace> & nodes_names_and_namespaces = {},
  const std::vector<NameAndTypes> & topics_names_and_types = {},
  DemangleFunctionT demangle_topic = identity_demangle,
  DemangleFunctionT demangle_type = identity_demangle)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  {
    rcutils_string_array_t names = rcutils_get_zero_initialized_string_array();
    rcutils_string_array_t namespaces = rcutils_get_zero_initialized_string_array();
    rcutils_string_array_t enclaves = rcutils_get_zero_initialized_string_array();
    graph_cache.get_node_names(&names, &namespaces, &enclaves, &allocator);
    check_names_and_namespace(names, namespaces, nodes_names_and_namespaces);
    EXPECT_EQ(RCUTILS_RET_OK, rcutils_string_array_fini(&enclaves));
    EXPECT_EQ(RCUTILS_RET_OK, rcutils_string_array_fini(&namespaces));
    EXPECT_EQ(RCUTILS_RET_OK, rcutils_string_array_fini(&names));
  }

  EXPECT_EQ(nodes_names_and_namespaces.size(), graph_cache.get_number_of_nodes());

  {
    rmw_names_and_types_t names_and_types = rmw_get_zero_initialized_names_and_types();
    graph_cache.get_names_and_types(
      demangle_topic,
      demangle_type,
      &allocator,
      &names_and_types);
    check_names_and_types(names_and_types, topics_names_and_types);
    EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_fini(&names_and_types));
  }
}

void check_results_by_node(
  const GraphCache & graph_cache,
  const std::string & node_namespace,
  const std::string & node_name,
  const std::vector<NameAndTypes> & readers_names_and_types = {},
  const std::vector<NameAndTypes> & writers_names_and_types = {},
  DemangleFunctionT demangle_topic = identity_demangle,
  DemangleFunctionT demangle_type = identity_demangle)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  {
    rmw_names_and_types_t names_and_types = rmw_get_zero_initialized_names_and_types();
    graph_cache.get_reader_names_and_types_by_node(
      node_name,
      node_namespace,
      demangle_topic,
      demangle_type,
      &allocator,
      &names_and_types);
    check_names_and_types(names_and_types, readers_names_and_types);
    EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_fini(&names_and_types));
  }

  {
    rmw_names_and_types_t names_and_types = rmw_get_zero_initialized_names_and_types();
    graph_cache.get_writer_names_and_types_by_node(
      node_name,
      node_namespace,
      demangle_topic,
      demangle_type,
      &allocator,
      &names_and_types);
    check_names_and_types(names_and_types, writers_names_and_types);
    EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_fini(&names_and_types));
  }
}


void
check_results_by_topic(
  const GraphCache & graph_cache,
  const std::string topic_name,
  size_t readers_count = 0U,
  size_t writers_count = 0U)
{
  {
    // Start with a value different from the expected one, to
    // ensure that the function is actually doing something.
    size_t count = readers_count + 1;
    EXPECT_EQ(
      graph_cache.get_reader_count(topic_name, &count),
      RMW_RET_OK);
    EXPECT_EQ(readers_count, count);
  }

  {
    size_t count = writers_count + 1;
    EXPECT_EQ(
      graph_cache.get_writer_count(topic_name, &count),
      RMW_RET_OK);
    EXPECT_EQ(writers_count, count);
  }
}

rmw_gid_t
gid_from_string(const std::string & str)
{
  rmw_gid_t gid = {};
  EXPECT_LT(str.size(), RMW_GID_STORAGE_SIZE);
  memcpy(
    reinterpret_cast<char *>(gid.data),
    str.c_str(),
    str.size() + 1);
  return gid;
}

struct EndpointInfo
{
  std::string gid;
  std::string node_namespace;
  std::string node_name;
  std::string topic_type;
};

void
check_results_by_topic(
  const GraphCache & graph_cache,
  const std::string topic_name,
  const std::vector<EndpointInfo> & readers_info,
  const std::vector<EndpointInfo> & writers_info)
{
  {
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    rmw_topic_endpoint_info_array_t info =
      rmw_get_zero_initialized_topic_endpoint_info_array();
    ASSERT_EQ(
      graph_cache.get_readers_info_by_topic(
        topic_name,
        identity_demangle,
        &allocator,
        &info),
      RMW_RET_OK);
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_EQ(rmw_topic_endpoint_info_array_fini(&info, &allocator), RMW_RET_OK);
    });
    ASSERT_EQ(info.size, readers_info.size());
    for (size_t i = 0; i < readers_info.size(); ++i) {
      rmw_gid_t endpoint_gid = {};
      memcpy(endpoint_gid.data, info.info_array[i].endpoint_gid, RMW_GID_STORAGE_SIZE);
      EXPECT_EQ(gid_from_string(readers_info[i].gid), endpoint_gid);
      EXPECT_EQ(readers_info[i].node_name, info.info_array[i].node_name);
      EXPECT_EQ(readers_info[i].node_namespace, info.info_array[i].node_namespace);
      EXPECT_EQ(readers_info[i].topic_type, info.info_array[i].topic_type);
      EXPECT_EQ(RMW_ENDPOINT_SUBSCRIPTION, info.info_array[i].endpoint_type);
    }
  }

  {
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    rmw_topic_endpoint_info_array_t info =
      rmw_get_zero_initialized_topic_endpoint_info_array();
    ASSERT_EQ(
      graph_cache.get_writers_info_by_topic(
        topic_name,
        identity_demangle,
        &allocator,
        &info),
      RMW_RET_OK);
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_EQ(rmw_topic_endpoint_info_array_fini(&info, &allocator), RMW_RET_OK);
    });
    ASSERT_EQ(info.size, writers_info.size());
    for (size_t i = 0; i < writers_info.size(); ++i) {
      rmw_gid_t endpoint_gid = {};
      memcpy(endpoint_gid.data, info.info_array[i].endpoint_gid, RMW_GID_STORAGE_SIZE);
      EXPECT_EQ(gid_from_string(writers_info[i].gid), endpoint_gid);
      EXPECT_EQ(writers_info[i].node_name, info.info_array[i].node_name);
      EXPECT_EQ(writers_info[i].node_namespace, info.info_array[i].node_namespace);
      EXPECT_EQ(writers_info[i].topic_type, info.info_array[i].topic_type);
      EXPECT_EQ(RMW_ENDPOINT_PUBLISHER, info.info_array[i].endpoint_type);
    }
  }
}

TEST(test_graph_cache, zero_initialized)
{
  GraphCache graph_cache;

  check_results(graph_cache);
  check_results_by_node(graph_cache, "some_namespace", "node/name");
  check_results_by_topic(graph_cache, "some/topic/name");
}

struct EntityInfo
{
  std::string gid;
  std::string participant_gid;
  std::string name;
  std::string type;
  bool is_reader;
};

void
add_entities(
  GraphCache & graph_cache,
  const std::vector<EntityInfo> & entities_info)
{
  for (const auto & elem : entities_info) {
    EXPECT_TRUE(
      graph_cache.add_entity(
        gid_from_string(elem.gid),
        elem.name,
        elem.type,
        gid_from_string(elem.participant_gid),
        rmw_qos_profile_default,
        elem.is_reader));
  }
}

void
remove_entities(
  GraphCache & graph_cache,
  const std::vector<EntityInfo> & entities_info)
{
  for (const auto & elem : entities_info) {
    EXPECT_TRUE(
      graph_cache.remove_entity(
        gid_from_string(elem.gid),
        elem.is_reader));
  }
}


TEST(test_graph_cache, add_remove_entities)
{
  GraphCache graph_cache;

  // Add some readers.
  add_entities(
    graph_cache,
  {
    // topic1 readers
    {"reader1", "participant1", "topic1", "Str", true},
    {"reader2", "participant1", "topic1", "Str", true},
    {"reader3", "participant1", "topic1", "Str", true},
    {"reader4", "participant1", "topic1", "Str", true},
    // topic2 readers
    {"reader5", "participant1", "topic2", "Str", true},
    {"reader6", "participant1", "topic2", "Int", true},
    // topic3 readers
    {"reader7", "participant1", "topic3", "Float", true},
  });

  // Check graph state.
  check_results(
    graph_cache,
    {},
  {
    {"topic1", {"Str"}},
    {"topic2", {"Int", "Str"}},
    {"topic3", {"Float"}},
  });
  check_results_by_node(graph_cache, "ns", "name");
  check_results_by_topic(graph_cache, "topic1", 4);
  check_results_by_topic(graph_cache, "topic2", 2);
  check_results_by_topic(graph_cache, "topic3", 1);

  // Add some writers.
  add_entities(
    graph_cache,
  {
    // topic1 writers
    {"writer1", "participant1", "topic1", "Str", false},
    {"writer2", "participant1", "topic1", "Str", false},
    // topic2 writers
    {"writer5", "participant1", "topic2", "Str", false},
    {"writer6", "participant1", "topic2", "Float", false},
    {"writer7", "participant1", "topic2", "Bool", false},
    // topic4 writers
    {"writer8", "participant1", "topic4", "Int", false},
  });

  // Check graph state.
  check_results(
    graph_cache,
    {},
  {
    {"topic1", {"Str"}},
    {"topic2", {"Bool", "Float", "Int", "Str"}},
    {"topic3", {"Float"}},
    {"topic4", {"Int"}},
  });
  check_results_by_node(graph_cache, "ns", "name");
  check_results_by_topic(graph_cache, "topic1", 4, 2);
  check_results_by_topic(graph_cache, "topic2", 2, 3);
  check_results_by_topic(graph_cache, "topic3", 1, 0);
  check_results_by_topic(graph_cache, "topic4", 0, 1);

  // Remove some readers and writers.
  remove_entities(
    graph_cache,
  {
    // topic1
    {"reader2", "participant1", "topic1", "Str", true},
    {"reader3", "participant1", "topic1", "Str", true},
    {"reader4", "participant1", "topic1", "Str", true},
    {"writer2", "participant1", "topic1", "Str", false},
    // topic2
    {"reader6", "participant1", "topic2", "Int", true},
    {"writer5", "participant1", "topic2", "Str", false},
    {"writer6", "participant1", "topic2", "Float", false},
    {"writer7", "participant1", "topic2", "Bool", false},
    // topic3
    {"reader7", "participant1", "topic3", "Float", true},
  });

  // Check graph state.
  check_results(
    graph_cache,
    {},
  {
    {"topic1", {"Str"}},
    {"topic2", {"Str"}},
    {"topic4", {"Int"}},
  });
  check_results_by_node(graph_cache, "ns", "name");
  check_results_by_topic(graph_cache, "topic1", 1, 1);
  check_results_by_topic(graph_cache, "topic2", 1, 0);
  check_results_by_topic(graph_cache, "topic3", 0, 0);
  check_results_by_topic(graph_cache, "topic4", 0, 1);

  // Remove the remaining readers and writers.
  remove_entities(
    graph_cache,
  {
    // topic1
    {"reader1", "participant1", "topic1", "Str", true},
    {"writer1", "participant1", "topic1", "Str", false},
    // topic2
    {"reader5", "participant1", "topic2", "Str", true},
    // topic4
    {"writer8", "participant1", "topic4", "Int", false},
  });

  // Check graph state.
  check_results(graph_cache);
  check_results_by_node(graph_cache, "ns", "name");
  check_results_by_topic(graph_cache, "topic1");
  check_results_by_topic(graph_cache, "topic2");
  check_results_by_topic(graph_cache, "topic3");
  check_results_by_topic(graph_cache, "topic4");
}

void
add_participants(
  GraphCache & graph_cache,
  const std::vector<std::string> & gids)
{
  for (const auto & gid : gids) {
    graph_cache.add_participant(gid_from_string(gid), "");
  }
}

void
remove_participants(
  GraphCache & graph_cache,
  const std::vector<std::string> & gids)
{
  for (const auto & gid : gids) {
    graph_cache.remove_participant(gid_from_string(gid));
  }
}

rmw_dds_common::msg::Gid
gid_msg_from_string(const std::string & str)
{
  rmw_dds_common::msg::Gid gid;
  gid.data = {};
  EXPECT_LT(str.size(), RMW_GID_STORAGE_SIZE);
  memcpy(
    reinterpret_cast<char *>(gid.data.data()),
    str.c_str(),
    str.size() + 1);
  return gid;
}

struct NodeInfo
{
  std::string gid;
  std::string namespace_;
  std::string name;
};

rmw_dds_common::msg::ParticipantEntitiesInfo
add_nodes(
  GraphCache & graph_cache,
  const std::vector<NodeInfo> & node_info)
{
  rmw_dds_common::msg::ParticipantEntitiesInfo msg;
  for (const auto & elem : node_info) {
    msg = graph_cache.add_node(
      gid_from_string(elem.gid),
      elem.name,
      elem.namespace_);
  }
  return msg;
}

rmw_dds_common::msg::ParticipantEntitiesInfo
remove_nodes(
  GraphCache & graph_cache,
  const std::vector<NodeInfo> & node_info)
{
  rmw_dds_common::msg::ParticipantEntitiesInfo msg;
  for (const auto & elem : node_info) {
    msg = graph_cache.remove_node(
      gid_from_string(elem.gid),
      elem.name,
      elem.namespace_);
  }
  return msg;
}

struct NodeEntitiesInfo
{
  std::string namespace_;
  std::string name;
  std::vector<std::string> reader_gids;
  std::vector<std::string> writer_gids;
};

struct ParticipantEntitiesInfo
{
  std::string gid;
  std::vector<NodeEntitiesInfo> node_entities_info_seq;
};

void
check_participant_entities_msg(
  const rmw_dds_common::msg::ParticipantEntitiesInfo & msg,
  const ParticipantEntitiesInfo & expected)
{
  EXPECT_EQ(msg.gid, gid_msg_from_string(expected.gid));
  ASSERT_EQ(msg.node_entities_info_seq.size(), expected.node_entities_info_seq.size());
  for (size_t i = 0; i < expected.node_entities_info_seq.size(); i++) {
    const auto & node_info = msg.node_entities_info_seq[i];
    const auto & node_info_expected = expected.node_entities_info_seq[i];
    EXPECT_EQ(node_info.node_namespace, node_info_expected.namespace_);
    EXPECT_EQ(node_info.node_name, node_info_expected.name);
    auto & readers_gids = node_info.reader_gid_seq;
    auto & expected_readers_gids = node_info_expected.reader_gids;
    ASSERT_EQ(readers_gids.size(), expected_readers_gids.size());
    for (size_t j = 0; j < readers_gids.size(); j++) {
      EXPECT_EQ(readers_gids[j], gid_msg_from_string(expected_readers_gids[j]));
    }
    auto & writers_gids = node_info.writer_gid_seq;
    auto & expected_writers_gids = node_info_expected.writer_gids;
    ASSERT_EQ(writers_gids.size(), expected_writers_gids.size());
    for (size_t j = 0; j < writers_gids.size(); j++) {
      EXPECT_EQ(writers_gids[j], gid_msg_from_string(expected_writers_gids[j]));
    }
  }
}

struct EntityAssociations
{
  std::string gid;
  bool is_reader;
  std::string participant_gid;
  std::string namespace_;
  std::string name;
};

void associate_entities(
  GraphCache & graph_cache,
  const std::vector<EntityAssociations> & associations)
{
  for (const auto & elem : associations) {
    if (elem.is_reader) {
      graph_cache.associate_reader(
        gid_from_string(elem.gid),
        gid_from_string(elem.participant_gid),
        elem.name,
        elem.namespace_);
    } else {
      graph_cache.associate_writer(
        gid_from_string(elem.gid),
        gid_from_string(elem.participant_gid),
        elem.name,
        elem.namespace_);
    }
  }
}

void dissociate_entities(
  GraphCache & graph_cache,
  const std::vector<EntityAssociations> & associations)
{
  for (const auto & elem : associations) {
    if (elem.is_reader) {
      graph_cache.dissociate_reader(
        gid_from_string(elem.gid),
        gid_from_string(elem.participant_gid),
        elem.name,
        elem.namespace_);
    } else {
      graph_cache.dissociate_writer(
        gid_from_string(elem.gid),
        gid_from_string(elem.participant_gid),
        elem.name,
        elem.namespace_);
    }
  }
}

rmw_dds_common::msg::ParticipantEntitiesInfo
get_participant_entities_info_msg(const ParticipantEntitiesInfo & info)
{
  rmw_dds_common::msg::ParticipantEntitiesInfo msg;
  msg.gid = gid_msg_from_string(info.gid);
  msg.node_entities_info_seq.reserve(info.node_entities_info_seq.size());
  for (const auto & elem : info.node_entities_info_seq) {
    msg.node_entities_info_seq.emplace_back();
    auto & node_info = msg.node_entities_info_seq.back();
    node_info.node_namespace = elem.namespace_;
    node_info.node_name = elem.name;
    auto & readers_gids = elem.reader_gids;
    node_info.reader_gid_seq.reserve(readers_gids.size());
    for (const auto & reader_gid : readers_gids) {
      node_info.reader_gid_seq.emplace_back(gid_msg_from_string(reader_gid));
    }
    auto & writers_gids = elem.writer_gids;
    node_info.writer_gid_seq.reserve(writers_gids.size());
    for (const auto & writer_gid : writers_gids) {
      node_info.writer_gid_seq.emplace_back(gid_msg_from_string(writer_gid));
    }
  }
  return msg;
}

TEST(test_graph_cache, normal_usage)
{
  GraphCache graph_cache;

  bool change_callback_called = false;
  auto change_callback = [&change_callback_called]() {
      change_callback_called = true;
    };
  graph_cache.set_on_change_callback(change_callback);

  // Add one participant.
  change_callback_called = false;
  add_participants(graph_cache, {"participant1"});
  EXPECT_TRUE(change_callback_called);

  // Check state.
  check_results(graph_cache);
  check_results_by_node(graph_cache, "ns", "some_random_node");
  check_results_by_topic(graph_cache, "some_topic");

  // Add some nodes.
  change_callback_called = false;
  check_participant_entities_msg(
    add_nodes(
      graph_cache, {
    {"participant1", "ns1", "node1"},
    {"participant1", "ns1", "node2"},
    {"participant1", "ns2", "node1"}}),
  {
    "participant1", {
      {"ns1", "node1", {}, {}},
      {"ns1", "node2", {}, {}},
      {"ns2", "node1", {}, {}},
    }
  });
  EXPECT_TRUE(change_callback_called);

  // Check state.
  check_results(
    graph_cache,
  {
    {"ns1", "node1"},
    {"ns1", "node2"},
    {"ns2", "node1"},
  });
  check_results_by_node(graph_cache, "ns1", "node1");
  check_results_by_node(graph_cache, "ns1", "node2");
  check_results_by_node(graph_cache, "ns2", "node1");
  check_results_by_node(graph_cache, "ns", "some_random_node");
  check_results_by_topic(graph_cache, "some_topic");

  // Add more participants and nodes.
  add_participants(graph_cache, {"participant2", "participant3"});
  check_participant_entities_msg(
    add_nodes(
      graph_cache, {
    {"participant2", "ns1", "node3"},
    {"participant2", "ns3", "node1"}}),
  {
    "participant2", {
      {"ns1", "node3", {}, {}},
      {"ns3", "node1", {}, {}},
    }
  });

  // Check state.
  check_results(
    graph_cache,
  {
    {"ns1", "node1"},
    {"ns1", "node2"},
    {"ns1", "node3"},
    {"ns2", "node1"},
    {"ns3", "node1"},
  });
  check_results_by_node(graph_cache, "ns1", "node1");
  check_results_by_node(graph_cache, "ns1", "node2");
  check_results_by_node(graph_cache, "ns1", "node3");
  check_results_by_node(graph_cache, "ns2", "node1");
  check_results_by_node(graph_cache, "ns3", "node1");
  check_results_by_node(graph_cache, "ns", "some_random_node");
  check_results_by_topic(graph_cache, "some_topic");

  // Add some readers and writers.
  change_callback_called = false;
  add_entities(
    graph_cache,
  {
    // topic1
    {"reader1", "participant1", "topic1", "Str", true},
    {"reader2", "participant1", "topic1", "Float", true},
    {"writer1", "participant2", "topic1", "Int", false},
    {"writer2", "participant2", "topic1", "Str", false},
    // topic2
    {"reader3", "participant1", "topic2", "Str", true},
    {"reader4", "participant1", "topic2", "Str", true},
    {"reader5", "participant2", "topic2", "Str", true},
    // topic3
    {"writer3", "participant1", "topic3", "Bool", false},
  });
  EXPECT_TRUE(change_callback_called);

  // Check state.
  check_results(
    graph_cache,
  {
    {"ns1", "node1"},
    {"ns1", "node2"},
    {"ns1", "node3"},
    {"ns2", "node1"},
    {"ns3", "node1"},
  },
  {
    {"topic1", {"Float", "Int", "Str"}},
    {"topic2", {"Str"}},
    {"topic3", {"Bool"}},
  });
  check_results_by_node(graph_cache, "ns1", "node1");
  check_results_by_node(graph_cache, "ns1", "node2");
  check_results_by_node(graph_cache, "ns1", "node3");
  check_results_by_node(graph_cache, "ns2", "node1");
  check_results_by_node(graph_cache, "ns3", "node1");
  check_results_by_node(graph_cache, "ns", "some_random_node");
  check_results_by_topic(graph_cache, "topic1", 2, 2);
  check_results_by_topic(graph_cache, "topic2", 3, 0);
  check_results_by_topic(graph_cache, "topic3", 0, 1);
  check_results_by_topic(graph_cache, "some_topic", 0, 0);

  // Associate entities
  change_callback_called = false;
  associate_entities(
    graph_cache,
  {
    // participant1, ns1, node1
    {"reader1", true, "participant1", "ns1", "node1"},
    {"reader2", true, "participant1", "ns1", "node1"},
    {"reader4", true, "participant1", "ns1", "node1"},
    {"writer3", false, "participant1", "ns1", "node1"},
    // participant1, ns2, node1
    {"reader3", true, "participant1", "ns2", "node1"},
    // participant2, ns1, node3
    {"reader5", true, "participant2", "ns1", "node3"},
    {"writer1", false, "participant2", "ns1", "node3"},
    {"writer2", false, "participant2", "ns1", "node3"},
  });
  EXPECT_TRUE(change_callback_called);

  // Check state.
  check_results(
    graph_cache,
  {
    {"ns1", "node1"},
    {"ns1", "node2"},
    {"ns1", "node3"},
    {"ns2", "node1"},
    {"ns3", "node1"},
  },
  {
    {"topic1", {"Float", "Int", "Str"}},
    {"topic2", {"Str"}},
    {"topic3", {"Bool"}},
  });
  check_results_by_node(
    graph_cache, "ns1", "node1",
  {
    {"topic1", {"Float", "Str"}},
    {"topic2", {"Str"}},
  },
  {
    {"topic3", {"Bool"}},
  });
  check_results_by_node(graph_cache, "ns1", "node2");
  check_results_by_node(
    graph_cache, "ns1", "node3",
  {
    {"topic2", {"Str"}},
  },
  {
    {"topic1", {"Int", "Str"}},
  });
  check_results_by_node(
    graph_cache, "ns2", "node1",
  {
    {"topic2", {"Str"}},
  });
  check_results_by_node(graph_cache, "ns3", "node1");
  check_results_by_node(graph_cache, "ns", "some_random_node");
  check_results_by_topic(
    graph_cache, "topic1",
  {
    {"reader1", "ns1", "node1", "Str"},
    {"reader2", "ns1", "node1", "Float"},
  },
  {
    {"writer1", "ns1", "node3", "Int"},
    {"writer2", "ns1", "node3", "Str"},
  });
  check_results_by_topic(graph_cache, "topic2", 3, 0);
  check_results_by_topic(graph_cache, "topic3", 0, 1);
  check_results_by_topic(graph_cache, "some_topic", 0, 0);

  // Associate entities
  change_callback_called = false;
  dissociate_entities(
    graph_cache,
  {
    // participant1, ns1, node1
    {"reader1", true, "participant1", "ns1", "node1"},
    {"reader2", true, "participant1", "ns1", "node1"},
    // participant2, ns1, node3
    {"reader5", true, "participant2", "ns1", "node3"},
    {"writer1", false, "participant2", "ns1", "node3"},
    {"writer2", false, "participant2", "ns1", "node3"},
  });
  EXPECT_TRUE(change_callback_called);

  // Check state.
  check_results(
    graph_cache,
  {
    {"ns1", "node1"},
    {"ns1", "node2"},
    {"ns1", "node3"},
    {"ns2", "node1"},
    {"ns3", "node1"},
  },
  {
    {"topic1", {"Float", "Int", "Str"}},
    {"topic2", {"Str"}},
    {"topic3", {"Bool"}},
  });
  check_results_by_node(
    graph_cache, "ns1", "node1",
  {
    {"topic2", {"Str"}},
  },
  {
    {"topic3", {"Bool"}},
  });
  check_results_by_node(graph_cache, "ns1", "node2");
  check_results_by_node(graph_cache, "ns1", "node3");
  check_results_by_node(
    graph_cache, "ns2", "node1",
  {
    {"topic2", {"Str"}},
  });
  check_results_by_node(graph_cache, "ns3", "node1");
  check_results_by_node(graph_cache, "ns", "some_random_node");
  check_results_by_topic(graph_cache, "topic1", 2, 2);
  check_results_by_topic(graph_cache, "topic2", 3, 0);
  check_results_by_topic(graph_cache, "topic3", 0, 1);
  check_results_by_topic(graph_cache, "some_topic", 0, 0);

  graph_cache.clear_on_change_callback();
  change_callback_called = false;

  // Add some readers and writers.
  add_entities(
    graph_cache,
  {
    // topic1
    {"reader6", "remote_participant", "topic1", "Str", true},
    {"reader7", "remote_participant", "topic1", "Custom", true},
    // topic2
    {"writer4", "remote_participant", "topic2", "Str", false},
    // topic4
    {"writer5", "remote_participant", "topic4", "Custom", false},
    {"writer6", "remote_participant", "topic4", "Bool", false},
  });

  // Associate them with a remote participant.
  auto msg = get_participant_entities_info_msg(
  {
    "remote_participant",
    {
      {
        "ns3",
        "node2",
        {"reader6"},
        {"writer4", "writer5"}
      },
      {
        "ns4",
        "node1",
        {"reader7"},
        {}
      }
    }
  });
  graph_cache.update_participant_entities(msg);
  EXPECT_FALSE(change_callback_called);

  // Check state.
  check_results(
    graph_cache,
  {
    {"ns1", "node1"},
    {"ns1", "node2"},
    {"ns1", "node3"},
    {"ns2", "node1"},
    {"ns3", "node1"},
    {"ns3", "node2"},
    {"ns4", "node1"},
  },
  {
    {"topic1", {"Custom", "Float", "Int", "Str"}},
    {"topic2", {"Str"}},
    {"topic3", {"Bool"}},
    {"topic4", {"Bool", "Custom"}},
  });
  check_results_by_node(
    graph_cache, "ns1", "node1",
  {
    {"topic2", {"Str"}},
  },
  {
    {"topic3", {"Bool"}},
  });
  check_results_by_node(graph_cache, "ns1", "node2");
  check_results_by_node(graph_cache, "ns1", "node3");
  check_results_by_node(
    graph_cache, "ns2", "node1",
  {
    {"topic2", {"Str"}},
  });
  check_results_by_node(graph_cache, "ns3", "node1");
  check_results_by_node(
    graph_cache, "ns3", "node2",
  {
    {"topic1", {"Str"}},
  },
  {
    {"topic2", {"Str"}},
    {"topic4", {"Custom"}},
  });
  check_results_by_node(
    graph_cache, "ns4", "node1",
  {
    {"topic1", {"Custom"}},
  });
  check_results_by_node(graph_cache, "ns", "some_random_node");
  check_results_by_topic(graph_cache, "topic1", 4, 2);
  check_results_by_topic(graph_cache, "topic2", 3, 1);
  check_results_by_topic(graph_cache, "topic3", 0, 1);
  check_results_by_topic(
    graph_cache, "topic4", {}, {
    {"writer5", "ns3", "node2", "Custom"},
    {"writer6", "_NODE_NAMESPACE_UNKNOWN_", "_NODE_NAME_UNKNOWN_", "Bool"},
  });
  check_results_by_topic(graph_cache, "some_topic", 0, 0);

  // Remove some readers and writers.
  remove_entities(
    graph_cache,
  {
    // topic1
    {"reader6", "remote_participant", "topic1", "Str", true},
    // topic2
    {"writer4", "remote_participant", "topic2", "Str", false},
    // topic4
    {"writer5", "remote_participant", "topic4", "Custom", false},
    {"writer6", "remote_participant", "topic4", "Bool", false},
  });

  // Check state.
  check_results(
    graph_cache,
  {
    {"ns1", "node1"},
    {"ns1", "node2"},
    {"ns1", "node3"},
    {"ns2", "node1"},
    {"ns3", "node1"},
    {"ns3", "node2"},
    {"ns4", "node1"},
  },
  {
    {"topic1", {"Custom", "Float", "Int", "Str"}},
    {"topic2", {"Str"}},
    {"topic3", {"Bool"}},
  });
  check_results_by_node(
    graph_cache, "ns1", "node1",
  {
    {"topic2", {"Str"}},
  },
  {
    {"topic3", {"Bool"}},
  });
  check_results_by_node(graph_cache, "ns1", "node2");
  check_results_by_node(graph_cache, "ns1", "node3");
  check_results_by_node(
    graph_cache, "ns2", "node1",
  {
    {"topic2", {"Str"}},
  });
  check_results_by_node(graph_cache, "ns3", "node1");
  check_results_by_node(graph_cache, "ns3", "node2");
  check_results_by_node(
    graph_cache, "ns4", "node1",
  {
    {"topic1", {"Custom"}},
  });
  check_results_by_node(graph_cache, "ns", "some_random_node");
  check_results_by_topic(graph_cache, "topic1", 3, 2);
  check_results_by_topic(graph_cache, "topic2", 3, 0);
  check_results_by_topic(graph_cache, "topic3", 0, 1);
  check_results_by_topic(graph_cache, "topic4", 0, 0);
  check_results_by_topic(graph_cache, "some_topic", 0, 0);

  // Associate them with a remote participant.
  msg = get_participant_entities_info_msg(
  {
    "remote_participant",
    {
      {
        "ns4",
        "node1",
        {"reader7"},
        {}
      }
    }
  });
  graph_cache.update_participant_entities(msg);

  // Check state.
  check_results(
    graph_cache,
  {
    {"ns1", "node1"},
    {"ns1", "node2"},
    {"ns1", "node3"},
    {"ns2", "node1"},
    {"ns3", "node1"},
    {"ns4", "node1"},
  },
  {
    {"topic1", {"Custom", "Float", "Int", "Str"}},
    {"topic2", {"Str"}},
    {"topic3", {"Bool"}},
  });
  check_results_by_node(
    graph_cache, "ns1", "node1",
  {
    {"topic2", {"Str"}},
  },
  {
    {"topic3", {"Bool"}},
  });
  check_results_by_node(graph_cache, "ns1", "node2");
  check_results_by_node(graph_cache, "ns1", "node3");
  check_results_by_node(
    graph_cache, "ns2", "node1",
  {
    {"topic2", {"Str"}},
  });
  check_results_by_node(graph_cache, "ns3", "node1");
  check_results_by_node(graph_cache, "ns3", "node2");
  check_results_by_node(
    graph_cache, "ns4", "node1",
  {
    {"topic1", {"Custom"}},
  });
  check_results_by_node(graph_cache, "ns", "some_random_node");
  check_results_by_topic(graph_cache, "topic1", 3, 2);
  check_results_by_topic(graph_cache, "topic2", 3, 0);
  check_results_by_topic(graph_cache, "topic3", 0, 1);
  check_results_by_topic(graph_cache, "topic4", 0, 0);
  check_results_by_topic(graph_cache, "some_topic", 0, 0);

  // Remove remote participant
  msg = get_participant_entities_info_msg(
  {
    "remote_participant",
    {}
  });
  graph_cache.update_participant_entities(msg);

  remove_participants(graph_cache, {"remote_participant"});

  // Remove remaining entities
  remove_entities(
    graph_cache,
  {
    // topic1
    {"reader7", "remote_participant", "topic1", "Custom", true},
  });

  // Check state.
  check_results(
    graph_cache,
  {
    {"ns1", "node1"},
    {"ns1", "node2"},
    {"ns1", "node3"},
    {"ns2", "node1"},
    {"ns3", "node1"},
  },
  {
    {"topic1", {"Float", "Int", "Str"}},
    {"topic2", {"Str"}},
    {"topic3", {"Bool"}},
  });
  check_results_by_node(
    graph_cache, "ns1", "node1",
  {
    {"topic2", {"Str"}},
  },
  {
    {"topic3", {"Bool"}},
  });
  check_results_by_node(graph_cache, "ns1", "node2");
  check_results_by_node(graph_cache, "ns1", "node3");
  check_results_by_node(
    graph_cache, "ns2", "node1",
  {
    {"topic2", {"Str"}},
  });
  check_results_by_node(graph_cache, "ns3", "node1");
  check_results_by_node(graph_cache, "ns", "some_random_node");
  check_results_by_topic(graph_cache, "topic1", 2, 2);
  check_results_by_topic(graph_cache, "topic2", 3, 0);
  check_results_by_topic(graph_cache, "topic3", 0, 1);
  check_results_by_topic(graph_cache, "topic4", 0, 0);
  check_results_by_topic(graph_cache, "some_topic", 0, 0);

  // Remove some local nodes
  remove_nodes(
    graph_cache,
  {
    {"participant1", "ns1", "node2"},
    {"participant1", "ns2", "node1"},
    {"participant2", "ns1", "node3"},
    {"participant2", "ns3", "node1"},
  });

  // Remove some local participants
  remove_participants(
    graph_cache,
    {"participant2", "participant3"});

  // Remove some entities
  remove_entities(
    graph_cache,
  {
    // topic1
    {"reader1", "participant1", "topic1", "Str", true},
    {"reader2", "participant1", "topic1", "Float", true},
    {"writer1", "participant2", "topic1", "Int", false},
    {"writer2", "participant2", "topic1", "Str", false},
    // topic2
    {"reader3", "participant1", "topic2", "Str", true},
    {"reader4", "participant1", "topic2", "Str", true},
    {"reader5", "participant2", "topic2", "Str", true},
  });

  // Check state.
  check_results(
    graph_cache,
  {
    {"ns1", "node1"},
  },
  {
    {"topic3", {"Bool"}},
  });
  check_results_by_node(
    graph_cache, "ns1", "node1",
    {},
  {
    {"topic3", {"Bool"}},
  });
  check_results_by_node(graph_cache, "ns1", "node2");
  check_results_by_node(graph_cache, "ns1", "node3");
  check_results_by_node(graph_cache, "ns2", "node1");
  check_results_by_node(graph_cache, "ns3", "node1");
  check_results_by_node(graph_cache, "ns", "some_random_node");
  check_results_by_topic(graph_cache, "topic1", 0, 0);
  check_results_by_topic(graph_cache, "topic2", 0, 0);
  check_results_by_topic(graph_cache, "topic3", 0, 1);
  check_results_by_topic(graph_cache, "topic4", 0, 0);
  check_results_by_topic(graph_cache, "some_topic", 0, 0);

  // Remove some local nodes
  remove_nodes(
    graph_cache,
  {
    {"participant1", "ns1", "node1"},
  });

  // Remove every remaining participant, node, entity
  remove_participants(
    graph_cache,
    {"participant1"});

  // Remove some entities
  remove_entities(
    graph_cache,
  {
    // topic3
    {"writer3", "participant1", "topic3", "Bool", false},
  });

  // Check state.
  check_results(graph_cache);
  check_results_by_node(graph_cache, "ns1", "node1");
  check_results_by_node(graph_cache, "ns1", "node2");
  check_results_by_node(graph_cache, "ns1", "node3");
  check_results_by_node(graph_cache, "ns2", "node1");
  check_results_by_node(graph_cache, "ns3", "node1");
  check_results_by_node(graph_cache, "ns", "some_random_node");
  check_results_by_topic(graph_cache, "topic1", 0, 0);
  check_results_by_topic(graph_cache, "topic2", 0, 0);
  check_results_by_topic(graph_cache, "topic3", 0, 0);
  check_results_by_topic(graph_cache, "topic4", 0, 0);
  check_results_by_topic(graph_cache, "some_topic", 0, 0);

  EXPECT_FALSE(change_callback_called);
}

TEST(test_graph_cache, test_operator)
{
  GraphCache graph_cache;

  // Add one participant.
  add_participants(graph_cache, {"participant1"});

  std::string graph_cache_case1_str = "---------------------------------\n"
    "Graph cache:\n"
    "  Discovered data writers:\n"
    "  Discovered data readers:\n"
    "  Discovered participants:\n"
    "    gid: '70.61.72.74.69.63.69.70.61.6e.74.31.0.0.0.0.0.0.0.0.0.0.0.0\n"
    "    enclave name '\n"
    "    nodes:\n"
    "---------------------------------\n";

  std::ostringstream stream_case1;
  stream_case1 << graph_cache;
  ASSERT_STREQ(stream_case1.str().c_str(), graph_cache_case1_str.c_str());

  std::string graph_cache_case2_str = "---------------------------------\n"
    "Graph cache:\n"
    "  Discovered data writers:\n"
    "  Discovered data readers:\n"
    "  Discovered participants:\n"
    "    gid: '70.61.72.74.69.63.69.70.61.6e.74.31.0.0.0.0.0.0.0.0.0.0.0.0\n"
    "    enclave name '\n"
    "    nodes:\n"
    "      namespace: 'ns1' name: 'node1'\n"
    "      associated data readers gids:\n"
    "      associated data writers gids:\n"
    "      namespace: 'ns1' name: 'node2'\n"
    "      associated data readers gids:\n"
    "      associated data writers gids:\n"
    "      namespace: 'ns2' name: 'node1'\n"
    "      associated data readers gids:\n"
    "      associated data writers gids:\n"
    "---------------------------------\n";

  // Add some nodes.
  check_participant_entities_msg(
    add_nodes(
      graph_cache, {
    {"participant1", "ns1", "node1"},
    {"participant1", "ns1", "node2"},
    {"participant1", "ns2", "node1"}}),
  {
    "participant1", {
      {"ns1", "node1", {}, {}},
      {"ns1", "node2", {}, {}},
      {"ns2", "node1", {}, {}},
    }
  });

  std::ostringstream stream_case2;
  stream_case2 << graph_cache;
  ASSERT_STREQ(stream_case2.str().c_str(), graph_cache_case2_str.c_str());

  std::string graph_cache_case3_str = "---------------------------------\n"
    "Graph cache:\n"
    "  Discovered data writers:\n"
    "    gid: '77.72.69.74.65.72.31.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0',"
    " topic name: 'topic1', topic_type: 'Str'\n"
    "  Discovered data readers:\n"
    "    gid: '72.65.61.64.65.72.31.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0',"
    " topic name: 'topic1', topic_type: 'Str'\n"
    "  Discovered participants:\n"
    "    gid: '70.61.72.74.69.63.69.70.61.6e.74.31.0.0.0.0.0.0.0.0.0.0.0.0\n"
    "    enclave name '\n"
    "    nodes:\n"
    "      namespace: 'ns1' name: 'node1'\n"
    "      associated data readers gids:\n"
    "        72.65.61.64.65.72.31.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0\n"
    "      associated data writers gids:\n"
    "      namespace: 'ns1' name: 'node2'\n"
    "      associated data readers gids:\n"
    "      associated data writers gids:\n"
    "        77.72.69.74.65.72.31.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0\n"
    "      namespace: 'ns2' name: 'node1'\n"
    "      associated data readers gids:\n"
    "      associated data writers gids:\n"
    "---------------------------------\n";

  add_entities(
    graph_cache,
  {
    // topic1
    {"reader1", "participant1", "topic1", "Str", true},
    {"writer1", "participant1", "topic1", "Str", false},
  });

  // Associate entities
  associate_entities(
    graph_cache,
  {
    // participant1, ns1, node1
    {"reader1", true, "participant1", "ns1", "node1"},
    {"writer1", false, "participant1", "ns1", "node2"},
  });
  std::ostringstream stream_case3;
  stream_case3 << graph_cache;
  ASSERT_STREQ(stream_case3.str().c_str(), graph_cache_case3_str.c_str());
}

TEST(test_graph_cache, bad_arguments)
{
  GraphCache graph_cache;

  EXPECT_EQ(
    graph_cache.get_reader_count("topic_name", nullptr),
    RMW_RET_INVALID_ARGUMENT);

  EXPECT_EQ(
    graph_cache.get_writer_count("topic_name", nullptr),
    RMW_RET_INVALID_ARGUMENT);

  {
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    rcutils_string_array_t names = rcutils_get_zero_initialized_string_array();
    rmw_ret_t ret = rcutils_string_array_init(&names, 3, &allocator);
    EXPECT_EQ(ret, RMW_RET_OK) << rcutils_get_error_string().str;
    rcutils_reset_error();
    rcutils_string_array_t namespaces = rcutils_get_zero_initialized_string_array();
    ret = graph_cache.get_node_names(&names, &namespaces, nullptr, &allocator);
    EXPECT_EQ(ret, RMW_RET_INVALID_ARGUMENT) << rcutils_get_error_string().str;
    rcutils_reset_error();
    ret = rcutils_string_array_fini(&names);
    EXPECT_EQ(ret, RMW_RET_OK) << rcutils_get_error_string().str;
    rcutils_reset_error();
  }

  {
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    rcutils_string_array_t names = rcutils_get_zero_initialized_string_array();
    rcutils_string_array_t namespaces = rcutils_get_zero_initialized_string_array();
    rmw_ret_t ret = rcutils_string_array_init(&namespaces, 3, &allocator);
    EXPECT_EQ(ret, RMW_RET_OK) << rcutils_get_error_string().str;
    rcutils_reset_error();
    ret = graph_cache.get_node_names(&names, &namespaces, nullptr, &allocator);
    EXPECT_EQ(ret, RMW_RET_INVALID_ARGUMENT) << rcutils_get_error_string().str;
    rcutils_reset_error();
    ret = rcutils_string_array_fini(&namespaces);
    EXPECT_EQ(ret, RMW_RET_OK) << rcutils_get_error_string().str;
    rcutils_reset_error();
  }

  {
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    rcutils_string_array_t names = rcutils_get_zero_initialized_string_array();
    rcutils_string_array_t namespaces = rcutils_get_zero_initialized_string_array();
    rcutils_string_array_t enclaves = rcutils_get_zero_initialized_string_array();
    rmw_ret_t ret = rcutils_string_array_init(&enclaves, 3, &allocator);
    EXPECT_EQ(ret, RMW_RET_OK) << rcutils_get_error_string().str;
    rcutils_reset_error();
    ret = graph_cache.get_node_names(&names, &namespaces, &enclaves, &allocator);
    EXPECT_EQ(ret, RMW_RET_INVALID_ARGUMENT) << rcutils_get_error_string().str;
    rcutils_reset_error();
    ret = rcutils_string_array_fini(&enclaves);
    EXPECT_EQ(ret, RMW_RET_OK) << rcutils_get_error_string().str;
    rcutils_reset_error();
  }

  {
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    rcutils_allocator_t zero_allocator = rcutils_get_zero_initialized_allocator();
    rmw_names_and_types_t names_and_types = rmw_get_zero_initialized_names_and_types();
    rmw_ret_t ret = graph_cache.get_writer_names_and_types_by_node(
      "node_name",
      "node_namespace",
      identity_demangle,
      identity_demangle,
      &allocator,
      &names_and_types);
    EXPECT_EQ(ret, RMW_RET_NODE_NAME_NON_EXISTENT);
    rcutils_reset_error();
    ret = graph_cache.get_writer_names_and_types_by_node(
      "node_name",
      "node_namespace",
      identity_demangle,
      identity_demangle,
      &zero_allocator,
      &names_and_types);
    EXPECT_EQ(ret, RMW_RET_INVALID_ARGUMENT);
    rcutils_reset_error();
    ret = graph_cache.get_writer_names_and_types_by_node(
      "node_name",
      "node_namespace",
      identity_demangle,
      identity_demangle,
      &allocator,
      nullptr);
    EXPECT_EQ(ret, RMW_RET_INVALID_ARGUMENT);
    rcutils_reset_error();
  }

  // Add one participant.
  add_participants(graph_cache, {"participant1"});
  {
    rmw_topic_endpoint_info_array_t topic_endpoint_info_array_sub =
      rmw_get_zero_initialized_topic_endpoint_info_array();
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    rmw_ret_t ret = graph_cache.get_writers_info_by_topic(
      "topic1",
      identity_demangle,
      &allocator,
      &topic_endpoint_info_array_sub);
    EXPECT_EQ(ret, RMW_RET_OK);
    ASSERT_EQ(topic_endpoint_info_array_sub.size, 0u) << "Expected one publisher";
    EXPECT_EQ(topic_endpoint_info_array_sub.info_array, nullptr);
  }

  // Add some nodes.
  add_nodes(
    graph_cache, {
    {"participant1", "ns1", "node1"},
    {"participant1", "ns1", "node2"},
    {"participant1", "ns2", "node1"}});

  add_entities(
    graph_cache,
  {
    // topic1
    {"reader1", "some_participant", "topic1", "Str", true},
    {"writer1", "some_participant", "topic1", "Str", false},
  });

  // Associate entities
  associate_entities(
    graph_cache,
  {
    // participant1, ns1, node1
    {"reader1", true, "participant1", "ns1", "node1"},
    {"writer1", false, "participant1", "ns1", "node2"},
  });

  {
    rcutils_allocator_t failing_allocator = get_failing_allocator();
    rcutils_string_array_t names = rcutils_get_zero_initialized_string_array();
    rcutils_string_array_t namespaces = rcutils_get_zero_initialized_string_array();
    rmw_ret_t ret = graph_cache.get_node_names(&names, &namespaces, nullptr, &failing_allocator);
    EXPECT_EQ(ret, RMW_RET_BAD_ALLOC);
    rcutils_reset_error();
  }

  {
    rmw_topic_endpoint_info_array_t topic_endpoint_info_array_sub =
      rmw_get_zero_initialized_topic_endpoint_info_array();
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    rmw_ret_t ret = graph_cache.get_writers_info_by_topic(
      "topic1",
      identity_demangle,
      &allocator,
      &topic_endpoint_info_array_sub);
    EXPECT_EQ(ret, RMW_RET_OK);
    ASSERT_EQ(topic_endpoint_info_array_sub.size, 1u) << "Expected one publisher";
    rmw_topic_endpoint_info_t topic_endpoint_info_pub = topic_endpoint_info_array_sub.info_array[0];
    EXPECT_STREQ(topic_endpoint_info_pub.node_name, "_CREATED_BY_BARE_DDS_APP_");
    EXPECT_STREQ(topic_endpoint_info_pub.node_namespace, "_CREATED_BY_BARE_DDS_APP_");
    EXPECT_STREQ(topic_endpoint_info_pub.topic_type, "Str");
    EXPECT_EQ(
      RMW_RET_OK,
      rmw_topic_endpoint_info_array_fini(
        &topic_endpoint_info_array_sub,
        &allocator));
    rcutils_reset_error();
  }
  {
    rmw_topic_endpoint_info_array_t topic_endpoint_info_array_sub =
      rmw_get_zero_initialized_topic_endpoint_info_array();
    rcutils_allocator_t failing_allocator = get_failing_allocator();
    rmw_ret_t ret = graph_cache.get_writers_info_by_topic(
      "topic1",
      identity_demangle,
      &failing_allocator,
      &topic_endpoint_info_array_sub);
    EXPECT_EQ(ret, RMW_RET_BAD_ALLOC);
    ASSERT_EQ(topic_endpoint_info_array_sub.size, 0u) << "Expected one publisher";
    EXPECT_EQ(topic_endpoint_info_array_sub.info_array, nullptr);
    rcutils_reset_error();
  }
  {
    rmw_topic_endpoint_info_array_t topic_endpoint_info_array_sub =
      rmw_get_zero_initialized_topic_endpoint_info_array();
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    rmw_ret_t ret = graph_cache.get_readers_info_by_topic(
      "topic1",
      identity_demangle,
      &allocator,
      &topic_endpoint_info_array_sub);
    EXPECT_EQ(ret, RMW_RET_OK);
    ASSERT_EQ(topic_endpoint_info_array_sub.size, 1u) << "Expected one publisher";
    rmw_topic_endpoint_info_t topic_endpoint_info_pub = topic_endpoint_info_array_sub.info_array[0];
    EXPECT_STREQ(topic_endpoint_info_pub.node_name, "_CREATED_BY_BARE_DDS_APP_");
    EXPECT_STREQ(topic_endpoint_info_pub.node_namespace, "_CREATED_BY_BARE_DDS_APP_");
    EXPECT_STREQ(topic_endpoint_info_pub.topic_type, "Str");
    EXPECT_EQ(
      RMW_RET_OK,
      rmw_topic_endpoint_info_array_fini(
        &topic_endpoint_info_array_sub,
        &allocator));
    rcutils_reset_error();
  }
  {
    rmw_topic_endpoint_info_array_t topic_endpoint_info_array_sub =
      rmw_get_zero_initialized_topic_endpoint_info_array();
    rcutils_allocator_t failing_allocator = get_failing_allocator();
    rmw_ret_t ret = graph_cache.get_readers_info_by_topic(
      "topic1",
      identity_demangle,
      &failing_allocator,
      &topic_endpoint_info_array_sub);
    EXPECT_EQ(ret, RMW_RET_BAD_ALLOC);
    ASSERT_EQ(topic_endpoint_info_array_sub.size, 0u) << "Expected one publisher";
    EXPECT_EQ(topic_endpoint_info_array_sub.info_array, nullptr);
    rcutils_reset_error();
  }
}

class TestGraphCache : public ::testing::Test
{
public:
  void SetUp()
  {
    add_participants(graph_cache_, {"participant1"});
    add_entities(
      graph_cache_,
    {
      // topic1
      {"reader1", "participant1", "topic1", "Str", true},
      {"writer1", "participant1", "topic1", "Str", false},
    });
    add_nodes(
      graph_cache_, {
      {"participant1", "ns1", "node1"},
      {"participant1", "ns1", "node2"},
      {"participant1", "ns2", "node1"}});
  }

protected:
  GraphCache graph_cache_;
};

TEST_F(TestGraphCache, get_writers_info_by_topic_maybe_fail)
{
  RCUTILS_FAULT_INJECTION_TEST(
  {
    rmw_topic_endpoint_info_array_t info = rmw_get_zero_initialized_topic_endpoint_info_array();
    rcutils_allocator_t allocator = rcutils_get_default_allocator();

    rmw_ret_t ret = graph_cache_.get_writers_info_by_topic(
      "topic1",
      identity_demangle,
      &allocator,
      &info);
    if (RMW_RET_OK == ret) {
      ret = rmw_topic_endpoint_info_array_fini(&info, &allocator);
      if (RMW_RET_OK != ret) {
        // If fault injection causes fini to fail, it should work the second time.
        EXPECT_EQ(RMW_RET_OK, rmw_topic_endpoint_info_array_fini(&info, &allocator));
      }
    } else {
      rcutils_reset_error();
    }
  });
}

TEST_F(TestGraphCache, get_node_names_maybe_fail)
{
  RCUTILS_FAULT_INJECTION_TEST(
  {
    rcutils_string_array_t names = rcutils_get_zero_initialized_string_array();
    rcutils_string_array_t namespaces = rcutils_get_zero_initialized_string_array();
    rcutils_string_array_t enclaves = rcutils_get_zero_initialized_string_array();
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    rmw_ret_t ret = graph_cache_.get_node_names(&names, &namespaces, &enclaves, &allocator);
    if (RMW_RET_OK == ret) {
      // rcutils_string_array_fini is not under test, so disable fault injection test here.
      int64_t fault_injection_count = rcutils_fault_injection_get_count();
      rcutils_fault_injection_set_count(RCUTILS_FAULT_INJECTION_NEVER_FAIL);

      EXPECT_EQ(RCUTILS_RET_OK, rcutils_string_array_fini(&names));
      EXPECT_EQ(RCUTILS_RET_OK, rcutils_string_array_fini(&namespaces));
      EXPECT_EQ(RCUTILS_RET_OK, rcutils_string_array_fini(&enclaves));

      rcutils_fault_injection_set_count(fault_injection_count);
    } else {
      rcutils_reset_error();
    }
  });
}

TEST_F(TestGraphCache, get_names_and_types_maybe_fail)
{
  RCUTILS_FAULT_INJECTION_TEST(
  {
    DemangleFunctionT demangle_topic = identity_demangle;
    DemangleFunctionT demangle_type = identity_demangle;
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    rmw_names_and_types_t names_and_types = rmw_get_zero_initialized_names_and_types();
    rmw_ret_t ret = graph_cache_.get_names_and_types(
      demangle_topic,
      demangle_type,
      &allocator,
      &names_and_types);
    if (RMW_RET_OK == ret) {
      ret = rmw_names_and_types_fini(&names_and_types);
      if (RMW_RET_OK != ret) {
        // If fault injection causes fini to fail, it should work the second time.
        EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_fini(&names_and_types));
      }
    } else {
      rcutils_reset_error();
    }
  });
}
