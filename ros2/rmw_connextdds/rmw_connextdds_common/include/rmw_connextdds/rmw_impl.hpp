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

#ifndef RMW_CONNEXTDDS__RMW_IMPL_HPP_
#define RMW_CONNEXTDDS__RMW_IMPL_HPP_

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "rmw_connextdds/context.hpp"
#include "rmw_connextdds/type_support.hpp"
#include "rmw_connextdds/demangle.hpp"
#include "rmw_connextdds/namespace_prefix.hpp"
#include "rmw_connextdds/rmw_api_impl.hpp"

#include "rcutils/types/uint8_array.h"
#include "rcpputils/thread_safety_annotations.hpp"

/******************************************************************************
 * General helpers and utilities.
 ******************************************************************************/

rcutils_ret_t
rcutils_uint8_array_copy(
  rcutils_uint8_array_t * const dst,
  const rcutils_uint8_array_t * const src);

rmw_qos_policy_kind_t
dds_qos_policy_to_rmw_qos_policy(const DDS_QosPolicyId_t last_policy_id);

bool ros_event_for_reader(const rmw_event_type_t ros);

DDS_StatusKind
ros_event_to_dds(const rmw_event_type_t ros, bool * const invalid);

const char *
dds_event_to_str(const DDS_StatusKind event);

rmw_ret_t
rmw_connextdds_parse_string_list(
  const char * const list,
  struct DDS_StringSeq * const parsed_out,
  const char delimiter = ',',
  const bool trim = true,
  const bool allow_empty_elements = true,
  const bool append_values = true);

bool rmw_connextdds_find_string_in_list(
  const DDS_StringSeq * const values,
  const char * const value);

DDS_Duration_t rmw_connextdds_duration_from_ros_time(
  const rmw_time_t * const ros_time);

/******************************************************************************
 * WaitSet wrapper
 ******************************************************************************/
class RMW_Connext_Publisher;
class RMW_Connext_Subscriber;
class RMW_Connext_Client;
class RMW_Connext_Service;

#include "rmw_connextdds/rmw_waitset_std.hpp"

/******************************************************************************
 * Node support
 ******************************************************************************/
class RMW_Connext_Node
{
  rmw_context_impl_t * ctx;

  explicit RMW_Connext_Node(rmw_context_impl_t * const ctx)
  : ctx(ctx)
  {
  }

public:
  static RMW_Connext_Node *
  create(rmw_context_impl_t * const ctx);

  rmw_ret_t
  finalize();

  rmw_guard_condition_t *
  graph_guard_condition()
  {
    return this->ctx->common.graph_guard_condition;
  }
};

/******************************************************************************
 * Publication support
 ******************************************************************************/

class RMW_Connext_Publisher
{
public:
  static RMW_Connext_Publisher *
  create(
    rmw_context_impl_t * const ctx,
    DDS_DomainParticipant * const dp,
    DDS_Publisher * const pub,
    const rosidl_message_type_support_t * const type_supports,
    const char * const topic_name,
    const rmw_qos_profile_t * const qos_policies,
    const rmw_publisher_options_t * const publisher_options,
    const bool internal = false,
    const RMW_Connext_MessageType msg_type = RMW_CONNEXT_MESSAGE_USERDATA,
    const void * const intro_members = nullptr,
    const bool intro_members_cpp = false,
    std::string * const type_name = nullptr);

  rmw_ret_t
  finalize();

  DDS_DataWriter *
  writer() const
  {
    return this->dds_writer;
  }

  RMW_Connext_MessageTypeSupport *
  message_type_support() const
  {
    return this->type_support;
  }

  DDS_InstanceHandle_t
  instance_handle()
  {
    return DDS_Entity_get_instance_handle(
      DDS_DataWriter_as_entity(this->writer()));
  }

  rmw_ret_t
  write(
    const void * const ros_message,
    const bool serialized,
    int64_t * const sn_out = nullptr);

  rmw_ret_t
  enable() const
  {
    if (DDS_RETCODE_OK !=
      DDS_Entity_enable(
        DDS_Topic_as_entity(
          DDS_DataWriter_get_topic(this->dds_writer))))
    {
      RMW_CONNEXT_LOG_ERROR_A_SET(
        "failed to enable dds writer's topic: '%s' [%s]",
        DDS_TopicDescription_get_name(
          DDS_Topic_as_topicdescription(this->dds_topic())),
        this->type_support->type_name())
      return RMW_RET_ERROR;
    }

    if (DDS_RETCODE_OK !=
      DDS_Entity_enable(DDS_DataWriter_as_entity(this->dds_writer)))
    {
      RMW_CONNEXT_LOG_ERROR_A_SET(
        "failed to enable dds writer: '%s' [%s]",
        DDS_TopicDescription_get_name(
          DDS_Topic_as_topicdescription(this->dds_topic())),
        this->type_support->type_name())
      return RMW_RET_ERROR;
    }

    return RMW_RET_OK;
  }

  RMW_Connext_PublisherStatusCondition *
  condition()
  {
    return &this->status_condition;
  }

  const rmw_gid_t * gid() const
  {
    return &this->ros_gid;
  }

  size_t
  subscriptions_count();

  rmw_ret_t
  assert_liveliness();

  rmw_ret_t
  wait_for_all_acked(rmw_time_t wait_timeout);

  rmw_ret_t
  qos(rmw_qos_profile_t * const qos);

  rmw_ret_t
  requestreply_header_to_dds(
    const RMW_Connext_RequestReplyMessage * const rr_msg,
    DDS_SampleIdentity_t * const sample_identity,
    DDS_SampleIdentity_t * const related_sample_identity);

  DDS_Topic * dds_topic() const
  {
    return DDS_DataWriter_get_topic(this->dds_writer);
  }

  DDS_Publisher * dds_publisher() const
  {
    return DDS_DataWriter_get_publisher(this->dds_writer);
  }

  DDS_DomainParticipant * dds_participant() const
  {
    DDS_Publisher * const pub = this->dds_publisher();

    return DDS_Publisher_get_participant(pub);
  }

private:
  rmw_context_impl_t * ctx;
  DDS_DataWriter * dds_writer;
  RMW_Connext_MessageTypeSupport * type_support;
  const bool created_topic;
  rmw_gid_t ros_gid;
  RMW_Connext_PublisherStatusCondition status_condition;

  RMW_Connext_Publisher(
    rmw_context_impl_t * const ctx,
    DDS_DataWriter * const dds_writer,
    RMW_Connext_MessageTypeSupport * const type_support,
    const bool created_topic);
};

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
  const bool internal = false);

rmw_ret_t
rmw_connextdds_destroy_publisher(
  rmw_context_impl_t * const ctx,
  rmw_publisher_t * const pub);

/******************************************************************************
 * Subscription support
 ******************************************************************************/

class RMW_Connext_Subscriber
{
public:
  static RMW_Connext_Subscriber *
  create(
    rmw_context_impl_t * const ctx,
    DDS_DomainParticipant * const dp,
    DDS_Subscriber * const sub,
    const rosidl_message_type_support_t * const type_supports,
    const char * const topic_name,
    const rmw_qos_profile_t * const qos_policies,
    const rmw_subscription_options_t * const subscriber_options,
    const bool internal = false,
    const RMW_Connext_MessageType msg_type = RMW_CONNEXT_MESSAGE_USERDATA,
    const void * const intro_members = nullptr,
    const bool intro_members_cpp = false,
    std::string * const type_name = nullptr,
    const char * const cft_name = nullptr,
    const char * const cft_filter = nullptr);

  rmw_ret_t
  finalize();

  DDS_DataReader *
  reader() const
  {
    return this->dds_reader;
  }

  RMW_Connext_UntypedSampleSeq *
  data_seq()
  {
    return &this->loan_data;
  }
  DDS_SampleInfoSeq *
  info_seq()
  {
    return &this->loan_info;
  }

  RMW_Connext_MessageTypeSupport *
  message_type_support() const
  {
    return this->type_support;
  }

  DDS_InstanceHandle_t
  instance_handle()
  {
    return DDS_Entity_get_instance_handle(
      DDS_DataReader_as_entity(this->reader()));
  }

  DDS_InstanceHandle_t
  participant_instance_handle()
  {
    return DDS_Entity_get_instance_handle(
      DDS_DomainParticipant_as_entity(
        DDS_Subscriber_get_participant(
          DDS_DataReader_get_subscriber(this->reader()))));
  }

  rmw_ret_t
  enable() const
  {
    if (DDS_RETCODE_OK !=
      DDS_Entity_enable(DDS_Topic_as_entity(this->dds_topic)))
    {
      RMW_CONNEXT_LOG_ERROR_A_SET(
        "failed to enable dds reader's topic: '%s' [%s]",
        DDS_TopicDescription_get_name(
          DDS_Topic_as_topicdescription(this->dds_topic)),
        this->type_support->type_name())
      return RMW_RET_ERROR;
    }

    if (DDS_RETCODE_OK !=
      DDS_Entity_enable(DDS_DataReader_as_entity(this->dds_reader)))
    {
      RMW_CONNEXT_LOG_ERROR_A_SET(
        "failed to enable dds reader: '%s' [%s]",
        DDS_TopicDescription_get_name(
          DDS_Topic_as_topicdescription(this->dds_topic)),
        this->type_support->type_name())
      return RMW_RET_ERROR;
    }

    return RMW_RET_OK;
  }

  RMW_Connext_SubscriberStatusCondition *
  condition()
  {
    return &this->status_condition;
  }

  const rmw_gid_t * gid() const
  {
    return &this->ros_gid;
  }

  size_t
  publications_count();

  rmw_ret_t
  qos(rmw_qos_profile_t * const qos);

  rmw_ret_t
  loan_messages(const bool update_condition = true);

  rmw_ret_t
  return_messages();

  rmw_ret_t
  loan_messages_if_needed(const bool update_condition = true)
  {
    rmw_ret_t rc = RMW_RET_OK;

    // take messages from reader if we don't have an outstanding loan
    // or if we have consumed the current loan
    if (this->loan_len == 0 || this->loan_next >= this->loan_len) {
      /* return any previously loaned messages */
      if (this->loan_len > 0) {
        rc = this->return_messages();
        if (RMW_RET_OK != rc) {
          return rc;
        }
      }
      /* loan messages from reader */
      rc = this->loan_messages(update_condition);
      if (RMW_RET_OK != rc) {
        return rc;
      }
    }

    return RMW_RET_OK;
  }

  void
  requestreply_header_from_dds(
    RMW_Connext_RequestReplyMessage * const rr_msg,
    const DDS_SampleIdentity_t * const sample_identity,
    const DDS_SampleIdentity_t * const related_sample_identity);

  rmw_ret_t
  take_next(
    void ** const ros_messages,
    rmw_message_info_t * const message_infos,
    const size_t max_samples,
    size_t * const taken,
    const bool serialized,
    const DDS_InstanceHandle_t * const request_writer_handle = nullptr);

  rmw_ret_t
  take_message(
    void * const ros_message,
    rmw_message_info_t * const message_info,
    bool * const taken,
    const DDS_InstanceHandle_t * const request_writer_handle = nullptr);

  rmw_ret_t
  take(
    rmw_message_sequence_t * const message_sequence,
    rmw_message_info_sequence_t * const message_info_sequence,
    const size_t max_samples,
    size_t * const taken);

  rmw_ret_t
  take_serialized(
    rmw_serialized_message_t * const serialized_message,
    rmw_message_info_t * const message_info,
    bool * const taken);

  rmw_ret_t
  set_content_filter(
    const rmw_subscription_content_filter_options_t * const options);

  rmw_ret_t
  get_content_filter(
    rcutils_allocator_t * allocator,
    rmw_subscription_content_filter_options_t * const options);

  bool
  has_data()
  {
    std::lock_guard<std::mutex> lock(this->loan_mutex);
    if (RMW_RET_OK !=
      this->loan_messages_if_needed(false /* update_condition */))
    {
      RMW_CONNEXT_LOG_ERROR("failed to check loaned messages")
      return false;
    }
    bool has_data = this->loan_len > 0;
    RMW_CONNEXT_ASSERT(!has_data || this->loan_next < this->loan_len)
    return has_data;
  }

  rmw_ret_t
  count_unread_samples(size_t & unread_count)
  {
    // The action of counting unread samples is not currently mutually exclusive
    // with the action of taking samples out of the reader cache. Unfortunately
    // we cannot use a mutex to synchronize calls between
    // count_unread_samples() and and take_next() because we would run the risk
    // of a deadlock. This is because count_unread_samples() is supposed to be
    // called from within the reader's "exclusive area" (since it is
    // executed within a listener callback), while take_next() is usually called
    // from an executor/application thread and it must acquire the reader's
    // "exclusive area" before taking samples.
    // This might mean that an application which relies on data callbacks and the
    // count provided by this function, and which at the same time polls data
    // on the subscription, might end up missing some samples in the total count
    // notified to it via callback because the samples were taken out of the
    // cache before they could be notified to the listener.
    // Fortunately, in Connext the listener callback is notified right after a
    // sample is added to the reader cache and before any mutex is released,
    // which should allow this function to always report a correct number, as
    // long as it is only called within a (DDS) listener callback.
    return rmw_connextdds_count_unread_samples(this, unread_count);
  }

  DDS_Subscriber * dds_subscriber() const
  {
    return DDS_DataReader_get_subscriber(this->dds_reader);
  }

  DDS_DomainParticipant * dds_participant() const
  {
    DDS_Subscriber * const sub = this->dds_subscriber();

    return DDS_Subscriber_get_participant(sub);
  }

  DDS_Topic * topic() const
  {
    return this->dds_topic;
  }

  static std::string get_atomic_id()
  {
    static std::atomic_uint64_t id;
    return std::to_string(id++);
  }

  bool is_cft_enabled()
  {
    return !this->cft_expression.empty();
  }

  const bool internal;
  const bool ignore_local;

private:
  rmw_context_impl_t * ctx;
  DDS_DataReader * dds_reader;
  DDS_Topic * dds_topic;
  DDS_TopicDescription * dds_topic_cft;
  std::string cft_expression;
  RMW_Connext_MessageTypeSupport * type_support;
  rmw_gid_t ros_gid;
  const bool created_topic;
  RMW_Connext_SubscriberStatusCondition status_condition;
  RMW_Connext_UntypedSampleSeq loan_data;
  DDS_SampleInfoSeq loan_info;
  size_t loan_len;
  size_t loan_next;
  std::mutex loan_mutex;

  RMW_Connext_Subscriber(
    rmw_context_impl_t * const ctx,
    DDS_DataReader * const dds_reader,
    DDS_Topic * const dds_topic,
    RMW_Connext_MessageTypeSupport * const type_support,
    const bool ignore_local,
    const bool created_topic,
    DDS_TopicDescription * const dds_topic_cft,
    const char * const cft_expression,
    const bool internal);

  friend class RMW_Connext_SubscriberStatusCondition;
};

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
  const bool internal = false);

rmw_ret_t
rmw_connextdds_destroy_subscriber(
  rmw_context_impl_t * const ctx,
  rmw_subscription_t * const sub);

void rmw_connextdds_message_info_from_dds(
  rmw_message_info_t * const to,
  const DDS_SampleInfo * const from);

/******************************************************************************
 * Client/Service support
 ******************************************************************************/

class RMW_Connext_Client
{
  RMW_Connext_Publisher * request_pub;
  RMW_Connext_Subscriber * reply_sub;
  std::atomic_uint next_request_id;
  rmw_context_impl_t * ctx;

  RMW_Connext_Client()
  : request_pub(nullptr),
    reply_sub(nullptr),
    next_request_id(1)
  {
  }

public:
  static RMW_Connext_Client *
  create(
    rmw_context_impl_t * const ctx,
    DDS_DomainParticipant * const dp,
    DDS_Publisher * const pub,
    DDS_Subscriber * const sub,
    const rosidl_service_type_support_t * const type_supports,
    const char * const svc_name,
    const rmw_qos_profile_t * const qos_policies);

  rmw_ret_t
  finalize();

  rmw_ret_t
  is_service_available(bool & available);

  rmw_ret_t
  take_response(
    rmw_service_info_t * const request_header,
    void * const ros_response,
    bool * const taken);

  rmw_ret_t
  send_request(
    const void * const ros_request,
    int64_t * const sequence_id);

  RMW_Connext_Publisher *
  publisher() const
  {
    return this->request_pub;
  }

  RMW_Connext_Subscriber *
  subscriber() const
  {
    return this->reply_sub;
  }

  rmw_ret_t
  enable();

  rmw_ret_t
  request_publisher_qos(rmw_qos_profile_t * const qos);

  rmw_ret_t
  response_subscription_qos(rmw_qos_profile_t * const qos);
};

class RMW_Connext_Service
{
  RMW_Connext_Publisher * reply_pub;
  RMW_Connext_Subscriber * request_sub;
  rmw_context_impl_t * ctx;

public:
  static RMW_Connext_Service *
  create(
    rmw_context_impl_t * const ctx,
    DDS_DomainParticipant * const dp,
    DDS_Publisher * const pub,
    DDS_Subscriber * const sub,
    const rosidl_service_type_support_t * const type_supports,
    const char * const svc_name,
    const rmw_qos_profile_t * const qos_policies);

  rmw_ret_t
  finalize();

  rmw_ret_t
  take_request(
    rmw_service_info_t * const request_header,
    void * const ros_request,
    bool * const taken);

  rmw_ret_t
  send_response(
    rmw_request_id_t * const request_id,
    const void * const ros_response);

  RMW_Connext_Publisher *
  publisher() const
  {
    return this->reply_pub;
  }

  RMW_Connext_Subscriber *
  subscriber() const
  {
    return this->request_sub;
  }

  rmw_ret_t
  enable();

  rmw_ret_t
  response_publisher_qos(rmw_qos_profile_t * const qos);

  rmw_ret_t
  request_subscription_qos(rmw_qos_profile_t * const qos);
};

/******************************************************************************
 * Waitsets and Conditions
 ******************************************************************************/

rmw_guard_condition_t *
rmw_connextdds_create_guard_condition(const bool internal);

rmw_ret_t
rmw_connextdds_destroy_guard_condition(rmw_guard_condition_t * const gc);

rmw_ret_t
rmw_connextdds_trigger_guard_condition(const rmw_guard_condition_t * const gc);

rmw_wait_set_t *
rmw_connextdds_create_waitset(const size_t max_conditions);

rmw_ret_t
rmw_connextdds_destroy_waitset(rmw_wait_set_t * const ws);

rmw_ret_t
rmw_connextdds_waitset_wait(
  rmw_wait_set_t * const ws,
  rmw_subscriptions_t * subs,
  rmw_guard_conditions_t * gcs,
  rmw_services_t * srvs,
  rmw_clients_t * cls,
  rmw_events_t * evs,
  const rmw_time_t * wait_timeout);

/******************************************************************************
 * GUID helpers
 ******************************************************************************/

rmw_ret_t
rmw_connextdds_gid_to_guid(const rmw_gid_t & gid, struct DDS_GUID_t & guid);

rmw_ret_t
rmw_connextdds_guid_to_gid(const struct DDS_GUID_t & guid, rmw_gid_t & gid);

void rmw_connextdds_sn_ros_to_dds(
  const int64_t sn_ros, struct DDS_SequenceNumber_t & sn_dds);

#define rmw_connextdds_sn_ros_to_dds(sn_ros_, sn_dds_) \
  { \
    (sn_dds_).high = ((sn_ros_) & 0xFFFFFFFF00000000LL) >> 32; \
    (sn_dds_).low = (sn_ros_) & 0x00000000FFFFFFFFLL; \
  }

void rmw_connextdds_sn_dds_to_ros(
  const struct DDS_SequenceNumber_t & sn_dds, int64_t & sn_ros);

#define rmw_connextdds_sn_dds_to_ros(sn_dds_, sn_ros_) \
  { \
    sn_ros_ = ((int64_t)(sn_dds_).high) << 32 | (sn_dds_).low; \
  }

void rmw_connextdds_ih_to_gid(
  const DDS_InstanceHandle_t & ih, rmw_gid_t & gid);

void rmw_connextdds_get_entity_gid(
  DDS_Entity * const entity, rmw_gid_t & gid);

void rmw_connextdds_get_entity_gid(
  DDS_DomainParticipant * const dp, rmw_gid_t & gid);

void rmw_connextdds_get_entity_gid(
  DDS_Publisher * const pub, rmw_gid_t & gid);

void rmw_connextdds_get_entity_gid(
  DDS_Subscriber * const sub, rmw_gid_t & gid);

void rmw_connextdds_get_entity_gid(
  DDS_DataWriter * const writer, rmw_gid_t & gid);

void rmw_connextdds_get_entity_gid(
  DDS_DataReader * const reader, rmw_gid_t & gid);

void rmw_connextdds_get_entity_gid(
  DDS_Topic * const topic, rmw_gid_t & gid);

/******************************************************************************
 * Type helpers
 ******************************************************************************/
std::string
rmw_connextdds_create_type_name(
  const message_type_support_callbacks_t * callbacks,
  const bool mangle_names = true);

std::string
rmw_connextdds_create_type_name(
  const rosidl_typesupport_introspection_cpp::MessageMembers * const members,
  const bool mangle_names = true);

std::string
rmw_connextdds_create_type_name(
  const rosidl_typesupport_introspection_c__MessageMembers * const members,
  const bool mangle_names = true);

std::string
rmw_connextdds_create_type_name_request(
  const service_type_support_callbacks_t * callbacks,
  const bool mangle_names = true);

std::string
rmw_connextdds_create_type_name_response(
  const service_type_support_callbacks_t * callbacks,
  const bool mangle_names = true);

/******************************************************************************
 * Topic Helpers
 ******************************************************************************/
std::string
rmw_connextdds_create_topic_name(
  const char * prefix,
  const char * topic_name,
  const char * suffix,
  bool avoid_ros_namespace_conventions);

std::string
rmw_connextdds_create_topic_name(
  const char * prefix,
  const char * topic_name,
  const char * suffix,
  const rmw_qos_profile_t * qos_policies);

/******************************************************************************
 * Qos Helpers
 ******************************************************************************/

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
  const rmw_subscription_options_t * const sub_options);

rmw_ret_t
rmw_connextdds_readerwriter_qos_to_ros(
  const DDS_HistoryQosPolicy * const history,
  const DDS_ReliabilityQosPolicy * const reliability,
  const DDS_DurabilityQosPolicy * const durability,
  const DDS_DeadlineQosPolicy * const deadline,
  const DDS_LivelinessQosPolicy * const liveliness,
  const DDS_LifespanQosPolicy * const lifespan,
  rmw_qos_profile_t * const qos_policies);

/******************************************************************************
 * Security Helpers
 ******************************************************************************/
rmw_ret_t
rmw_connextdds_apply_security_logging_configuration(
  DDS_PropertyQosPolicy * const properties);

#endif  // RMW_CONNEXTDDS__RMW_IMPL_HPP_
