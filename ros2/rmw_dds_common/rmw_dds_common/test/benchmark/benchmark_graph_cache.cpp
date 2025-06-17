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

#include <string>
#include <vector>

#include "performance_test_fixture/performance_test_fixture.hpp"

#include "rmw/qos_profiles.h"

#include "rmw_dds_common/gid_utils.hpp"
#include "rmw_dds_common/graph_cache.hpp"

using performance_test_fixture::PerformanceTest;
using rmw_dds_common::GraphCache;

std::string
identity_demangle(const std::string & name)
{
  return name;
}

rmw_gid_t
gid_from_string(const std::string & str)
{
  rmw_gid_t gid = {};
  memcpy(
    reinterpret_cast<char *>(gid.data),
    str.c_str(),
    str.size() + 1);
  return gid;
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
    graph_cache.add_entity(
      gid_from_string(elem.gid),
      elem.name,
      elem.type,
      gid_from_string(elem.participant_gid),
      rmw_qos_profile_default,
      elem.is_reader);
  }
}

void
remove_entities(
  GraphCache & graph_cache,
  const std::vector<EntityInfo> & entities_info)
{
  for (const auto & elem : entities_info) {
    graph_cache.remove_entity(
      gid_from_string(elem.gid),
      elem.is_reader);
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

class TestGraphCache : public PerformanceTest
{
public:
  void SetUp(benchmark::State & st)
  {
    add_participants(graph_cache, {"participant1"});
    add_entities(
      graph_cache,
    {
      // topic1
      {"reader1", "participant1", "topic1", "Str", true},
      {"writer1", "participant1", "topic1", "Str", false},
    });
    add_nodes(
      graph_cache, {
      {"participant1", "ns1", "node1"},
      {"participant1", "ns1", "node2"},
      {"participant1", "ns2", "node1"}});
    performance_test_fixture::PerformanceTest::SetUp(st);
  }
  void TearDown(::benchmark::State & st)
  {
    performance_test_fixture::PerformanceTest::TearDown(st);
    remove_nodes(
      graph_cache, {
      {"participant1", "ns1", "node1"},
      {"participant1", "ns1", "node2"},
      {"participant1", "ns2", "node1"}});
    remove_entities(
      graph_cache,
    {
      // topic1
      {"reader1", "participant1", "topic1", "Str", true},
      {"writer1", "participant1", "topic1", "Str", false},
    });
    remove_participants(graph_cache, {"participant1"});
  }

protected:
  GraphCache graph_cache;
};

BENCHMARK_F(PerformanceTest, add_remove_participant_benchmark)(benchmark::State & st)
{
  GraphCache graph_cache;

  for (auto _ : st) {
    add_participants(graph_cache, {"participant1"});
    remove_participants(graph_cache, {"participant1"});
  }
}

BENCHMARK_F(PerformanceTest, add_remove_participant_and_node_benchmark)(benchmark::State & st)
{
  GraphCache graph_cache;

  reset_heap_counters();

  for (auto _ : st) {
    add_participants(graph_cache, {"participant1"});
    add_nodes(graph_cache, {{"participant1", "ns1", "node"}});
    remove_nodes(graph_cache, {{"participant1", "ns1", "node"}});
    remove_participants(graph_cache, {"participant1"});
  }
}

BENCHMARK_F(TestGraphCache, get_writers_info_by_topic_benchmark)(benchmark::State & st)
{
  rmw_topic_endpoint_info_array_t info = rmw_get_zero_initialized_topic_endpoint_info_array();
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  for (auto _ : st) {
    rmw_ret_t ret = graph_cache.get_writers_info_by_topic(
      "topic1",
      identity_demangle,
      &allocator,
      &info);
    if (ret != RMW_RET_OK) {
      st.SkipWithError("get_writers_info_by_topic failed");
    }
    ret = rmw_topic_endpoint_info_array_fini(&info, &allocator);
    if (ret != RMW_RET_OK) {
      st.SkipWithError("rmw_topic_endpoint_info_array_fini failed");
    }
  }
}

BENCHMARK_F(TestGraphCache, get_readers_info_by_topic_benchmark)(benchmark::State & st)
{
  rmw_topic_endpoint_info_array_t info = rmw_get_zero_initialized_topic_endpoint_info_array();
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  for (auto _ : st) {
    rmw_ret_t ret = graph_cache.get_readers_info_by_topic(
      "topic1",
      identity_demangle,
      &allocator,
      &info);
    if (ret != RMW_RET_OK) {
      st.SkipWithError("get_readers_info_by_topic failed");
    }
    ret = rmw_topic_endpoint_info_array_fini(&info, &allocator);
    if (ret != RMW_RET_OK) {
      st.SkipWithError("rmw_topic_endpoint_info_array_fini failed");
    }
  }
}

BENCHMARK_F(TestGraphCache, get_names_and_types_benchmark)(benchmark::State & st)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  for (auto _ : st) {
    rmw_names_and_types_t names_and_types = rmw_get_zero_initialized_names_and_types();
    rmw_ret_t ret = graph_cache.get_names_and_types(
      identity_demangle,
      identity_demangle,
      &allocator,
      &names_and_types);
    if (ret != RMW_RET_OK) {
      st.SkipWithError("get_names_and_types failed");
    }
    ret = rmw_names_and_types_fini(&names_and_types);
    if (ret != RMW_RET_OK) {
      st.SkipWithError("rmw_names_and_types_fini failed");
    }
  }
}

BENCHMARK_F(TestGraphCache, get_reader_names_and_types_by_node)(benchmark::State & st)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  for (auto _ : st) {
    rmw_names_and_types_t names_and_types = rmw_get_zero_initialized_names_and_types();
    rmw_ret_t ret = graph_cache.get_reader_names_and_types_by_node(
      "node1",
      "ns1",
      identity_demangle,
      identity_demangle,
      &allocator,
      &names_and_types);
    if (ret != RMW_RET_OK) {
      st.SkipWithError("get_reader_names_and_types_by_node failed");
    }
    ret = rmw_names_and_types_fini(&names_and_types);
    if (ret != RMW_RET_OK) {
      st.SkipWithError("rmw_names_and_types_fini failed");
    }
  }
}

BENCHMARK_F(TestGraphCache, get_writer_names_and_types_by_node)(benchmark::State & st)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  for (auto _ : st) {
    rmw_names_and_types_t names_and_types = rmw_get_zero_initialized_names_and_types();
    rmw_ret_t ret = graph_cache.get_writer_names_and_types_by_node(
      "node1",
      "ns1",
      identity_demangle,
      identity_demangle,
      &allocator,
      &names_and_types);
    if (ret != RMW_RET_OK) {
      st.SkipWithError("get_writer_names_and_types_by_node failed");
    }
    ret = rmw_names_and_types_fini(&names_and_types);
    if (ret != RMW_RET_OK) {
      st.SkipWithError("rmw_names_and_types_fini failed");
    }
  }
}

BENCHMARK_F(TestGraphCache, get_reader_count_benchmark)(benchmark::State & st)
{
  size_t count;
  for (auto _ : st) {
    rmw_ret_t ret = graph_cache.get_reader_count("reader1", &count);
    if (ret != RMW_RET_OK) {
      st.SkipWithError("get_reader_count failed");
    }
  }
}

BENCHMARK_F(TestGraphCache, get_writer_count_benchmark)(benchmark::State & st)
{
  size_t count;
  for (auto _ : st) {
    rmw_ret_t ret = graph_cache.get_writer_count("writer1", &count);
    if (ret != RMW_RET_OK) {
      st.SkipWithError("get_reader_count failed");
    }
  }
}

BENCHMARK_F(TestGraphCache, associate_entities_benchmark)(benchmark::State & st)
{
  for (auto _ : st) {
    // Associate entities
    associate_entities(
      graph_cache,
    {
      {"reader1", true, "participant1", "ns1", "node1"},
      {"writer1", false, "participant1", "ns1", "node2"},
    });
    dissociate_entities(
      graph_cache,
    {
      {"reader1", true, "participant1", "ns1", "node1"},
      {"writer1", false, "participant1", "ns1", "node2"},
    });
  }
}
