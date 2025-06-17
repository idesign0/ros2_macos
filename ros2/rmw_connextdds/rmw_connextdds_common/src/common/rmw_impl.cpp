// Copyright 2020 Real-Time Innovations, Inc. (RTI)
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

#include "rmw_connextdds/rmw_impl.hpp"

#include <algorithm>
#include <string>
#include <vector>
#include <stdexcept>

#include "rmw_dds_common/time_utils.hpp"

#include "rmw_connextdds/graph_cache.hpp"

#define ROS_SERVICE_REQUESTER_PREFIX_STR "rq"
#define ROS_SERVICE_RESPONSE_PREFIX_STR  "rr"

const char * const ROS_TOPIC_PREFIX = "rt";
const char * const ROS_SERVICE_REQUESTER_PREFIX = ROS_SERVICE_REQUESTER_PREFIX_STR;
const char * const ROS_SERVICE_RESPONSE_PREFIX = ROS_SERVICE_RESPONSE_PREFIX_STR;
const char * const ROS_CFT_TOPIC_NAME_INFIX = "_ContentFilterTopic";

std::string
rmw_connextdds_create_topic_name(
  const char * prefix,
  const char * topic_name,
  const char * suffix,
  bool avoid_ros_namespace_conventions)
{
  if (avoid_ros_namespace_conventions) {
    return std::string(topic_name) + std::string(suffix);
  } else {
    return std::string(prefix) +
           std::string(topic_name) +
           std::string(suffix);
  }
}

std::string
rmw_connextdds_create_topic_name(
  const char * prefix,
  const char * topic_name,
  const char * suffix,
  const rmw_qos_profile_t * qos_policies)
{
  return rmw_connextdds_create_topic_name(
    prefix,
    topic_name,
    suffix,
    qos_policies->avoid_ros_namespace_conventions);
}

rcutils_ret_t
rcutils_uint8_array_copy(
  rcutils_uint8_array_t * const dst,
  const rcutils_uint8_array_t * const src)
{
  if (src->buffer_length > 0) {
    if (src->buffer_length > dst->buffer_capacity) {
      rcutils_ret_t rc =
        rcutils_uint8_array_resize(dst, src->buffer_length);

      if (RCUTILS_RET_OK != rc) {
        return rc;
      }
    }

    dst->buffer_length = src->buffer_length;
    memcpy(dst->buffer, src->buffer, src->buffer_length);
  } else {
    dst->buffer_length = 0;
  }

  return RCUTILS_RET_OK;
}

static inline void str_trim(
  const char * const s,
  const size_t s_len,
  size_t * const s_start_out,
  size_t * const s_end_out)
{
  size_t s_i = 0;

  for (; s_i < s_len && std::isspace(s[s_i]); s_i++) {
  }
  *s_start_out = s_i;

  for (; s_i < s_len && !std::isspace(s[s_i]); s_i++) {
  }
  *s_end_out = s_i;
}

// This function "tokenizes" a string and stores the parsed tokens in a sequence.
// Note that that a trailing empty element is not supported (e.g. `"foo,bar,"`,
// or `"foo, bar,  "` if trimming is enabled), unless empty elements are allowed
// (in which, e.g., `"foo,,bar,"` would also be accepted).
rmw_ret_t
rmw_connextdds_parse_string_list(
  const char * const list,
  struct DDS_StringSeq * const parsed_out,
  const char delimiter,
  const bool trim_elements,
  const bool allow_empty_elements,
  const bool append_values)
{
  const size_t input_len = strlen(list);
  // This function expects to be called on a non-empty input string
  RMW_CONNEXT_ASSERT(input_len > 0)

  RMW_CONNEXT_LOG_TRACE_A(
    "parse list: "
    "delim=%c, trim_el=%d, empty_el=%d, append=%d, input='%s'",
    delimiter, trim_elements, allow_empty_elements, append_values, list)

  DDS_Long parsed_len = DDS_StringSeq_get_length(parsed_out);

  if (!append_values) {
    parsed_len = 0;
    if (!DDS_StringSeq_set_length(parsed_out, parsed_len)) {
      RMW_CONNEXT_LOG_ERROR_SET("failed to reset sequence length")
      return RMW_RET_ERROR;
    }
  }

  size_t input_i = 0,
    next_i_start = 0;
  for (;
    input_i < input_len;
    parsed_len += 1,
    input_i += 2,
    next_i_start = input_i)
  {
    // determine token's lenght by finding a delimiter (or end of input)
    for (;
      input_i + 1 < input_len && delimiter != list[input_i + 1];
      input_i += 1)
    {
    }

    RMW_CONNEXT_ASSERT(input_i >= next_i_start)
    RMW_CONNEXT_ASSERT(input_i < input_len)

    size_t next_len = input_i - next_i_start + 1;

    if (next_len > 0 && trim_elements) {
      size_t trim_head = 0,
        trim_tail = 0;
      str_trim(list + next_i_start, next_len, &trim_head, &trim_tail);
      next_i_start += trim_head;
      next_len = trim_tail - trim_head;
    }

    if (next_len == 0 && !allow_empty_elements) {
      RMW_CONNEXT_LOG_ERROR_A_SET("empty elements are not allowed: '%s'", list)
      return RMW_RET_ERROR;
    }

    if (!DDS_StringSeq_ensure_length(
        parsed_out, parsed_len + 1, parsed_len + 1))
    {
      RMW_CONNEXT_LOG_ERROR_SET("failed to resize string sequence")
      return RMW_RET_ERROR;
    }

    char ** const el_ref = DDS_StringSeq_get_reference(parsed_out, parsed_len);
    RMW_CONNEXT_ASSERT(nullptr != el_ref);
    if (nullptr != *el_ref) {
      DDS_String_free(*el_ref);
    }
    *el_ref = DDS_String_alloc(next_len);
    if (nullptr == el_ref) {
      RMW_CONNEXT_LOG_ERROR_SET("failed to allocate string")
      return RMW_RET_ERROR;
    }

    if (next_len > 0) {
      memcpy(*el_ref, list + next_i_start, next_len);
    }

    RMW_CONNEXT_LOG_TRACE_A(
      "parsed list element: i=%d, el='%s'",
      parsed_len, *el_ref)

    // check if we have a trailing empty element
    if (input_i + 2 == input_len) {
      if (!allow_empty_elements) {
        RMW_CONNEXT_LOG_ERROR_A_SET("empty elements are not allowed: '%s'", list)
        return RMW_RET_ERROR;
      }
    }
  }

  return RMW_RET_OK;
}

bool
rmw_connextdds_find_string_in_list(
  const DDS_StringSeq * const values,
  const char * const value)
{
  const DDS_Long values_len = DDS_StringSeq_get_length(values);
  for (DDS_Long i = 0; i < values_len; i++) {
    const char * const v_str =
      *DDS_StringSeq_get_reference(values, i);

    if (strcmp(v_str, value) == 0) {
      return true;
    }
  }
  return false;
}

DDS_Duration_t
rmw_connextdds_duration_from_ros_time(
  const rmw_time_t * const ros_time)
{
  if (rmw_time_equal(*ros_time, RMW_DURATION_INFINITE)) {
    return DDS_DURATION_INFINITE;
  }

  rmw_time_t in_time = rmw_dds_common::clamp_rmw_time_to_dds_time(*ros_time);

  DDS_Duration_t duration;
  duration.sec = static_cast<DDS_Long>(in_time.sec);
  duration.nanosec = static_cast<DDS_UnsignedLong>(in_time.nsec);
  return duration;
}

/******************************************************************************
 * Qos Helpers
 ******************************************************************************/
rmw_qos_policy_kind_t
dds_qos_policy_to_rmw_qos_policy(const DDS_QosPolicyId_t last_policy_id)
{
  switch (last_policy_id) {
    case DDS_DURABILITY_QOS_POLICY_ID:
      return RMW_QOS_POLICY_DURABILITY;
    case DDS_DEADLINE_QOS_POLICY_ID:
      return RMW_QOS_POLICY_DEADLINE;
    case DDS_LIVELINESS_QOS_POLICY_ID:
      return RMW_QOS_POLICY_LIVELINESS;
    case DDS_RELIABILITY_QOS_POLICY_ID:
      return RMW_QOS_POLICY_RELIABILITY;
    case DDS_HISTORY_QOS_POLICY_ID:
      return RMW_QOS_POLICY_HISTORY;
    case DDS_LIFESPAN_QOS_POLICY_ID:
      return RMW_QOS_POLICY_LIFESPAN;
    default:
      break;
  }
  return RMW_QOS_POLICY_INVALID;
}

static
DDS_Duration_t
rmw_time_to_dds_duration(const rmw_time_t & time)
{
  if (rmw_time_equal(time, RMW_DURATION_INFINITE)) {
    return DDS_DURATION_INFINITE;
  }
  // Use rmw_dds_common::clamp_rmw_time_to_dds_time() to make sure value doesn't
  // exceed the valid domain for DDS_Duration_t.
  const rmw_time_t ctime = rmw_dds_common::clamp_rmw_time_to_dds_time(time);
  DDS_Duration_t duration;
  duration.sec = static_cast<DDS_Long>(ctime.sec);
  duration.nanosec = static_cast<DDS_UnsignedLong>(ctime.nsec);
  return duration;
}

static
rmw_time_t
dds_duration_to_rmw_time(const DDS_Duration_t & duration)
{
  if (DDS_Duration_is_infinite(&duration)) {
    return RMW_DURATION_INFINITE;
  }
  assert(duration.sec > 0);
  rmw_time_t result = {static_cast<uint64_t>(duration.sec), duration.nanosec};
  return result;
}

rmw_ret_t
rmw_connextdds_get_readerwriter_qos(
  const bool writer_qos,
  RMW_Connext_MessageTypeSupport * const type_support,
  DDS_HistoryQosPolicy * const history,
  DDS_ReliabilityQosPolicy * const reliability,
  DDS_DurabilityQosPolicy * const durability,
  DDS_DeadlineQosPolicy * const deadline,
  DDS_LivelinessQosPolicy * const liveliness,
  DDS_ResourceLimitsQosPolicy * const resource_limits,
  DDS_PublishModeQosPolicy * const publish_mode,
  DDS_LifespanQosPolicy * const lifespan,
  const rmw_qos_profile_t * const qos_policies,
  const rmw_publisher_options_t * const pub_options,
  const rmw_subscription_options_t * const sub_options)
{
  UNUSED_ARG(writer_qos);
  UNUSED_ARG(type_support);
  UNUSED_ARG(publish_mode);
  UNUSED_ARG(resource_limits);
  UNUSED_ARG(pub_options);
  UNUSED_ARG(sub_options);

  switch (qos_policies->history) {
    case RMW_QOS_POLICY_HISTORY_SYSTEM_DEFAULT:
      {
        break;
      }
    case RMW_QOS_POLICY_HISTORY_KEEP_LAST:
      {
        if (qos_policies->depth == RMW_QOS_POLICY_DEPTH_SYSTEM_DEFAULT) {
          history->depth = 1;
          history->kind = DDS_KEEP_LAST_HISTORY_QOS;
        } else {
          if (qos_policies->depth < 1 || qos_policies->depth > INT32_MAX) {
            RMW_CONNEXT_LOG_ERROR_A_SET(
              "unsupported history depth: %ld", qos_policies->depth)
            return RMW_RET_ERROR;
          }

          history->depth = static_cast<DDS_Long>(qos_policies->depth);
          history->kind = DDS_KEEP_LAST_HISTORY_QOS;
        }
        break;
      }
    case RMW_QOS_POLICY_HISTORY_KEEP_ALL:
      {
        history->kind = DDS_KEEP_ALL_HISTORY_QOS;
        break;
      }
    // case RMW_QOS_POLICY_HISTORY_UNKNOWN:
    default:
      {
        RMW_CONNEXT_LOG_ERROR_A_SET(
          "unsupported history kind: %d", qos_policies->history)
        return RMW_RET_ERROR;
      }
  }

  RMW_CONNEXT_LOG_DEBUG_A(
    "endpoint resource history: "
    "kind=%d, "
    "depth=%d",
    history->kind,
    history->depth);

  reliability->max_blocking_time = DDS_DURATION_INFINITE;

  switch (qos_policies->reliability) {
    case RMW_QOS_POLICY_RELIABILITY_SYSTEM_DEFAULT:
      {
        break;
      }
    case RMW_QOS_POLICY_RELIABILITY_RELIABLE:
      {
        reliability->kind = DDS_RELIABLE_RELIABILITY_QOS;
        break;
      }
    case RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT:
      {
        reliability->kind = DDS_BEST_EFFORT_RELIABILITY_QOS;
        break;
      }
    // case RMW_QOS_POLICY_RELIABILITY_UNKNOWN:
    default:
      {
        RMW_CONNEXT_LOG_ERROR_A_SET(
          "unsupported reliability kind: %d", qos_policies->reliability)
        return RMW_RET_ERROR;
      }
  }

  switch (qos_policies->durability) {
    case RMW_QOS_POLICY_DURABILITY_SYSTEM_DEFAULT:
      {
        break;
      }
    case RMW_QOS_POLICY_DURABILITY_VOLATILE:
      {
        durability->kind = DDS_VOLATILE_DURABILITY_QOS;
        break;
      }
    case RMW_QOS_POLICY_DURABILITY_TRANSIENT_LOCAL:
      {
        durability->kind = DDS_TRANSIENT_LOCAL_DURABILITY_QOS;
        break;
      }
    // case RMW_QOS_POLICY_DURABILITY_UNKNOWN:
    default:
      {
        RMW_CONNEXT_LOG_ERROR_A_SET(
          "unsupported durability kind: %d", qos_policies->durability)
        return RMW_RET_ERROR;
      }
  }

  if (!rmw_time_equal(qos_policies->deadline, RMW_DURATION_UNSPECIFIED)) {
    deadline->period = rmw_time_to_dds_duration(qos_policies->deadline);
  }

  if (!rmw_time_equal(qos_policies->liveliness_lease_duration, RMW_DURATION_UNSPECIFIED)) {
    liveliness->lease_duration =
      rmw_time_to_dds_duration(qos_policies->liveliness_lease_duration);
  }

  switch (qos_policies->liveliness) {
    case RMW_QOS_POLICY_LIVELINESS_SYSTEM_DEFAULT:
      {
        break;
      }
    case RMW_QOS_POLICY_LIVELINESS_AUTOMATIC:
      {
        liveliness->kind = DDS_AUTOMATIC_LIVELINESS_QOS;
        break;
      }
    case RMW_QOS_POLICY_LIVELINESS_MANUAL_BY_TOPIC:
      {
        liveliness->kind = DDS_MANUAL_BY_TOPIC_LIVELINESS_QOS;
        break;
      }
    // case RMW_QOS_POLICY_LIVELINESS_UNKNOWN:
    default:
      {
        RMW_CONNEXT_LOG_ERROR_A_SET(
          "unsupported liveliness kind: %d", qos_policies->liveliness)
        return RMW_RET_ERROR;
      }
  }

  // LifespanQosPolicy is a writer-only policy, so `lifespan` might be NULL.
  // Micro does not support this policy, so the value will always be NULL.
#if RMW_CONNEXT_DDS_API == RMW_CONNEXT_DDS_API_MICRO
  assert(nullptr == lifespan);
#else /* RMW_CONNEXT_DDS_API == RMW_CONNEXT_DDS_API_PRO */
  if (lifespan != nullptr &&
    !rmw_time_equal(qos_policies->lifespan, RMW_DURATION_UNSPECIFIED))
  {
    // Guard access to type since it's not defined by Micro (only forward declared
    // by rmw_connextdds/dds_api_rtime.hpp)
    lifespan->duration = rmw_time_to_dds_duration(qos_policies->lifespan);
  }
#endif /* RMW_CONNEXT_DDS_API == RMW_CONNEXT_DDS_API_PRO */

  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_readerwriter_qos_to_ros(
  const DDS_HistoryQosPolicy * const history,
  const DDS_ReliabilityQosPolicy * const reliability,
  const DDS_DurabilityQosPolicy * const durability,
  const DDS_DeadlineQosPolicy * const deadline,
  const DDS_LivelinessQosPolicy * const liveliness,
  const DDS_LifespanQosPolicy * const lifespan,
  rmw_qos_profile_t * const qos_policies)
{
  if (nullptr != history) {
    switch (history->kind) {
      case DDS_KEEP_LAST_HISTORY_QOS:
        {
          qos_policies->history = RMW_QOS_POLICY_HISTORY_KEEP_LAST;
          break;
        }
      case DDS_KEEP_ALL_HISTORY_QOS:
        {
          qos_policies->history = RMW_QOS_POLICY_HISTORY_KEEP_ALL;
          break;
        }
      default:
        {
          RMW_CONNEXT_LOG_ERROR_A_SET(
            "invalid DDS history kind: %d", history->kind)
          return RMW_RET_ERROR;
        }
    }
    qos_policies->depth = (uint32_t) history->depth;
  }

  switch (reliability->kind) {
    case DDS_RELIABLE_RELIABILITY_QOS:
      {
        qos_policies->reliability = RMW_QOS_POLICY_RELIABILITY_RELIABLE;
        break;
      }
    case DDS_BEST_EFFORT_RELIABILITY_QOS:
      {
        qos_policies->reliability = RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT;
        break;
      }
    default:
      {
        RMW_CONNEXT_LOG_ERROR_A_SET(
          "invalid DDS reliability kind: %d", reliability->kind)
        return RMW_RET_ERROR;
      }
  }

  switch (durability->kind) {
    case DDS_VOLATILE_DURABILITY_QOS:
      {
        qos_policies->durability = RMW_QOS_POLICY_DURABILITY_VOLATILE;
        break;
      }
    case DDS_TRANSIENT_LOCAL_DURABILITY_QOS:
      {
        qos_policies->durability = RMW_QOS_POLICY_DURABILITY_TRANSIENT_LOCAL;
        break;
      }
    default:
      {
        RMW_CONNEXT_LOG_ERROR_A_SET(
          "invalid DDS durability kind: %d", durability->kind)
        return RMW_RET_ERROR;
      }
  }

  qos_policies->deadline = dds_duration_to_rmw_time(deadline->period);

  qos_policies->liveliness_lease_duration =
    dds_duration_to_rmw_time(liveliness->lease_duration);

  switch (liveliness->kind) {
    case DDS_AUTOMATIC_LIVELINESS_QOS:
      {
        qos_policies->liveliness = RMW_QOS_POLICY_LIVELINESS_AUTOMATIC;
        break;
      }
    case DDS_MANUAL_BY_TOPIC_LIVELINESS_QOS:
      {
        qos_policies->liveliness = RMW_QOS_POLICY_LIVELINESS_MANUAL_BY_TOPIC;
        break;
      }
    default:
      {
        RMW_CONNEXT_LOG_ERROR_A_SET(
          "invalid DDS liveliness kind: %d", liveliness->kind)
        return RMW_RET_ERROR;
      }
  }

  if (nullptr != lifespan) {
#if RMW_CONNEXT_DDS_API == RMW_CONNEXT_DDS_API_PRO
    qos_policies->lifespan = dds_duration_to_rmw_time(lifespan->duration);
#else
    // Only emit a debug message in this case, since we don't want to pollute the
    // output too much. We print a warning when going from ROS to DDS.
    RMW_CONNEXT_LOG_DEBUG("lifespan qos policy not supported")
#endif /* RMW_CONNEXT_DDS_API == RMW_CONNEXT_DDS_API_PRO */
  }

  return RMW_RET_OK;
}


rmw_ret_t
rmw_connextdds_datawriter_qos_to_ros(
  DDS_DataWriterQos * const qos,
  rmw_qos_profile_t * const qos_policies)
{
  return rmw_connextdds_readerwriter_qos_to_ros(
    &qos->history,
    &qos->reliability,
    &qos->durability,
    &qos->deadline,
    &qos->liveliness,
#if RMW_CONNEXT_DDS_API == RMW_CONNEXT_DDS_API_PRO
    &qos->lifespan,
#elif RMW_CONNEXT_DDS_API == RMW_CONNEXT_DDS_API_MICRO
    nullptr,
#endif /* RMW_CONNEXT_DDS_API == RMW_CONNEXT_DDS_API_PRO */
    qos_policies);
}


rmw_ret_t
rmw_connextdds_datareader_qos_to_ros(
  DDS_DataReaderQos * const qos,
  rmw_qos_profile_t * const qos_policies)
{
  return rmw_connextdds_readerwriter_qos_to_ros(
    &qos->history,
    &qos->reliability,
    &qos->durability,
    &qos->deadline,
    &qos->liveliness,
    nullptr /* Lifespan is a writer-only qos policy */,
    qos_policies);
}

/******************************************************************************
 * Node support
 ******************************************************************************/

RMW_Connext_Node *
RMW_Connext_Node::create(
  rmw_context_impl_t * const ctx)
{
  RMW_Connext_Node * node_impl =
    new (std::nothrow) RMW_Connext_Node(ctx);

  if (nullptr == node_impl) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to allocate node implementation")
    return nullptr;
  }

  return node_impl;
}

rmw_ret_t
RMW_Connext_Node::finalize()
{
  return RMW_RET_OK;
}

/******************************************************************************
 * Publisher Implementation functions
 ******************************************************************************/

RMW_Connext_Publisher::RMW_Connext_Publisher(
  rmw_context_impl_t * const ctx,
  DDS_DataWriter * const dds_writer,
  RMW_Connext_MessageTypeSupport * const type_support,
  const bool created_topic)
: ctx(ctx),
  dds_writer(dds_writer),
  type_support(type_support),
  created_topic(created_topic),
  status_condition(dds_writer)
{
  rmw_connextdds_get_entity_gid(this->dds_writer, this->ros_gid);
  if (RMW_RET_OK != this->status_condition.install(this)) {
    RMW_CONNEXT_LOG_ERROR("failed to install condition on writer")
    throw std::runtime_error("failed to install condition on writer");
  }
}

RMW_Connext_Publisher *
RMW_Connext_Publisher::create(
  rmw_context_impl_t * const ctx,
  DDS_DomainParticipant * const dp,
  DDS_Publisher * const pub,
  const rosidl_message_type_support_t * const type_supports,
  const char * const topic_name,
  const rmw_qos_profile_t * const qos_policies,
  const rmw_publisher_options_t * const publisher_options,
  const bool internal,
  const RMW_Connext_MessageType msg_type,
  const void * const intro_members,
  const bool intro_members_cpp,
  std::string * const type_name)
{
  UNUSED_ARG(internal);

  RMW_Connext_MessageTypeSupport * type_support =
    RMW_Connext_MessageTypeSupport::register_type_support(
    ctx,
    type_supports,
    dp,
    msg_type,
    intro_members,
    intro_members_cpp,
    type_name);

  if (nullptr == type_support) {
    RMW_CONNEXT_LOG_ERROR("failed to register type for writer")
    return nullptr;
  }

  auto scope_exit_type_unregister = rcpputils::make_scope_exit(
    [dp, type_support, ctx]()
    {
      if (RMW_RET_OK !=
      RMW_Connext_MessageTypeSupport::unregister_type_support(
        ctx, dp, type_support->type_name()))
      {
        RMW_CONNEXT_LOG_ERROR(
          "failed to unregister type for writer")
      }
      delete type_support;
    });

  std::string fqtopic_name;
  std::string prefix_rep(ROS_SERVICE_RESPONSE_PREFIX_STR "/");
  std::string prefix_req(ROS_SERVICE_REQUESTER_PREFIX_STR "/");
  std::string str_topic(topic_name);

  if (str_topic.find(prefix_rep) == 0 || str_topic.find(prefix_req) == 0) {
    fqtopic_name = str_topic;
  } else {
    fqtopic_name =
      rmw_connextdds_create_topic_name(
      ROS_TOPIC_PREFIX, topic_name, "", qos_policies);
  }

  DDS_Topic * topic = nullptr;
  bool topic_created = false;

  if (RMW_RET_OK !=
    ctx->assert_topic(
      dp,
      fqtopic_name.c_str(),
      type_support->type_name(),
      internal,
      &topic,
      topic_created))
  {
    RMW_CONNEXT_LOG_ERROR_A(
      "failed to assert topic: "
      "name=%s, "
      "type=%s",
      fqtopic_name.c_str(),
      type_support->type_name())
    return nullptr;
  }

  auto scope_exit_topic_delete = rcpputils::make_scope_exit(
    [topic_created, dp, topic]()
    {
      if (topic_created) {
        if (DDS_RETCODE_OK !=
        DDS_DomainParticipant_delete_topic(dp, topic))
        {
          RMW_CONNEXT_LOG_ERROR_SET(
            "failed to delete writer's topic")
        }
      }
    });

  // The following initialization generates warnings when built
  // with RTI Connext DDS Professional < 6 (e.g. 5.3.1), so use
  // DDS_DataWriterQos_initialize() for older versions.
#if !RMW_CONNEXT_DDS_API_PRO_LEGACY
  DDS_DataWriterQos dw_qos = DDS_DataWriterQos_INITIALIZER;
#else
  DDS_DataWriterQos dw_qos;
  if (DDS_RETCODE_OK != DDS_DataWriterQos_initialize(&dw_qos)) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to initialize datawriter qos")
    return nullptr;
  }
#endif /* !RMW_CONNEXT_DDS_API_PRO_LEGACY */

  DDS_DataWriterQos * const dw_qos_ptr = &dw_qos;
  auto scope_exit_dw_qos_delete =
    rcpputils::make_scope_exit(
    [dw_qos_ptr]()
    {
      if (DDS_RETCODE_OK != DDS_DataWriterQos_finalize(dw_qos_ptr)) {
        RMW_CONNEXT_LOG_ERROR_SET("failed to finalize DataWriterQoS")
      }
    });

  if (DDS_RETCODE_OK !=
    DDS_Publisher_get_default_datawriter_qos_w_topic_name(pub, &dw_qos, fqtopic_name.c_str()))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to get default writer QoS")
    return nullptr;
  }

  DDS_DataWriter * const dds_writer =
    rmw_connextdds_create_datawriter(
    ctx,
    dp,
    pub,
    qos_policies,
    publisher_options,
    internal,
    type_support,
    topic,
    &dw_qos);

  if (nullptr == dds_writer) {
    RMW_CONNEXT_LOG_ERROR("failed to create DDS writer")
    return nullptr;
  }

  auto scope_exit_dds_writer_delete =
    rcpputils::make_scope_exit(
    [pub, dds_writer]()
    {
      if (DDS_RETCODE_OK !=
      DDS_Publisher_delete_datawriter(pub, dds_writer))
      {
        RMW_CONNEXT_LOG_ERROR_SET(
          "failed to delete DDS DataWriter")
      }
    });

  RMW_Connext_Publisher * rmw_pub_impl =
    new (std::nothrow) RMW_Connext_Publisher(
    ctx, dds_writer, type_support, topic_created);

  if (nullptr == rmw_pub_impl) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to allocate RMW publisher")
    return nullptr;
  }

  scope_exit_type_unregister.cancel();
  scope_exit_topic_delete.cancel();
  scope_exit_dds_writer_delete.cancel();

  return rmw_pub_impl;
}

rmw_ret_t
RMW_Connext_Publisher::finalize()
{
  RMW_CONNEXT_LOG_DEBUG_A(
    "finalizing publisher: pub=%p, type=%s",
    (void *)this, this->type_support->type_name())

  // Make sure publisher's condition is detached from any waitset
  this->status_condition.invalidate();

  if (DDS_RETCODE_OK !=
    DDS_Publisher_delete_datawriter(
      this->dds_publisher(), this->dds_writer))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to delete DDS DataWriter")
    return RMW_RET_ERROR;
  }

  DDS_DomainParticipant * const participant = this->dds_participant();

  if (this->created_topic) {
    DDS_Topic * const topic = this->dds_topic();

    RMW_CONNEXT_LOG_DEBUG_A(
      "deleting topic: name=%s",
      DDS_TopicDescription_get_name(
        DDS_Topic_as_topicdescription(topic)))

    DDS_ReturnCode_t rc =
      DDS_DomainParticipant_delete_topic(participant, topic);

    if (DDS_RETCODE_OK != rc) {
      RMW_CONNEXT_LOG_ERROR_SET("failed to delete DDS Topic")
      return RMW_RET_ERROR;
    }
  }

  rmw_ret_t rc = RMW_Connext_MessageTypeSupport::unregister_type_support(
    this->ctx, participant, this->type_support->type_name());

  if (RMW_RET_OK != rc) {
    return rc;
  }

  delete this->type_support;
  this->type_support = nullptr;

  return RMW_RET_OK;
}

#ifndef DDS_GUID_INITIALIZER
#define DDS_GUID_INITIALIZER        DDS_GUID_DEFAULT
#endif /* DDS_GUID_INITIALIZER */

rmw_ret_t
RMW_Connext_Publisher::requestreply_header_to_dds(
  const RMW_Connext_RequestReplyMessage * const rr_msg,
  DDS_SampleIdentity_t * const sample_identity,
  DDS_SampleIdentity_t * const related_sample_identity)
{
  struct DDS_GUID_t src_guid = DDS_GUID_INITIALIZER;
  struct DDS_SequenceNumber_t src_sn = DDS_SEQUENCE_NUMBER_UNKNOWN;

  rmw_ret_t rc = RMW_RET_ERROR;
  rc = rmw_connextdds_gid_to_guid(rr_msg->gid, src_guid);
  if (RMW_RET_OK != rc) {
    return rc;
  }

  rmw_connextdds_sn_ros_to_dds(rr_msg->sn, src_sn);

  if (rr_msg->request) {
    sample_identity->writer_guid = src_guid;
    sample_identity->sequence_number = src_sn;
  } else {
    related_sample_identity->writer_guid = src_guid;
    related_sample_identity->sequence_number = src_sn;
  }

  return RMW_RET_OK;
}


rmw_ret_t
RMW_Connext_Publisher::write(
  const void * const ros_message,
  const bool serialized,
  int64_t * const sn_out)
{
  RMW_Connext_Message user_msg;
  if (RMW_RET_OK != RMW_Connext_Message_initialize(&user_msg, this->type_support, 0)) {
    return RMW_RET_ERROR;
  }
  user_msg.user_data = ros_message;
  user_msg.serialized = serialized;

  return rmw_connextdds_write_message(this, &user_msg, sn_out);
}


size_t
RMW_Connext_Publisher::subscriptions_count()
{
  DDS_PublicationMatchedStatus status =
    DDS_PublicationMatchedStatus_INITIALIZER;

  if (DDS_RETCODE_OK !=
    DDS_DataWriter_get_publication_matched_status(
      this->dds_writer, &status))
  {
    RMW_CONNEXT_LOG_ERROR_SET(
      "failed to get publication matched status")
    return 0;
  }

  return status.current_count;
}

rmw_ret_t
RMW_Connext_Publisher::assert_liveliness()
{
  if (DDS_RETCODE_OK !=
    DDS_DataWriter_assert_liveliness(this->dds_writer))
  {
    RMW_CONNEXT_LOG_ERROR_SET(
      "failed to assert writer liveliness")
    return RMW_RET_ERROR;
  }

  return RMW_RET_OK;
}

rmw_ret_t
RMW_Connext_Publisher::wait_for_all_acked(rmw_time_t wait_timeout)
{
  DDS_Duration_t timeout = rmw_connextdds_duration_from_ros_time(&wait_timeout);
#if RMW_CONNEXT_DDS_API == RMW_CONNEXT_DDS_API_MICRO
  // Avoid warnings for unused variable in micro, since wait_for_ack() is
  // mapped to an empty call.
  UNUSED_ARG(timeout);
#endif  // RMW_CONNEXT_DDS_API == RMW_CONNEXT_DDS_API_MICRO

  const DDS_ReturnCode_t dds_rc =
    DDS_DataWriter_wait_for_acknowledgments(this->dds_writer, &timeout);

  switch (dds_rc) {
    case DDS_RETCODE_OK:
      return RMW_RET_OK;
    case DDS_RETCODE_TIMEOUT:
      return RMW_RET_TIMEOUT;
    default:
      {
        RMW_CONNEXT_LOG_ERROR_A_SET(
          "failed to wait for reader acknowledgements: dds_rc=%d", dds_rc)
        return RMW_RET_ERROR;
      }
  }
}

rmw_ret_t
RMW_Connext_Publisher::qos(rmw_qos_profile_t * const qos)
{
  // The following initialization generates warnings when built
  // with RTI Connext DDS Professional < 6 (e.g. 5.3.1), so use
  // DDS_DataWriterQos_initialize() for older versions.
#if !RMW_CONNEXT_DDS_API_PRO_LEGACY
  DDS_DataWriterQos dw_qos = DDS_DataWriterQos_INITIALIZER;
#else
  DDS_DataWriterQos dw_qos;
  if (DDS_RETCODE_OK != DDS_DataWriterQos_initialize(&dw_qos)) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to initialize datawriter qos")
    return RMW_RET_ERROR;
  }
#endif /* !RMW_CONNEXT_DDS_API_PRO_LEGACY */

  if (DDS_RETCODE_OK != DDS_DataWriter_get_qos(this->dds_writer, &dw_qos)) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to get DDS writer's qos")
    return RMW_RET_ERROR;
  }

  rmw_ret_t rc = rmw_connextdds_datawriter_qos_to_ros(&dw_qos, qos);

  DDS_DataWriterQos_finalize(&dw_qos);
  return rc;
}

rmw_publisher_t *
rmw_connextdds_create_publisher(
  rmw_context_impl_t * const ctx,
  const rmw_node_t * const node,
  DDS_DomainParticipant * const dp,
  DDS_Publisher * const pub,
  const rosidl_message_type_support_t * const type_supports,
  const char * const topic_name,
  const rmw_qos_profile_t * const qos_policies,
  const rmw_publisher_options_t * const publisher_options,
  const bool internal)
{
  std::lock_guard<std::mutex> guard(ctx->endpoint_mutex);
  RMW_Connext_Publisher * rmw_pub_impl =
    RMW_Connext_Publisher::create(
    ctx,
    dp,
    pub,
    type_supports,
    topic_name,
    qos_policies,
    publisher_options,
    internal);

  if (nullptr == rmw_pub_impl) {
    RMW_CONNEXT_LOG_ERROR(
      "failed to allocate RMW_Connext_Publisher")
    return nullptr;
  }

  auto scope_exit_rmw_writer_impl_delete =
    rcpputils::make_scope_exit(
    [rmw_pub_impl]()
    {
      if (RMW_RET_OK != rmw_pub_impl->finalize()) {
        RMW_CONNEXT_LOG_ERROR(
          "failed to finalize RMW_Connext_Publisher")
      }
      delete rmw_pub_impl;
    });

  rmw_publisher_t * rmw_publisher = rmw_publisher_allocate();
  if (nullptr == rmw_publisher) {
    RMW_CONNEXT_LOG_ERROR_SET(
      "failed to allocate RMW publisher")
    return nullptr;
  }
  rmw_publisher->topic_name = nullptr;

  auto scope_exit_rmw_writer_delete = rcpputils::make_scope_exit(
    [rmw_publisher]() {
      if (nullptr != rmw_publisher->topic_name) {
        rmw_free(const_cast<char *>(rmw_publisher->topic_name));
      }
      rmw_publisher_free(rmw_publisher);
    });

  size_t topic_name_len = strlen(topic_name);

  rmw_publisher->implementation_identifier = RMW_CONNEXTDDS_ID;
  rmw_publisher->data = rmw_pub_impl;
  rmw_publisher->topic_name =
    reinterpret_cast<char *>(rmw_allocate(topic_name_len + 1));
  if (nullptr == rmw_publisher->topic_name) {
    RMW_CONNEXT_LOG_ERROR_SET(
      "failed to allocate publisher's topic name")
    return nullptr;
  }
  memcpy(
    const_cast<char *>(rmw_publisher->topic_name),
    topic_name,
    topic_name_len + 1);
  rmw_publisher->options = *publisher_options;
  rmw_publisher->can_loan_messages = false;

  if (!internal) {
    if (RMW_RET_OK != rmw_pub_impl->enable()) {
      RMW_CONNEXT_LOG_ERROR("failed to enable publisher")
      return nullptr;
    }

    if (RMW_RET_OK !=
      rmw_connextdds_graph_on_publisher_created(
        ctx, node, rmw_pub_impl))
    {
      RMW_CONNEXT_LOG_ERROR("failed to update graph for publisher")
      return nullptr;
    }
  }


  scope_exit_rmw_writer_impl_delete.cancel();
  scope_exit_rmw_writer_delete.cancel();
  return rmw_publisher;
}

rmw_ret_t
rmw_connextdds_destroy_publisher(
  rmw_context_impl_t * const ctx,
  rmw_publisher_t * const rmw_publisher)
{
  std::lock_guard<std::mutex> guard(ctx->endpoint_mutex);
  UNUSED_ARG(ctx);

  RMW_Connext_Publisher * const rmw_pub_impl =
    static_cast<RMW_Connext_Publisher *>(rmw_publisher->data);

  if (nullptr == rmw_pub_impl) {
    RMW_CONNEXT_LOG_ERROR_SET("invalid publisher data")
    return RMW_RET_ERROR;
  }

  rmw_ret_t rc = rmw_pub_impl->finalize();
  if (RMW_RET_OK != rc) {
    return rc;
  }

  delete rmw_pub_impl;
  rmw_free(const_cast<char *>(rmw_publisher->topic_name));
  rmw_publisher_free(rmw_publisher);

  return RMW_RET_OK;
}


/******************************************************************************
 * Subscriber Implementation functions
 ******************************************************************************/

RMW_Connext_Subscriber::RMW_Connext_Subscriber(
  rmw_context_impl_t * const ctx,
  DDS_DataReader * const dds_reader,
  DDS_Topic * const dds_topic,
  RMW_Connext_MessageTypeSupport * const type_support,
  const bool ignore_local,
  const bool created_topic,
  DDS_TopicDescription * const dds_topic_cft,
  const char * const cft_expression,
  const bool internal)
: internal(internal),
  ignore_local(ignore_local),
  ctx(ctx),
  dds_reader(dds_reader),
  dds_topic(dds_topic),
  dds_topic_cft(dds_topic_cft),
  cft_expression(cft_expression),
  type_support(type_support),
  created_topic(created_topic),
  status_condition(dds_reader, ignore_local, internal)
{
  rmw_connextdds_get_entity_gid(this->dds_reader, this->ros_gid);

  RMW_Connext_UntypedSampleSeq def_data_seq =
    RMW_Connext_UntypedSampleSeq_INITIALIZER;
  DDS_SampleInfoSeq def_info_seq = DDS_SEQUENCE_INITIALIZER;
  this->loan_data = def_data_seq;
  this->loan_info = def_info_seq;
  this->loan_len = 0;
  this->loan_next = 0;
  if (RMW_RET_OK != this->status_condition.install(this)) {
    RMW_CONNEXT_LOG_ERROR("failed to install condition on reader")
    throw std::runtime_error("failed to install condition on reader");
  }
}

RMW_Connext_Subscriber *
RMW_Connext_Subscriber::create(
  rmw_context_impl_t * const ctx,
  DDS_DomainParticipant * const dp,
  DDS_Subscriber * const sub,
  const rosidl_message_type_support_t * const type_supports,
  const char * const topic_name,
  const rmw_qos_profile_t * const qos_policies,
  const rmw_subscription_options_t * const subscriber_options,
  const bool internal,
  const RMW_Connext_MessageType msg_type,
  const void * const intro_members,
  const bool intro_members_cpp,
  std::string * const type_name,
  const char * const cft_name,
  const char * const cft_filter)
{
  RMW_Connext_MessageTypeSupport * const type_support =
    RMW_Connext_MessageTypeSupport::register_type_support(
    ctx,
    type_supports,
    dp,
    msg_type,
    intro_members,
    intro_members_cpp,
    type_name);

  if (nullptr == type_support) {
    RMW_CONNEXT_LOG_ERROR("failed to register type for reader")
    return nullptr;
  }

  auto scope_exit_type_unregister = rcpputils::make_scope_exit(
    [dp, type_support, ctx]()
    {
      if (RMW_RET_OK !=
      RMW_Connext_MessageTypeSupport::unregister_type_support(
        ctx, dp, type_support->type_name()))
      {
        RMW_CONNEXT_LOG_ERROR(
          "failed to unregister type for writer")
      }
      delete type_support;
    });

  std::string fqtopic_name;
  std::string prefix_rep(ROS_SERVICE_RESPONSE_PREFIX_STR "/");
  std::string prefix_req(ROS_SERVICE_REQUESTER_PREFIX_STR "/");
  std::string str_topic(topic_name);

  if (str_topic.find(prefix_rep) == 0 || str_topic.find(prefix_req) == 0) {
    fqtopic_name = str_topic;
  } else {
    fqtopic_name =
      rmw_connextdds_create_topic_name(
      ROS_TOPIC_PREFIX, topic_name, "", qos_policies);
  }

  DDS_Topic * topic = nullptr;
  DDS_TopicDescription * cft_topic = nullptr;
  bool topic_created = false;

  if (RMW_RET_OK !=
    ctx->assert_topic(
      dp,
      fqtopic_name.c_str(),
      type_support->type_name(),
      internal,
      &topic,
      topic_created))
  {
    RMW_CONNEXT_LOG_ERROR_A(
      "failed to assert topic: "
      "name=%s, "
      "type=%s",
      fqtopic_name.c_str(),
      type_support->type_name())
    return nullptr;
  }

  auto scope_exit_topic_delete = rcpputils::make_scope_exit(
    [ctx, &topic_created, dp, &topic, &cft_topic]()
    {
      if (nullptr != cft_topic) {
        if (RMW_RET_OK !=
        rmw_connextdds_delete_contentfilteredtopic(ctx, dp, cft_topic))
        {
          RMW_CONNEXT_LOG_ERROR("failed to delete content-filtered topic")
        }
      }
      if (topic_created) {
        if (DDS_RETCODE_OK !=
        DDS_DomainParticipant_delete_topic(dp, topic))
        {
          RMW_CONNEXT_LOG_ERROR_SET(
            "failed to delete reader's topic")
        }
      }
    });

  DDS_TopicDescription * sub_topic = DDS_Topic_as_topicdescription(topic);
  std::string sub_cft_name;
  const char * sub_cft_expr = "";
  const rcutils_string_array_t * sub_cft_params = nullptr;

  if (nullptr != cft_name) {
    sub_cft_name = cft_name;
    sub_cft_expr = cft_filter;
  } else {
    sub_cft_name =
      fqtopic_name + ROS_CFT_TOPIC_NAME_INFIX + RMW_Connext_Subscriber::get_atomic_id();
    if (nullptr != subscriber_options->content_filter_options) {
      sub_cft_expr =
        subscriber_options->content_filter_options->filter_expression;
      sub_cft_params =
        &subscriber_options->content_filter_options->expression_parameters;
    }
  }

  rmw_ret_t cft_rc =
    rmw_connextdds_create_contentfilteredtopic(
    ctx,
    dp,
    topic,
    sub_cft_name.c_str(),
    sub_cft_expr,
    sub_cft_params,
    &cft_topic);

  if (RMW_RET_OK != cft_rc) {
    if (RMW_RET_UNSUPPORTED != cft_rc) {
      return nullptr;
    }
  } else {
    sub_topic = cft_topic;
  }

  // The following initialization generates warnings when built
  // with RTI Connext DDS Professional < 6 (e.g. 5.3.1), so use
  // DDS_DataWriterQos_initialize() for older versions.
#if !RMW_CONNEXT_DDS_API_PRO_LEGACY
  DDS_DataReaderQos dr_qos = DDS_DataReaderQos_INITIALIZER;
#else
  DDS_DataReaderQos dr_qos;
  if (DDS_RETCODE_OK != DDS_DataReaderQos_initialize(&dr_qos)) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to initialize datareader qos")
    return nullptr;
  }
#endif /* !RMW_CONNEXT_DDS_API_PRO_LEGACY */

  DDS_DataReaderQos * const dr_qos_ptr = &dr_qos;
  auto scope_exit_dr_qos_delete =
    rcpputils::make_scope_exit(
    [dr_qos_ptr]()
    {
      DDS_DataReaderQos_finalize(dr_qos_ptr);
    });

  if (DDS_RETCODE_OK !=
    DDS_Subscriber_get_default_datareader_qos_w_topic_name(sub, &dr_qos, fqtopic_name.c_str()))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to get default reader QoS")
    return nullptr;
  }

  DDS_DataReader * dds_reader =
    rmw_connextdds_create_datareader(
    ctx,
    dp,
    sub,
    qos_policies,
    subscriber_options,
    internal,
    type_support,
    sub_topic,
    &dr_qos);

  if (nullptr == dds_reader) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to create DDS reader")
    return nullptr;
  }

  auto scope_exit_dds_reader_delete =
    rcpputils::make_scope_exit(
    [sub, dds_reader]()
    {
      if (DDS_RETCODE_OK !=
      DDS_Subscriber_delete_datareader(sub, dds_reader))
      {
        RMW_CONNEXT_LOG_ERROR_SET(
          "failed to delete DDS DataWriter")
      }
    });

  RMW_Connext_Subscriber * rmw_sub_impl =
    new (std::nothrow) RMW_Connext_Subscriber(
    ctx,
    dds_reader,
    topic,
    type_support,
    subscriber_options->ignore_local_publications,
    topic_created,
    cft_topic,
    sub_cft_expr,
    internal);

  if (nullptr == rmw_sub_impl) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to allocate RMW subscriber")
    return nullptr;
  }
  scope_exit_dds_reader_delete.cancel();
  scope_exit_topic_delete.cancel();
  scope_exit_type_unregister.cancel();

  return rmw_sub_impl;
}

rmw_ret_t
RMW_Connext_Subscriber::finalize()
{
  RMW_CONNEXT_LOG_DEBUG_A(
    "finalizing subscriber: sub=%p, type=%s",
    (void *)this, this->type_support->type_name())

  // Make sure subscriber's condition is detached from any waitset
  this->status_condition.invalidate();

  if (this->loan_len > 0) {
    this->loan_next = this->loan_len;
    if (RMW_RET_OK != this->return_messages()) {
      return RMW_RET_ERROR;
    }
  }

  if (DDS_RETCODE_OK !=
    DDS_Subscriber_delete_datareader(
      this->dds_subscriber(), this->dds_reader))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to delete DDS DataReader")
    return RMW_RET_ERROR;
  }

  DDS_DomainParticipant * const participant = this->dds_participant();

  if (nullptr != this->dds_topic_cft) {
    rmw_ret_t cft_rc = rmw_connextdds_delete_contentfilteredtopic(
      ctx, participant, this->dds_topic_cft);

    if (RMW_RET_OK != cft_rc) {
      return cft_rc;
    }
  }

  if (this->created_topic) {
    DDS_Topic * const topic = this->dds_topic;

    RMW_CONNEXT_LOG_DEBUG_A(
      "deleting topic: name=%s",
      DDS_TopicDescription_get_name(
        DDS_Topic_as_topicdescription(topic)))

    DDS_ReturnCode_t rc =
      DDS_DomainParticipant_delete_topic(participant, topic);

    if (DDS_RETCODE_OK != rc) {
      RMW_CONNEXT_LOG_ERROR_SET("failed to delete DDS Topic")
      return RMW_RET_ERROR;
    }
  }

  rmw_ret_t rc = RMW_Connext_MessageTypeSupport::unregister_type_support(
    this->ctx, participant, this->type_support->type_name());

  if (RMW_RET_OK != rc) {
    return rc;
  }

  delete this->type_support;
  this->type_support = nullptr;

  return RMW_RET_OK;
}

size_t
RMW_Connext_Subscriber::publications_count()
{
  DDS_SubscriptionMatchedStatus status =
    DDS_SubscriptionMatchedStatus_INITIALIZER;

  if (DDS_RETCODE_OK !=
    DDS_DataReader_get_subscription_matched_status(
      this->dds_reader, &status))
  {
    RMW_CONNEXT_LOG_ERROR_SET(
      "failed to get subscription matched status")
    return 0;
  }

  return status.current_count;
}


rmw_ret_t
RMW_Connext_Subscriber::qos(rmw_qos_profile_t * const qos)
{
  // The following initialization generates warnings when built
  // with RTI Connext DDS Professional < 6 (e.g. 5.3.1), so use
  // DDS_DataWriterQos_initialize() for older versions.
#if !RMW_CONNEXT_DDS_API_PRO_LEGACY
  DDS_DataReaderQos dr_qos = DDS_DataReaderQos_INITIALIZER;
#else
  DDS_DataReaderQos dr_qos;
  if (DDS_RETCODE_OK != DDS_DataReaderQos_initialize(&dr_qos)) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to initialize datareader qos")
    return RMW_RET_ERROR;
  }
#endif /* !RMW_CONNEXT_DDS_API_PRO_LEGACY */

  if (DDS_RETCODE_OK != DDS_DataReader_get_qos(this->dds_reader, &dr_qos)) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to get DDS reader's qos")
    return RMW_RET_ERROR;
  }

  rmw_ret_t rc = rmw_connextdds_datareader_qos_to_ros(&dr_qos, qos);

  DDS_DataReaderQos_finalize(&dr_qos);
  return rc;
}

rmw_ret_t
RMW_Connext_Subscriber::take_message(
  void * const ros_message,
  rmw_message_info_t * const message_info,
  bool * const taken,
  const DDS_InstanceHandle_t * const request_writer_handle)
{
  *taken = false;
  size_t taken_count = 0;
  void * ros_messages[1];
  ros_messages[0] = ros_message;
  rmw_ret_t rc = this->take_next(
    ros_messages,
    message_info,
    1,
    &taken_count,
    false /* serialized*/,
    request_writer_handle);
  if (RMW_RET_OK == rc) {
    *taken = taken_count > 0;
  }
  return rc;
}

rmw_ret_t
RMW_Connext_Subscriber::take(
  rmw_message_sequence_t * const message_sequence,
  rmw_message_info_sequence_t * const message_info_sequence,
  const size_t max_samples,
  size_t * const taken)
{
  if (max_samples == 0 ||
    message_sequence->capacity < max_samples ||
    message_info_sequence->capacity != message_sequence->capacity)
  {
    return RMW_RET_INVALID_ARGUMENT;
  }
  return this->take_next(
    message_sequence->data,
    message_info_sequence->data,
    max_samples,
    taken,
    false /* serialized*/);
}

rmw_ret_t
RMW_Connext_Subscriber::take_serialized(
  rmw_serialized_message_t * const serialized_message,
  rmw_message_info_t * const message_info,
  bool * const taken)
{
  *taken = false;
  size_t taken_count = 0;
  void * ros_messages[1];
  ros_messages[0] = serialized_message;
  rmw_ret_t rc = this->take_next(
    ros_messages,
    message_info,
    1,
    &taken_count,
    true /* serialized*/);
  if (RMW_RET_OK == rc) {
    *taken = taken_count > 0;
  }
  return rc;
}


rmw_ret_t
RMW_Connext_Subscriber::set_content_filter(
  const rmw_subscription_content_filter_options_t * const options)
{
  if (RMW_RET_OK !=
    rmw_connextdds_set_cft_filter_expression(
      this->dds_topic_cft, options->filter_expression, &options->expression_parameters))
  {
    return RMW_RET_ERROR;
  }

  this->cft_expression = options->filter_expression;
  return RMW_RET_OK;
}

rmw_ret_t
RMW_Connext_Subscriber::get_content_filter(
  rcutils_allocator_t * allocator,
  rmw_subscription_content_filter_options_t * const options)
{
  return rmw_connextdds_get_cft_filter_expression(this->dds_topic_cft, allocator, options);
}

rmw_ret_t
RMW_Connext_Subscriber::loan_messages(const bool update_condition)
{
  /* this function should only be called once all previously
     loaned messages have been returned */
  RMW_CONNEXT_ASSERT(this->loan_len == 0)
  RMW_CONNEXT_ASSERT(this->loan_next == 0)

  if (RMW_RET_OK != rmw_connextdds_take_samples(this)) {
    return RMW_RET_ERROR;
  }

  this->loan_len = DDS_UntypedSampleSeq_get_length(&this->loan_data);

  RMW_CONNEXT_LOG_TRACE_A(
    "[%s] loaned messages: %lu",
    this->type_support->type_name(), this->loan_len)

  if (update_condition) {
    return this->status_condition.set_data_available(this->loan_len > 0);
  } else {
    return RMW_RET_OK;
  }
}

rmw_ret_t
RMW_Connext_Subscriber::return_messages()
{
  /* this function should be called only if a loan is available */
  RMW_CONNEXT_ASSERT(this->loan_len > 0)

  RMW_CONNEXT_LOG_TRACE_A(
    "[%s] return loaned messages: %lu",
    this->type_support->type_name(), this->loan_len)

  this->loan_len = 0;
  this->loan_next = 0;

  rmw_ret_t rc_result = RMW_RET_OK;
  rmw_ret_t rc = rmw_connextdds_return_samples(this);
  if (RMW_RET_OK != rc) {
    rc_result = rc;
  }

  rc = this->status_condition.set_data_available(false);
  if (RMW_RET_OK != rc) {
    rc_result = rc;
  }

  return rc_result;
}

void
RMW_Connext_Subscriber::requestreply_header_from_dds(
  RMW_Connext_RequestReplyMessage * const rr_msg,
  const DDS_SampleIdentity_t * const sample_identity,
  const DDS_SampleIdentity_t * const related_sample_identity)
{
  const struct DDS_GUID_t * src_guid = nullptr;
  const struct DDS_SequenceNumber_t * src_sn = nullptr;

  if (rr_msg->request) {
    src_guid = &sample_identity->writer_guid;
    src_sn = &sample_identity->sequence_number;
  } else {
    src_guid = &related_sample_identity->writer_guid;
    src_sn = &related_sample_identity->sequence_number;
  }

  rmw_connextdds_guid_to_gid(*src_guid, rr_msg->gid);
  rmw_connextdds_sn_dds_to_ros(*src_sn, rr_msg->sn);
}

rmw_ret_t
RMW_Connext_Subscriber::take_next(
  void ** const ros_messages,
  rmw_message_info_t * const message_infos,
  const size_t max_samples,
  size_t * const taken,
  const bool serialized,
  const DDS_InstanceHandle_t * const request_writer_handle)
{
  rmw_ret_t rc = RMW_RET_OK,
    rc_exit = RMW_RET_OK;

  *taken = 0;

  std::lock_guard<std::mutex> lock(this->loan_mutex);

  while (*taken < max_samples) {
    rc = this->loan_messages_if_needed();
    if (RMW_RET_OK != rc) {
      return rc;
    }

    if (this->loan_len == 0) {
      /* no data available on reader */
      return RMW_RET_OK;
    }

    for (; *taken < max_samples &&
      this->loan_next < this->loan_len &&
      RMW_RET_OK == rc_exit;
      this->loan_next++)
    {
      RMW_Connext_Message * msg =
        reinterpret_cast<RMW_Connext_Message *>(
        DDS_UntypedSampleSeq_get_reference(
          &this->loan_data, static_cast<DDS_Long>(this->loan_next)));
      DDS_SampleInfo * info =
        DDS_SampleInfoSeq_get_reference(
        &this->loan_info, static_cast<DDS_Long>(this->loan_next));
      void * ros_message = ros_messages[*taken];

      if (info->valid_data) {
        bool accepted = false;

        // If this is a request/reply message, and we are in "compatibility mode",
        // then we must always call deserialize() to at least retrieve the
        // request header.
        if (this->type_support->type_requestreply()) {
          if (this->ctx->request_reply_mapping == RMW_Connext_RequestReplyMapping::Basic) {
            size_t deserialized_size = 0;
            UNUSED_ARG(deserialized_size);

            if (RMW_RET_OK !=
              this->type_support->deserialize(
                ros_message, &msg->data_buffer, deserialized_size, true /* header_only */))
            {
              RMW_CONNEXT_LOG_ERROR_SET("failed to deserialize taken sample")
              rc_exit = RMW_RET_ERROR;
              continue;
            }
          } else {
            RMW_Connext_RequestReplyMessage * const rr_msg =
              reinterpret_cast<RMW_Connext_RequestReplyMessage *>(ros_message);

            DDS_SampleIdentity_t identity,
              related_sample_identity;

            DDS_SampleInfo_get_sample_identity(info, &identity);
            DDS_SampleInfo_get_related_sample_identity(
              info, &related_sample_identity);

            this->requestreply_header_from_dds(
              rr_msg, &identity, &related_sample_identity);
          }
        }

        if (RMW_RET_OK != rmw_connextdds_filter_sample(
            this, ros_message, info, request_writer_handle, &accepted))
        {
          RMW_CONNEXT_LOG_ERROR_SET("failed to filter received sample")
          rc_exit = RMW_RET_ERROR;
          continue;
        }
        if (!accepted) {
          RMW_CONNEXT_LOG_TRACE_A(
            "[%s] DROPPED message", this->type_support->type_name())
          continue;
        }

        if (serialized) {
          if (RCUTILS_RET_OK !=
            rcutils_uint8_array_copy(
              reinterpret_cast<rcutils_uint8_array_t *>(ros_message),
              &msg->data_buffer))
          {
            RMW_CONNEXT_LOG_ERROR_SET("failed to copy uint8 array")
            rc_exit = RMW_RET_ERROR;
            continue;
          }
        } else {
          size_t deserialized_size = 0;

          if (RMW_RET_OK !=
            this->type_support->deserialize(
              ros_message, &msg->data_buffer, deserialized_size))
          {
            RMW_CONNEXT_LOG_ERROR_SET(
              "failed to deserialize taken sample")
            rc_exit = RMW_RET_ERROR;
            continue;
          }
        }

        if (nullptr != message_infos) {
          rmw_message_info_t * message_info = &message_infos[*taken];
          rmw_connextdds_message_info_from_dds(message_info, info);
        }

        *taken += 1;
        continue;
      }
    }
  }
  RMW_CONNEXT_LOG_DEBUG_A(
    "[%s] taken messages: %lu",
    this->type_support->type_name(), *taken)

  if (this->loan_len && this->loan_next >= this->loan_len) {
    rc = this->return_messages();
    if (RMW_RET_OK != rc) {
      RMW_CONNEXT_LOG_ERROR("failed to return loaned messages")
      rc_exit = rc;
    }
  }

  return rc_exit;
}

rmw_subscription_t *
rmw_connextdds_create_subscriber(
  rmw_context_impl_t * const ctx,
  const rmw_node_t * const node,
  DDS_DomainParticipant * const dp,
  DDS_Subscriber * const sub,
  const rosidl_message_type_support_t * const type_supports,
  const char * const topic_name,
  const rmw_qos_profile_t * const qos_policies,
  const rmw_subscription_options_t * const subscriber_options,
  const bool internal)
{
  UNUSED_ARG(internal);

  std::lock_guard<std::mutex> guard(ctx->endpoint_mutex);
  RMW_Connext_Subscriber * rmw_sub_impl =
    RMW_Connext_Subscriber::create(
    ctx,
    dp,
    sub,
    type_supports,
    topic_name,
    qos_policies,
    subscriber_options,
    internal);

  if (nullptr == rmw_sub_impl) {
    RMW_CONNEXT_LOG_ERROR(
      "failed to allocate RMW_Connext_Subscriber")
    return nullptr;
  }
  auto scope_exit_rmw_reader_impl_delete =
    rcpputils::make_scope_exit(
    [rmw_sub_impl]()
    {
      if (RMW_RET_OK != rmw_sub_impl->finalize()) {
        RMW_CONNEXT_LOG_ERROR(
          "failed to finalize RMW_Connext_Subscriber")
      }
      delete rmw_sub_impl;
    });
#if RMW_CONNEXT_DEBUG && RMW_CONNEXT_DDS_API == RMW_CONNEXT_DDS_API_PRO
  auto scope_exit_enable_participant_on_error =
    rcpputils::make_scope_exit(
    [ctx]()
    {
      // If we are building in Debug mode, an issue in Connext may prevent the
      // participant from being able to delete any content-filtered topic if
      // the participant has not been enabled.
      // For this reason, make sure to enable the participant before trying to
      // finalize it.
      // TODO(asorbini) reconsider the need for this code in Connext > 6.1.0
      if (DDS_RETCODE_OK !=
      DDS_Entity_enable(DDS_DomainParticipant_as_entity(ctx->participant)))
      {
        RMW_CONNEXT_LOG_ERROR_SET(
          "failed to enable DomainParticipant on subscriber creation error")
      }
    });
#endif  // RMW_CONNEXT_DEBUG && RMW_CONNEXT_DDS_API == RMW_CONNEXT_DDS_API_PRO

  rmw_subscription_t * rmw_subscriber = rmw_subscription_allocate();
  if (nullptr == rmw_subscriber) {
    RMW_CONNEXT_LOG_ERROR_SET(
      "failed to allocate RMW subscriber")
    return nullptr;
  }

  auto scope_exit_rmw_reader_delete = rcpputils::make_scope_exit(
    [rmw_subscriber]() {
      if (nullptr != rmw_subscriber->topic_name) {
        rmw_free(const_cast<char *>(rmw_subscriber->topic_name));
      }
      rmw_subscription_free(rmw_subscriber);
    });

  size_t topic_name_len = strlen(topic_name);

  rmw_subscriber->implementation_identifier = RMW_CONNEXTDDS_ID;
  rmw_subscriber->data = rmw_sub_impl;
  rmw_subscriber->topic_name =
    reinterpret_cast<char *>(rmw_allocate(topic_name_len + 1));
  if (nullptr == rmw_subscriber->topic_name) {
    RMW_CONNEXT_LOG_ERROR_SET(
      "failed to allocate subscriber's topic name")
    return nullptr;
  }
  memcpy(
    const_cast<char *>(rmw_subscriber->topic_name),
    topic_name,
    topic_name_len + 1);
  rmw_subscriber->options = *subscriber_options;
  rmw_subscriber->can_loan_messages = false;
  rmw_subscriber->is_cft_enabled = rmw_sub_impl->is_cft_enabled();

  if (!internal) {
    if (RMW_RET_OK != rmw_sub_impl->enable()) {
      RMW_CONNEXT_LOG_ERROR("failed to enable subscription")
      return nullptr;
    }

    if (RMW_RET_OK !=
      rmw_connextdds_graph_on_subscriber_created(
        ctx, node, rmw_sub_impl))
    {
      RMW_CONNEXT_LOG_ERROR("failed to update graph for subscriber")
      return nullptr;
    }
  }

#if RMW_CONNEXT_DEBUG && RMW_CONNEXT_DDS_API == RMW_CONNEXT_DDS_API_PRO
  scope_exit_enable_participant_on_error.cancel();
#endif  // RMW_CONNEXT_DEBUG && RMW_CONNEXT_DDS_API == RMW_CONNEXT_DDS_API_PRO
  scope_exit_rmw_reader_impl_delete.cancel();
  scope_exit_rmw_reader_delete.cancel();
  return rmw_subscriber;
}

rmw_ret_t
rmw_connextdds_destroy_subscriber(
  rmw_context_impl_t * const ctx,
  rmw_subscription_t * const rmw_subscriber)
{
  std::lock_guard<std::mutex> guard(ctx->endpoint_mutex);
  UNUSED_ARG(ctx);

  RMW_Connext_Subscriber * const rmw_sub_impl =
    static_cast<RMW_Connext_Subscriber *>(rmw_subscriber->data);

  if (nullptr == rmw_sub_impl) {
    RMW_CONNEXT_LOG_ERROR_SET("invalid subscriber data")
    return RMW_RET_ERROR;
  }

  rmw_ret_t rc = rmw_sub_impl->finalize();
  if (RMW_RET_OK != rc) {
    return rc;
  }

  delete rmw_sub_impl;
  rmw_free(const_cast<char *>(rmw_subscriber->topic_name));
  rmw_subscription_free(rmw_subscriber);

  return RMW_RET_OK;
}

static
constexpr uint64_t C_NANOSECONDS_PER_SEC = 1000000000ULL;

#define dds_time_to_u64(t_) \
  ((C_NANOSECONDS_PER_SEC * (uint64_t)(t_)->sec) + (uint64_t)(t_)->nanosec)

void
rmw_connextdds_message_info_from_dds(
  rmw_message_info_t * const to,
  const DDS_SampleInfo * const from)
{
  rmw_connextdds_ih_to_gid(from->publication_handle, to->publisher_gid);
// Message timestamps are disabled on Windows because RTI Connext DDS
// does not support a high enough clock resolution by default (see: _ftime()).
#if !RTI_WIN32
  to->source_timestamp = dds_time_to_u64(&from->source_timestamp);
  to->received_timestamp = dds_time_to_u64(&from->reception_timestamp);
#else
  to->source_timestamp = 0;
  to->received_timestamp = 0;
#endif /* !RTI_WIN32 */
  // Currently we cannot use `rmw_connextdds_sn_dds_to_ros`, as that was used to convert to
  // `rmw_request_id_t.sequence_number`, which is an int64_t and not an uint64_t.
  // When rmw is updated and all sequence numbers are a `uint64_t`,
  // rmw_connextdds_sn_dds_to_ros() should be updated and used everywhere.
  to->publication_sequence_number =
    static_cast<uint64_t>((from->publication_sequence_number).high) << 32 |
    static_cast<uint64_t>((from->publication_sequence_number).low);
#if RMW_CONNEXT_DDS_API == RMW_CONNEXT_DDS_API_PRO
  to->reception_sequence_number =
    static_cast<uint64_t>((from->reception_sequence_number).high) << 32 |
    static_cast<uint64_t>((from->reception_sequence_number).low);
#else
  to->reception_sequence_number = RMW_MESSAGE_INFO_SEQUENCE_NUMBER_UNSUPPORTED;
#endif  // RMW_CONNEXT_DDS_API == RMW_CONNEXT_DDS_API_PRO
}

/******************************************************************************
 * Guard Condition Implementation functions
 ******************************************************************************/

rmw_guard_condition_t *
rmw_connextdds_create_guard_condition(const bool internal)
{
  RMW_Connext_GuardCondition * const gcond =
    new (std::nothrow) RMW_Connext_GuardCondition(internal);

  if (nullptr == gcond) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to create guard condition")
    return nullptr;
  }

  rmw_guard_condition_t * gcond_handle = rmw_guard_condition_allocate();
  if (nullptr == gcond_handle) {
    delete gcond;
    RMW_CONNEXT_LOG_ERROR_SET("failed to create guard condition handle")
    return nullptr;
  }

  gcond_handle->implementation_identifier = RMW_CONNEXTDDS_ID;
  gcond_handle->data = gcond;
  return gcond_handle;
}

rmw_ret_t
rmw_connextdds_destroy_guard_condition(
  rmw_guard_condition_t * const gcond_handle)
{
  RMW_Connext_GuardCondition * const gcond =
    reinterpret_cast<RMW_Connext_GuardCondition *>(gcond_handle->data);

  delete gcond;

  rmw_guard_condition_free(gcond_handle);

  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_trigger_guard_condition(
  const rmw_guard_condition_t * const gcond_handle)
{
  RMW_Connext_GuardCondition * const gcond =
    reinterpret_cast<RMW_Connext_GuardCondition *>(gcond_handle->data);

  return gcond->trigger();
}

rmw_wait_set_t *
rmw_connextdds_create_waitset(const size_t max_conditions)
{
  UNUSED_ARG(max_conditions);

  rmw_wait_set_t * const rmw_ws = rmw_wait_set_allocate();
  if (nullptr == rmw_ws) {
    RMW_CONNEXT_LOG_ERROR("failed to allocate RMW WaitSet")
    return nullptr;
  }
  auto scope_exit_ws_delete = rcpputils::make_scope_exit(
    [rmw_ws]()
    {
      rmw_wait_set_free(rmw_ws);
    });

  RMW_Connext_WaitSet * const ws_impl =
    new (std::nothrow) RMW_Connext_WaitSet();


  if (nullptr == ws_impl) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to allocate WaitSet implementation")
    return nullptr;
  }

  rmw_ws->implementation_identifier = RMW_CONNEXTDDS_ID;
  rmw_ws->data = ws_impl;

  scope_exit_ws_delete.cancel();
  return rmw_ws;
}

rmw_ret_t
rmw_connextdds_destroy_waitset(rmw_wait_set_t * const rmw_ws)
{
  RMW_Connext_WaitSet * const ws_impl =
    reinterpret_cast<RMW_Connext_WaitSet *>(rmw_ws->data);

  delete ws_impl;

  rmw_wait_set_free(rmw_ws);

  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_waitset_wait(
  rmw_wait_set_t * const rmw_ws,
  rmw_subscriptions_t * subs,
  rmw_guard_conditions_t * gcs,
  rmw_services_t * srvs,
  rmw_clients_t * cls,
  rmw_events_t * evs,
  const rmw_time_t * wait_timeout)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(rmw_ws, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_ws,
    rmw_ws->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  RMW_Connext_WaitSet * const ws_impl =
    reinterpret_cast<RMW_Connext_WaitSet *>(rmw_ws->data);

  return ws_impl->wait(subs, gcs, srvs, cls, evs, wait_timeout);
}

/******************************************************************************
 * GUID functions
 ******************************************************************************/
rmw_ret_t
rmw_connextdds_gid_to_guid(const rmw_gid_t & gid, struct DDS_GUID_t & guid)
{
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    &gid,
    gid.implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  static_assert(
    RMW_GID_STORAGE_SIZE >= sizeof(guid.value),
    "rmw_gid_t type too small for an DDS GUID");

  memcpy(guid.value, gid.data, sizeof(guid.value));

  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_guid_to_gid(const struct DDS_GUID_t & guid, rmw_gid_t & gid)
{
  static_assert(
    RMW_GID_STORAGE_SIZE >= sizeof(guid.value),
    "rmw_gid_t type too small for an DDS GUID");
  memset(&gid, 0, sizeof(gid));
  memcpy(gid.data, guid.value, sizeof(guid.value));
  gid.implementation_identifier = RMW_CONNEXTDDS_ID;

  return RMW_RET_OK;
}

void rmw_connextdds_get_entity_gid(
  DDS_Entity * const entity,
  rmw_gid_t & gid)
{
  DDS_InstanceHandle_t ih = DDS_Entity_get_instance_handle(entity);
  rmw_connextdds_ih_to_gid(ih, gid);
}

void rmw_connextdds_get_entity_gid(
  DDS_DomainParticipant * const dp, rmw_gid_t & gid)
{
  DDS_Entity * const entity = DDS_DomainParticipant_as_entity(dp);
  rmw_connextdds_get_entity_gid(entity, gid);
}

void rmw_connextdds_get_entity_gid(
  DDS_Publisher * const pub, rmw_gid_t & gid)
{
  DDS_Entity * const entity = DDS_Publisher_as_entity(pub);
  rmw_connextdds_get_entity_gid(entity, gid);
}

void rmw_connextdds_get_entity_gid(
  DDS_Subscriber * const sub, rmw_gid_t & gid)
{
  DDS_Entity * const entity = DDS_Subscriber_as_entity(sub);
  rmw_connextdds_get_entity_gid(entity, gid);
}

void rmw_connextdds_get_entity_gid(
  DDS_DataWriter * const writer, rmw_gid_t & gid)
{
  DDS_Entity * const entity = DDS_DataWriter_as_entity(writer);
  rmw_connextdds_get_entity_gid(entity, gid);
}

void rmw_connextdds_get_entity_gid(
  DDS_DataReader * const reader, rmw_gid_t & gid)
{
  DDS_Entity * const entity = DDS_DataReader_as_entity(reader);
  rmw_connextdds_get_entity_gid(entity, gid);
}

void rmw_connextdds_get_entity_gid(
  DDS_Topic * const topic, rmw_gid_t & gid)
{
  DDS_Entity * const entity = DDS_Topic_as_entity(topic);
  rmw_connextdds_get_entity_gid(entity, gid);
}

/******************************************************************************
 * Type helpers
 ******************************************************************************/

static
std::string
rmw_connextdds_create_type_name(
  const char * const message_namespace,
  const char * const message_name,
  const char * const message_suffix,
  const bool mangle_prefix)
{
  const char * prefix_sfx = "";
  if (mangle_prefix) {
    prefix_sfx = "_";
  }

  std::ostringstream ss;
  std::string msg_namespace(message_namespace);
  if (!msg_namespace.empty()) {
    ss << msg_namespace << "::";
  }
  ss << "dds" << prefix_sfx << "::" << message_name << message_suffix;
  return ss.str();
}

std::string
rmw_connextdds_create_type_name(
  const message_type_support_callbacks_t * callbacks,
  const bool mangle_names)
{
  const char * msg_prefix = "";
  const bool mangle_prefix = mangle_names;
  if (mangle_names) {
    msg_prefix = "_";
  }
  return rmw_connextdds_create_type_name(
    callbacks->message_namespace_,
    callbacks->message_name_,
    msg_prefix,
    mangle_prefix);
}

std::string
rmw_connextdds_create_type_name(
  const rosidl_typesupport_introspection_cpp::MessageMembers * const members,
  const bool mangle_names)
{
  const char * msg_prefix = "";
  const bool mangle_prefix = mangle_names;
  if (mangle_names) {
    msg_prefix = "_";
  }
  return rmw_connextdds_create_type_name(
    members->message_namespace_,
    members->message_name_,
    msg_prefix,
    mangle_prefix);
}

std::string
rmw_connextdds_create_type_name(
  const rosidl_typesupport_introspection_c__MessageMembers * const members,
  const bool mangle_names)
{
  const char * msg_prefix = "";
  const bool mangle_prefix = mangle_names;
  if (mangle_names) {
    msg_prefix = "_";
  }
  return rmw_connextdds_create_type_name(
    members->message_namespace_,
    members->message_name_,
    msg_prefix,
    mangle_prefix);
}

std::string
rmw_connextdds_create_type_name_request(
  const service_type_support_callbacks_t * callbacks,
  const bool mangle_names)
{
  const char * msg_prefix = "Request";
  const bool mangle_prefix = mangle_names;
  if (mangle_names) {
    msg_prefix = "_Request_";
  }
  return rmw_connextdds_create_type_name(
    callbacks->service_namespace_,
    callbacks->service_name_,
    msg_prefix,
    mangle_prefix);
}

std::string
rmw_connextdds_create_type_name_response(
  const service_type_support_callbacks_t * callbacks,
  const bool mangle_names)
{
  const char * msg_prefix = "Response";
  const bool mangle_prefix = mangle_names;
  if (mangle_names) {
    msg_prefix = "_Response_";
  }
  return rmw_connextdds_create_type_name(
    callbacks->service_namespace_,
    callbacks->service_name_,
    msg_prefix,
    mangle_prefix);
}

/******************************************************************************
 * Client/Service helpers
 ******************************************************************************/
static rmw_ret_t
rmw_connextdds_client_content_filter(
  DDS_InstanceHandle_t * const writer_ih,
  const char * const reply_topic,
  char ** const cft_name_out,
  char ** const cft_filter_out)
{
  // TODO(asorbini) convert ih directly to guid
  DDS_GUID_t writer_guid = DDS_GUID_INITIALIZER;
  rmw_gid_t writer_gid;
  rmw_connextdds_ih_to_gid(*writer_ih, writer_gid);
  rmw_connextdds_gid_to_guid(writer_gid, writer_guid);

#define GUID_FMT \
  "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X"

#define GUID_FMT_ARGS(g_) \
  (g_)->value[0], \
  (g_)->value[1], \
  (g_)->value[2], \
  (g_)->value[3], \
  (g_)->value[4], \
  (g_)->value[5], \
  (g_)->value[6], \
  (g_)->value[7], \
  (g_)->value[8], \
  (g_)->value[9], \
  (g_)->value[10], \
  (g_)->value[11], \
  (g_)->value[12], \
  (g_)->value[13], \
  (g_)->value[14], \
  (g_)->value[15]

  /* Create content-filtered topic expression for the reply reader */
  static const int GUID_SIZE = 16;
  static const char * GUID_FIELD_NAME =
    "@related_sample_identity.writer_guid.value";
  const size_t reply_topic_len = strlen(reply_topic);
  const size_t cft_name_len = reply_topic_len + 1 + (GUID_SIZE * 2);
  char * const cft_name = DDS_String_alloc(cft_name_len);
  if (nullptr == cft_name) {
    RMW_CONNEXT_LOG_ERROR_SET("failed too allocate cft name")
    return RMW_RET_ERROR;
  }
  auto scope_exit_cft_name = rcpputils::make_scope_exit(
    [cft_name]()
    {
      DDS_String_free(cft_name);
    });

  // TODO(asorbini) check output of snprintf()
  snprintf(
    cft_name, cft_name_len + 1 /* \0 */,
    "%s_" GUID_FMT,
    reply_topic,
    GUID_FMT_ARGS(&writer_guid));

  const size_t cft_filter_len =
    strlen(GUID_FIELD_NAME) + strlen(" = &hex(") + (GUID_SIZE * 2) + 1;
  char * const cft_filter = DDS_String_alloc(cft_filter_len);
  if (nullptr == cft_filter) {
    RMW_CONNEXT_LOG_ERROR_SET("failed too allocate cft filter")
    return RMW_RET_ERROR;
  }
  auto scope_exit_cft_filter = rcpputils::make_scope_exit(
    [cft_filter]()
    {
      DDS_String_free(cft_filter);
    });

  // TODO(asorbini) check output of snprintf()
  snprintf(
    cft_filter, cft_filter_len + 1 /* \0 */, "%s = &hex(" GUID_FMT ")",
    GUID_FIELD_NAME,
    GUID_FMT_ARGS(&writer_guid));

#undef GUID_FMT
#undef GUID_FMT_ARGS

  scope_exit_cft_name.cancel();
  scope_exit_cft_filter.cancel();

  *cft_name_out = cft_name;
  *cft_filter_out = cft_filter;

  return RMW_RET_OK;
}


RMW_Connext_Client *
RMW_Connext_Client::create(
  rmw_context_impl_t * const ctx,
  DDS_DomainParticipant * const dp,
  DDS_Publisher * const pub,
  DDS_Subscriber * const sub,
  const rosidl_service_type_support_t * const type_supports,
  const char * const svc_name,
  const rmw_qos_profile_t * const qos_policies)
{
  RMW_Connext_Client * client_impl =
    new (std::nothrow) RMW_Connext_Client();

  if (nullptr == client_impl) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to allocate client implementation")
    return nullptr;
  }

  client_impl->ctx = ctx;

  auto scope_exit_client_impl_delete = rcpputils::make_scope_exit(
    [client_impl]()
    {
      if (RMW_RET_OK != client_impl->finalize()) {
        RMW_CONNEXT_LOG_ERROR("failed to finalize client on error")
      }
      delete client_impl;
    });
  bool svc_members_req_cpp = false,
    svc_members_res_cpp = false;
  const void * svc_members_req = nullptr,
    * svc_members_res = nullptr;
  const rosidl_message_type_support_t
  * type_support_req =
    RMW_Connext_ServiceTypeSupportWrapper::get_request_type_support(
    type_supports,
    &svc_members_req,
    svc_members_req_cpp),
    * type_support_res =
    RMW_Connext_ServiceTypeSupportWrapper::get_response_type_support(
    type_supports,
    &svc_members_res,
    svc_members_res_cpp);

  if (nullptr == type_support_req || nullptr == type_support_res) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to lookup type supports for client")
    return nullptr;
  }

  std::string reply_topic =
    rmw_connextdds_create_topic_name(
    ROS_SERVICE_RESPONSE_PREFIX,
    svc_name,
    "Reply",
    qos_policies);
  std::string request_topic =
    rmw_connextdds_create_topic_name(
    ROS_SERVICE_REQUESTER_PREFIX,
    svc_name,
    "Request",
    qos_policies);

  std::string
    request_type =
    RMW_Connext_ServiceTypeSupportWrapper::get_request_type_name(
    type_supports),
    reply_type =
    RMW_Connext_ServiceTypeSupportWrapper::get_response_type_name(
    type_supports);

  rmw_publisher_options_t pub_options = rmw_get_default_publisher_options();
  rmw_subscription_options_t sub_options = rmw_get_default_subscription_options();


  RMW_CONNEXT_LOG_DEBUG_A(
    "creating request publisher: "
    "service=%s, "
    "topic=%s",
    svc_name,
    request_topic.c_str())

  client_impl->request_pub =
    RMW_Connext_Publisher::create(
    ctx,
    dp,
    pub,
    type_support_req,
    request_topic.c_str(),
    qos_policies,
    &pub_options,
    false /* internal */,
    RMW_CONNEXT_MESSAGE_REQUEST,
    svc_members_req,
    svc_members_req_cpp,
    &request_type);

  if (nullptr == client_impl->request_pub) {
    RMW_CONNEXT_LOG_ERROR("failed to create client requester")
    return nullptr;
  }

  RMW_CONNEXT_LOG_DEBUG_A(
    "creating reply subscriber: "
    "service=%s, "
    "topic=%s",
    svc_name,
    reply_topic.c_str())

  char * cft_name = nullptr,
  *cft_filter = nullptr;
  auto scope_exit_cft_name = rcpputils::make_scope_exit(
    [cft_name, cft_filter]()
    {
      if (nullptr != cft_name) {
        DDS_String_free(cft_name);
      }
      if (nullptr != cft_filter) {
        DDS_String_free(cft_filter);
      }
    });

  if (ctx->request_reply_mapping == RMW_Connext_RequestReplyMapping::Extended) {
    DDS_InstanceHandle_t writer_ih = client_impl->request_pub->instance_handle();
    if (RMW_RET_OK != rmw_connextdds_client_content_filter(
        &writer_ih, reply_topic.c_str(), &cft_name, &cft_filter))
    {
      RMW_CONNEXT_LOG_ERROR("failed to create content filter for client")
      return nullptr;
    }
  }

  client_impl->reply_sub =
    RMW_Connext_Subscriber::create(
    ctx,
    dp,
    sub,
    type_support_res,
    reply_topic.c_str(),
    qos_policies,
    &sub_options,
    false /* internal */,
    RMW_CONNEXT_MESSAGE_REPLY,
    svc_members_res,
    svc_members_res_cpp,
    &reply_type,
    cft_name,
    cft_filter);

  if (nullptr == client_impl->reply_sub) {
    RMW_CONNEXT_LOG_ERROR("failed to create client replier")
    return nullptr;
  }

  scope_exit_client_impl_delete.cancel();
  return client_impl;
}

rmw_ret_t
RMW_Connext_Client::enable()
{
  rmw_ret_t rc = this->request_pub->enable();
  if (RMW_RET_OK != rc) {
    RMW_CONNEXT_LOG_ERROR("failed to enable client's publisher")
    return rc;
  }
  rc = this->reply_sub->enable();
  if (RMW_RET_OK != rc) {
    RMW_CONNEXT_LOG_ERROR("failed to enable client's subscription")
    return rc;
  }
  return RMW_RET_OK;
}

rmw_ret_t
RMW_Connext_Client::is_service_available(bool & available)
{
  // mark service as available if we have at least one writer and one reader
  // matched from the same remote DomainParticipant.
  struct DDS_InstanceHandleSeq matched_req_subs = DDS_SEQUENCE_INITIALIZER,
    matched_rep_pubs = DDS_SEQUENCE_INITIALIZER;
  auto scope_exit_seqs = rcpputils::make_scope_exit(
    [&matched_req_subs, &matched_rep_pubs]()
    {
      if (!DDS_InstanceHandleSeq_finalize(&matched_req_subs)) {
        RMW_CONNEXT_LOG_ERROR("failed to finalize req instance handle sequence")
      }
      if (!DDS_InstanceHandleSeq_finalize(&matched_rep_pubs)) {
        RMW_CONNEXT_LOG_ERROR("failed to finalize rep instance handle sequence")
      }
    });

  DDS_ReturnCode_t dds_rc =
    DDS_DataWriter_get_matched_subscriptions(this->request_pub->writer(), &matched_req_subs);
  if (DDS_RETCODE_OK != dds_rc) {
    RMW_CONNEXT_LOG_ERROR_A_SET("failed to list matched subscriptions: dds_rc=%d", dds_rc)
    return RMW_RET_ERROR;
  }

  dds_rc =
    DDS_DataReader_get_matched_publications(this->reply_sub->reader(), &matched_rep_pubs);
  if (DDS_RETCODE_OK != dds_rc) {
    RMW_CONNEXT_LOG_ERROR_A_SET("failed to list matched publications: dds_rc=%d", dds_rc)
    return RMW_RET_ERROR;
  }

  const DDS_Long subs_len = DDS_InstanceHandleSeq_get_length(&matched_req_subs),
    pubs_len = DDS_InstanceHandleSeq_get_length(&matched_rep_pubs);

  for (DDS_Long i = 0; i < subs_len && !available; i++) {
    DDS_InstanceHandle_t * const sub_ih =
      DDS_InstanceHandleSeq_get_reference(&matched_req_subs, i);

    for (DDS_Long j = 0; j < pubs_len && !available; j++) {
      DDS_InstanceHandle_t * const pub_ih =
        DDS_InstanceHandleSeq_get_reference(&matched_rep_pubs, j);
      available = DDS_InstanceHandle_compare_prefix(sub_ih, pub_ih) == 0;
    }
  }

  return RMW_RET_OK;
}

rmw_ret_t
RMW_Connext_Client::take_response(
  rmw_service_info_t * const request_header,
  void * const ros_response,
  bool * const taken)
{
  *taken = false;

  RMW_Connext_RequestReplyMessage rr_msg;
  rr_msg.request = false;
  rr_msg.payload = ros_response;

  rmw_message_info_t message_info;
  bool taken_msg = false;

  DDS_InstanceHandle_t req_writer_handle =
    this->request_pub->instance_handle();

  rmw_ret_t rc =
    this->reply_sub->take_message(
    &rr_msg, &message_info, &taken_msg, &req_writer_handle);

  if (RMW_RET_OK != rc) {
    return rc;
  }

  if (taken_msg) {
    request_header->request_id.sequence_number = rr_msg.sn;

    memcpy(
      request_header->request_id.writer_guid,
      rr_msg.gid.data,
      16);

    if (this->ctx->cyclone_compatible) {
      // Copy 8 LSB from the client's publisher's guid
      memcpy(
        request_header->request_id.writer_guid,
        this->request_pub->gid()->data,
        8);
    }

    request_header->source_timestamp = message_info.source_timestamp;
    request_header->received_timestamp = message_info.received_timestamp;

    *taken = true;

    RMW_CONNEXT_LOG_DEBUG_A(
      "[%s] taken RESPONSE: "
      "gid=%08X.%08X.%08X.%08X, "
      "sn=%lu",
      this->reply_sub->message_type_support()->type_name(),
      reinterpret_cast<const uint32_t *>(rr_msg.gid.data)[0],
      reinterpret_cast<const uint32_t *>(rr_msg.gid.data)[1],
      reinterpret_cast<const uint32_t *>(rr_msg.gid.data)[2],
      reinterpret_cast<const uint32_t *>(rr_msg.gid.data)[3],
      rr_msg.sn)
  }

  return RMW_RET_OK;
}

rmw_ret_t
RMW_Connext_Client::send_request(
  const void * const ros_request,
  int64_t * const sequence_id)
{
  RMW_Connext_RequestReplyMessage rr_msg;
  rr_msg.request = true;

  if (this->ctx->request_reply_mapping == RMW_Connext_RequestReplyMapping::Basic) {
    *sequence_id = ++this->next_request_id;
    rr_msg.sn = *sequence_id;
  } else {
    rr_msg.sn = -1;
  }
  rr_msg.gid = *this->request_pub->gid();
  rr_msg.payload = const_cast<void *>(ros_request);

  RMW_CONNEXT_LOG_DEBUG_A(
    "[%s] send REQUEST: "
    "gid=%08X.%08X.%08X.%08X, "
    "sn=%lu",
    this->request_pub->message_type_support()->type_name(),
    reinterpret_cast<const uint32_t *>(rr_msg.gid.data)[0],
    reinterpret_cast<const uint32_t *>(rr_msg.gid.data)[1],
    reinterpret_cast<const uint32_t *>(rr_msg.gid.data)[2],
    reinterpret_cast<const uint32_t *>(rr_msg.gid.data)[3],
    rr_msg.sn)

  rmw_ret_t rc = this->request_pub->write(&rr_msg, false /* serialized */, sequence_id);

  RMW_CONNEXT_LOG_DEBUG_A(
    "[%s] SENT REQUEST: "
    "gid=%08X.%08X.%08X.%08X, "
    "sn=%lu",
    this->request_pub->message_type_support()->type_name(),
    reinterpret_cast<const uint32_t *>(rr_msg.gid.data)[0],
    reinterpret_cast<const uint32_t *>(rr_msg.gid.data)[1],
    reinterpret_cast<const uint32_t *>(rr_msg.gid.data)[2],
    reinterpret_cast<const uint32_t *>(rr_msg.gid.data)[3],
    *sequence_id)

  return rc;
}

rmw_ret_t
RMW_Connext_Client::request_publisher_qos(rmw_qos_profile_t * const qos)
{
  rmw_ret_t rc = this->request_pub->qos(qos);
  if (rc != RMW_RET_OK) {
    RMW_SET_ERROR_MSG("coudn't get client's request publisher qos");
    return rc;
  }
  return RMW_RET_OK;
}

rmw_ret_t
RMW_Connext_Client::response_subscription_qos(rmw_qos_profile_t * const qos)
{
  rmw_ret_t rc = this->reply_sub->qos(qos);
  if (rc != RMW_RET_OK) {
    RMW_SET_ERROR_MSG("coudn't get client's response subscription qos");
    return rc;
  }
  return RMW_RET_OK;
}

rmw_ret_t
RMW_Connext_Client::finalize()
{
  if (nullptr != this->request_pub) {
    if (RMW_RET_OK != this->request_pub->finalize()) {
      RMW_CONNEXT_LOG_ERROR("failed to finalize client publisher")
      return RMW_RET_ERROR;
    }
    delete this->request_pub;
    this->request_pub = nullptr;
  }

  if (nullptr != this->reply_sub) {
    if (RMW_RET_OK != this->reply_sub->finalize()) {
      RMW_CONNEXT_LOG_ERROR("failed to finalize client subscriber")
      return RMW_RET_ERROR;
    }

    delete this->reply_sub;
    this->reply_sub = nullptr;
  }

  return RMW_RET_OK;
}


RMW_Connext_Service *
RMW_Connext_Service::create(
  rmw_context_impl_t * const ctx,
  DDS_DomainParticipant * const dp,
  DDS_Publisher * const pub,
  DDS_Subscriber * const sub,
  const rosidl_service_type_support_t * const type_supports,
  const char * const svc_name,
  const rmw_qos_profile_t * const qos_policies)
{
  RMW_Connext_Service * svc_impl =
    new (std::nothrow) RMW_Connext_Service();

  if (nullptr == svc_impl) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to allocate service implementation")
    return nullptr;
  }

  svc_impl->ctx = ctx;

  auto scope_exit_svc_impl_delete = rcpputils::make_scope_exit(
    [svc_impl]()
    {
      if (RMW_RET_OK != svc_impl->finalize()) {
        RMW_CONNEXT_LOG_ERROR("failed to finalize service on error")
      }
      delete svc_impl;
    });

  bool svc_members_req_cpp = false,
    svc_members_res_cpp = false;
  const void * svc_members_req = nullptr,
    * svc_members_res = nullptr;
  const rosidl_message_type_support_t
  * type_support_req =
    RMW_Connext_ServiceTypeSupportWrapper::get_request_type_support(
    type_supports,
    &svc_members_req,
    svc_members_req_cpp),
    * type_support_res =
    RMW_Connext_ServiceTypeSupportWrapper::get_response_type_support(
    type_supports,
    &svc_members_res,
    svc_members_res_cpp);

  if (nullptr == type_support_req || nullptr == type_support_res) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to lookup type supports for service")
    return nullptr;
  }

  std::string reply_topic =
    rmw_connextdds_create_topic_name(
    ROS_SERVICE_RESPONSE_PREFIX,
    svc_name,
    "Reply",
    qos_policies);
  std::string request_topic =
    rmw_connextdds_create_topic_name(
    ROS_SERVICE_REQUESTER_PREFIX,
    svc_name,
    "Request",
    qos_policies);

  std::string
    request_type =
    RMW_Connext_ServiceTypeSupportWrapper::get_request_type_name(
    type_supports),
    reply_type =
    RMW_Connext_ServiceTypeSupportWrapper::get_response_type_name(
    type_supports);

  rmw_publisher_options_t pub_options = rmw_get_default_publisher_options();
  rmw_subscription_options_t sub_options = rmw_get_default_subscription_options();

  RMW_CONNEXT_LOG_DEBUG_A(
    "creating reply publisher: "
    "service=%s, "
    "topic=%s",
    svc_name,
    reply_topic.c_str())

  svc_impl->reply_pub =
    RMW_Connext_Publisher::create(
    ctx,
    dp,
    pub,
    type_support_res,
    reply_topic.c_str(),
    qos_policies,
    &pub_options,
    false /* internal */,
    RMW_CONNEXT_MESSAGE_REPLY,
    svc_members_res,
    svc_members_res_cpp,
    &reply_type);

  if (nullptr == svc_impl->reply_pub) {
    RMW_CONNEXT_LOG_ERROR("failed to create service replier")
    return nullptr;
  }

  RMW_CONNEXT_LOG_DEBUG_A(
    "creating request subscriber: "
    "service=%s, "
    "topic=%s",
    svc_name,
    request_topic.c_str())

  svc_impl->request_sub =
    RMW_Connext_Subscriber::create(
    ctx,
    dp,
    sub,
    type_support_req,
    request_topic.c_str(),
    qos_policies,
    &sub_options,
    false /* internal */,
    RMW_CONNEXT_MESSAGE_REQUEST,
    svc_members_req,
    svc_members_req_cpp,
    &request_type);

  if (nullptr == svc_impl->request_sub) {
    RMW_CONNEXT_LOG_ERROR("failed to create service requester")
    return nullptr;
  }

  scope_exit_svc_impl_delete.cancel();
  return svc_impl;
}

rmw_ret_t
RMW_Connext_Service::enable()
{
  rmw_ret_t rc = this->reply_pub->enable();
  if (RMW_RET_OK != rc) {
    RMW_CONNEXT_LOG_ERROR("failed to enable service's publisher")
    return rc;
  }
  rc = this->request_sub->enable();
  if (RMW_RET_OK != rc) {
    RMW_CONNEXT_LOG_ERROR("failed to enable service's subscription")
    return rc;
  }
  return RMW_RET_OK;
}

rmw_ret_t
RMW_Connext_Service::take_request(
  rmw_service_info_t * const request_header,
  void * const ros_request,
  bool * const taken)
{
  *taken = false;

  RMW_Connext_RequestReplyMessage rr_msg;
  rr_msg.request = true;
  rr_msg.payload = ros_request;

  rmw_message_info_t message_info;
  bool taken_msg = false;

  rmw_ret_t rc =
    this->request_sub->take_message(
    &rr_msg, &message_info, &taken_msg);

  if (RMW_RET_OK != rc) {
    return rc;
  }

  if (taken_msg) {
    request_header->request_id.sequence_number = rr_msg.sn;

    memcpy(
      request_header->request_id.writer_guid,
      rr_msg.gid.data,
      16);

    request_header->source_timestamp = message_info.source_timestamp;
    request_header->received_timestamp = message_info.received_timestamp;

    *taken = true;

    RMW_CONNEXT_LOG_DEBUG_A(
      "[%s] taken REQUEST: "
      "gid=%08X.%08X.%08X.%08X, "
      "sn=%lu",
      this->request_sub->message_type_support()->type_name(),
      reinterpret_cast<const uint32_t *>(rr_msg.gid.data)[0],
      reinterpret_cast<const uint32_t *>(rr_msg.gid.data)[1],
      reinterpret_cast<const uint32_t *>(rr_msg.gid.data)[2],
      reinterpret_cast<const uint32_t *>(rr_msg.gid.data)[3],
      rr_msg.sn)
  }

  return RMW_RET_OK;
}

rmw_ret_t
RMW_Connext_Service::send_response(
  rmw_request_id_t * const request_id,
  const void * const ros_response)
{
  RMW_Connext_RequestReplyMessage rr_msg;
  rr_msg.request = false;
  rr_msg.sn = request_id->sequence_number;
  memcpy(rr_msg.gid.data, request_id->writer_guid, 16);
  rr_msg.gid.implementation_identifier = RMW_CONNEXTDDS_ID;
  rr_msg.payload = const_cast<void *>(ros_response);

  RMW_CONNEXT_LOG_DEBUG_A(
    "[%s] send RESPONSE: "
    "gid=%08X.%08X.%08X.%08X, "
    "sn=%lu",
    this->reply_pub->message_type_support()->type_name(),
    reinterpret_cast<const uint32_t *>(rr_msg.gid.data)[0],
    reinterpret_cast<const uint32_t *>(rr_msg.gid.data)[1],
    reinterpret_cast<const uint32_t *>(rr_msg.gid.data)[2],
    reinterpret_cast<const uint32_t *>(rr_msg.gid.data)[3],
    rr_msg.sn)

  return this->reply_pub->write(&rr_msg, false /* serialized */);
}

rmw_ret_t
RMW_Connext_Service::response_publisher_qos(rmw_qos_profile_t * const qos)
{
  rmw_ret_t rc = this->reply_pub->qos(qos);
  if (rc != RMW_RET_OK) {
    RMW_SET_ERROR_MSG("coudn't get service's response publisher qos");
    return rc;
  }
  return RMW_RET_OK;
}

rmw_ret_t
RMW_Connext_Service::request_subscription_qos(rmw_qos_profile_t * const qos)
{
  rmw_ret_t rc = this->request_sub->qos(qos);
  if (rc != RMW_RET_OK) {
    RMW_SET_ERROR_MSG("coudn't get service's request subscription qos");
    return rc;
  }
  return RMW_RET_OK;
}

rmw_ret_t
RMW_Connext_Service::finalize()
{
  if (nullptr != this->reply_pub) {
    if (RMW_RET_OK != this->reply_pub->finalize()) {
      RMW_CONNEXT_LOG_ERROR("failed to finalize service publisher")
      return RMW_RET_ERROR;
    }

    delete this->reply_pub;
    this->reply_pub = nullptr;
  }

  if (nullptr != this->request_sub) {
    if (RMW_RET_OK != this->request_sub->finalize()) {
      RMW_CONNEXT_LOG_ERROR("failed to finalize service subscriber")
      return RMW_RET_ERROR;
    }

    delete this->request_sub;
    this->request_sub = nullptr;
  }

  return RMW_RET_OK;
}

/******************************************************************************
 * Event helpers
 ******************************************************************************/

DDS_StatusKind
ros_event_to_dds(const rmw_event_type_t ros, bool * const invalid)
{
  if (nullptr != invalid) {
    *invalid = false;
  }
  switch (ros) {
    case RMW_EVENT_LIVELINESS_CHANGED:
      {
        return DDS_LIVELINESS_CHANGED_STATUS;
      }
    case RMW_EVENT_REQUESTED_DEADLINE_MISSED:
      {
        return DDS_REQUESTED_DEADLINE_MISSED_STATUS;
      }
    case RMW_EVENT_LIVELINESS_LOST:
      {
        return DDS_LIVELINESS_LOST_STATUS;
      }
    case RMW_EVENT_OFFERED_DEADLINE_MISSED:
      {
        return DDS_OFFERED_DEADLINE_MISSED_STATUS;
      }
    case RMW_EVENT_REQUESTED_QOS_INCOMPATIBLE:
      {
        return DDS_REQUESTED_INCOMPATIBLE_QOS_STATUS;
      }
    case RMW_EVENT_OFFERED_QOS_INCOMPATIBLE:
      {
        return DDS_OFFERED_INCOMPATIBLE_QOS_STATUS;
      }
    case RMW_EVENT_MESSAGE_LOST:
      {
        return DDS_SAMPLE_LOST_STATUS;
      }
    default:
      {
        if (nullptr != invalid) {
          *invalid = true;
        }
        return (DDS_StatusKind)UINT32_MAX;
      }
  }
}

const char *
dds_event_to_str(const DDS_StatusKind event)
{
  switch (event) {
    case DDS_LIVELINESS_CHANGED_STATUS:
      {
        return "LIVELINESS_CHANGED";
      }
    case DDS_REQUESTED_DEADLINE_MISSED_STATUS:
      {
        return "REQUESTED_DEADLINE_MISSED";
      }
    case DDS_LIVELINESS_LOST_STATUS:
      {
        return "LIVELINESS_LOST";
      }
    case DDS_OFFERED_DEADLINE_MISSED_STATUS:
      {
        return "OFFERED_DEADLINE_MISSED";
      }
    case DDS_REQUESTED_INCOMPATIBLE_QOS_STATUS:
      {
        return "REQUESTED_INCOMPATIBLE_QOS";
      }
    case DDS_OFFERED_INCOMPATIBLE_QOS_STATUS:
      {
        return "OFFERED_INCOMPATIBLE_QOS";
      }
    case DDS_SAMPLE_LOST_STATUS:
      {
        return "SAMPLE_LOST";
      }
    default:
      {
        return "UNSUPPORTED";
      }
  }
}

bool
ros_event_for_reader(const rmw_event_type_t ros)
{
  switch (ros) {
    case RMW_EVENT_LIVELINESS_CHANGED:
    case RMW_EVENT_REQUESTED_DEADLINE_MISSED:
    case RMW_EVENT_REQUESTED_QOS_INCOMPATIBLE:
    case RMW_EVENT_MESSAGE_LOST:
      {
        return true;
      }
    default:
      {
        return false;
      }
  }
}

rmw_ret_t
RMW_Connext_SubscriberStatusCondition::get_status(
  const rmw_event_type_t event_type, void * const event_info)
{
  rmw_ret_t rc = RMW_RET_ERROR;

  switch (event_type) {
    case RMW_EVENT_LIVELINESS_CHANGED:
      {
        rmw_liveliness_changed_status_t * status =
          reinterpret_cast<rmw_liveliness_changed_status_t *>(event_info);

        rc = this->get_liveliness_changed_status(status);
        break;
      }
    case RMW_EVENT_REQUESTED_DEADLINE_MISSED:
      {
        rmw_requested_deadline_missed_status_t * status =
          reinterpret_cast<rmw_requested_deadline_missed_status_t *>(event_info);

        rc = this->get_requested_deadline_missed_status(status);
        break;
      }
    case RMW_EVENT_REQUESTED_QOS_INCOMPATIBLE:
      {
        rmw_requested_qos_incompatible_event_status_t * const status =
          reinterpret_cast<rmw_requested_qos_incompatible_event_status_t *>(event_info);

        rc = this->get_requested_qos_incompatible_status(status);
        break;
      }
    case RMW_EVENT_MESSAGE_LOST:
      {
        rmw_message_lost_status_t * const status =
          reinterpret_cast<rmw_message_lost_status_t *>(event_info);

        rc = this->get_message_lost_status(status);
        break;
      }
    default:
      {
        RMW_CONNEXT_LOG_ERROR_A_SET(
          "unsupported subscriber qos: %d", event_type)
        RMW_CONNEXT_ASSERT(0)
        return RMW_RET_ERROR;
      }
  }

  return rc;
}

rmw_ret_t
RMW_Connext_PublisherStatusCondition::get_status(
  const rmw_event_type_t event_type, void * const event_info)
{
  rmw_ret_t rc = RMW_RET_ERROR;

  switch (event_type) {
    case RMW_EVENT_LIVELINESS_LOST:
      {
        rmw_liveliness_lost_status_t * status =
          reinterpret_cast<rmw_liveliness_lost_status_t *>(event_info);

        rc = this->get_liveliness_lost_status(status);
        break;
      }
    case RMW_EVENT_OFFERED_DEADLINE_MISSED:
      {
        rmw_offered_deadline_missed_status_t * status =
          reinterpret_cast<rmw_offered_deadline_missed_status_t *>(event_info);

        rc = this->get_offered_deadline_missed_status(status);
        break;
      }
    case RMW_EVENT_OFFERED_QOS_INCOMPATIBLE:
      {
        rmw_offered_qos_incompatible_event_status_t * const status =
          reinterpret_cast<rmw_offered_qos_incompatible_event_status_t *>(event_info);

        rc = this->get_offered_qos_incompatible_status(status);
        break;
      }
    default:
      {
        RMW_CONNEXT_LOG_ERROR_A_SET("unsupported publisher qos: %d", event_type)
        RMW_CONNEXT_ASSERT(0)
        return RMW_RET_ERROR;
      }
  }

  return rc;
}
