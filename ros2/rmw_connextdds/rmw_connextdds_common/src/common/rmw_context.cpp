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

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "rmw_connextdds/rmw_impl.hpp"
#include "rmw_connextdds/discovery.hpp"
#include "rmw_connextdds/graph_cache.hpp"

#include "rmw_dds_common/security.hpp"

#include "rcutils/env.h"
#include "rcutils/filesystem.h"

/******************************************************************************
 * Global reference to the Domain Participant Factory.
 * The first context to be initialized will set this reference, and the first
 * context to be finalized while all nodes have been deleted from all created
 * contexts will reset it (to signal others that the DPF has been finalized).
 * Note that because access to this variable is not synchronized, creation of
 * RMW contexts is not thread safe (TODO(asorbini) verify that this is true in
 * the rcl/rclcpp API too).
 ******************************************************************************/
DDS_DomainParticipantFactory * RMW_Connext_gv_DomainParticipantFactory = nullptr;
size_t RMW_Connext_gv_ContextCount = 0;

/******************************************************************************
 * Context Implementation
 ******************************************************************************/

rmw_ret_t
rmw_connextdds_initialize_participant_factory_qos(
  rmw_context_impl_t * const ctx)
{
  UNUSED_ARG(ctx);

  DDS_DomainParticipantFactoryQos qos =
    DDS_DomainParticipantFactoryQos_INITIALIZER;

  if (DDS_RETCODE_OK !=
    DDS_DomainParticipantFactory_get_qos(
      RMW_Connext_gv_DomainParticipantFactory, &qos))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to get participant factory qos")
    return RMW_RET_ERROR;
  }

  qos.entity_factory.autoenable_created_entities = DDS_BOOLEAN_FALSE;

  if (DDS_RETCODE_OK !=
    DDS_DomainParticipantFactory_set_qos(
      RMW_Connext_gv_DomainParticipantFactory, &qos))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to get participant factory qos")
    DDS_DomainParticipantFactoryQos_finalize(&qos);
    return RMW_RET_ERROR;
  }

  DDS_DomainParticipantFactoryQos_finalize(&qos);
  return RMW_RET_OK;
}

static
rmw_ret_t
rmw_connextdds_initialize_participant_qos(
  rmw_context_impl_t * const ctx,
  DDS_DomainParticipantQos & dp_qos)
{
  RMW_CONNEXT_ASSERT(nullptr != RMW_Connext_gv_DomainParticipantFactory)
  if (DDS_RETCODE_OK !=
    DDS_DomainParticipantFactory_get_default_participant_qos(
      RMW_Connext_gv_DomainParticipantFactory, &dp_qos))
  {
    return RMW_RET_ERROR;
  }

  if (RMW_RET_OK !=
    rmw_connextdds_initialize_participant_qos_impl(ctx, &dp_qos))
  {
    return RMW_RET_ERROR;
  }

  if (ctx->participant_qos_override_policy ==
    rmw_context_impl_t::participant_qos_override_policy_t::All &&
    DDS_StringSeq_get_length(&ctx->initial_peers) > 0)
  {
    if (!DDS_StringSeq_copy(&dp_qos.discovery.initial_peers, &ctx->initial_peers)) {
      RMW_CONNEXT_LOG_ERROR_SET("failed to copy initial peers sequence")
      return RMW_RET_ERROR;
    }
  }

  return RMW_RET_OK;
}

rmw_ret_t
rmw_context_impl_t::initialize_node(
  const char * const node_name,
  const char * const node_namespace,
  const bool localhost_only)
{
  UNUSED_ARG(node_name);
  UNUSED_ARG(node_namespace);

  RMW_CONNEXT_LOG_DEBUG_A(
    "initializing new node: total=%lu, localhost=%d",
    this->node_count, localhost_only)

  if (0u != this->node_count) {
    if ((this->localhost_only && !localhost_only) ||
      (!this->localhost_only && localhost_only))
    {
      RMW_CONNEXT_LOG_ERROR_A_SET(
        "incompatible node for context:"
        "ctx.localhost_only=%d, node.localhost_only=%d",
        this->localhost_only, localhost_only)
      return RMW_RET_ERROR;
    }

    this->node_count += 1;
    RMW_CONNEXT_LOG_DEBUG_A(
      "initialized new node: total=%lu", this->node_count)
    return RMW_RET_OK;
  }

  rmw_ret_t rc = this->initialize_participant(localhost_only);
  if (RMW_RET_OK != rc) {
    RMW_CONNEXT_LOG_ERROR("failed to initialize DomainParticipant")
    return rc;
  }

  rc = this->enable_participant();
  if (RMW_RET_OK != rc) {
    RMW_CONNEXT_LOG_ERROR("failed to enable DomainParticipant")
    if (RMW_RET_OK != this->finalize_participant()) {
      RMW_CONNEXT_LOG_ERROR("failed to finalize participant on error")
    }
    return rc;
  }

  this->node_count = 1;

  return RMW_RET_OK;
}

rmw_ret_t
rmw_context_impl_t::initialize_participant(const bool localhost_only)
{
  RMW_CONNEXT_LOG_DEBUG("initializing DDS DomainParticipant")

  this->localhost_only = localhost_only;

  if (nullptr == RMW_Connext_gv_DomainParticipantFactory) {
    RMW_CONNEXT_LOG_ERROR("DDS DomainParticipantFactory not initialized")
    return RMW_RET_ERROR;
  }

  DDS_DomainId_t domain_id =
    static_cast<DDS_DomainId_t>(this->domain_id);

  struct DDS_DomainParticipantQos dp_qos =
    DDS_DomainParticipantQos_INITIALIZER;

  std::unique_ptr<DDS_DomainParticipantQos, std::function<void(DDS_DomainParticipantQos *)>>
  dp_qos_guard(&dp_qos, &DDS_DomainParticipantQos_finalize);
  if (nullptr == dp_qos_guard) {
    return RMW_RET_ERROR;
  }

  rmw_context_impl_t * const ctx = this;
  auto scope_exit_dp_finalize = rcpputils::make_scope_exit(
    [ctx]()
    {
      if (RMW_RET_OK != ctx->finalize_participant()) {
        RMW_CONNEXT_LOG_ERROR(
          "failed to finalize participant on error")
      }
    });

  if (RMW_RET_OK !=
    rmw_connextdds_initialize_participant_qos(this, dp_qos))
  {
    RMW_CONNEXT_LOG_ERROR("failed to initialize participant qos")
    return RMW_RET_ERROR;
  }

  if (RMW_RET_OK != rmw_connextdds_configure_security(this, &dp_qos)) {
    RMW_CONNEXT_LOG_ERROR("failed to configure DDS Security")
    return RMW_RET_ERROR;
  }

  RMW_CONNEXT_LOG_DEBUG_A(
    "creating DomainParticipant: "
    "domain=%d", domain_id)

  this->participant = DDS_DomainParticipantFactory_create_participant(
    RMW_Connext_gv_DomainParticipantFactory,
    domain_id,
    &dp_qos,
    nullptr,
    DDS_STATUS_MASK_NONE);
  if (nullptr == this->participant) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to create DDS participant")
    return RMW_RET_ERROR;
  }

  rmw_ret_t cfg_rc = rmw_connextdds_configure_participant(this, this->participant);
  if (RMW_RET_OK != cfg_rc) {
    RMW_CONNEXT_LOG_ERROR("failed to configure DDS participant")
    return cfg_rc;
  }

  /* Create DDS publisher/subscriber objects that will be used for all DDS
     writers/readers created to support RMW publishers/subscriptions. */

  DDS_PublisherQos pub_qos = DDS_PublisherQos_INITIALIZER;

  std::unique_ptr<DDS_PublisherQos, std::function<void(DDS_PublisherQos *)>>
  pub_qos_guard(&pub_qos, &DDS_PublisherQos_finalize);
  if (nullptr == pub_qos_guard) {
    return RMW_RET_ERROR;
  }

  if (DDS_RETCODE_OK !=
    DDS_DomainParticipant_get_default_publisher_qos(
      this->participant, &pub_qos))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to get default Publisher QoS")
    return RMW_RET_ERROR;
  }

  pub_qos.entity_factory.autoenable_created_entities = DDS_BOOLEAN_FALSE;

  RMW_CONNEXT_LOG_DEBUG("creating default DDS Publisher")

  this->dds_pub = DDS_DomainParticipant_create_publisher(
    this->participant,
    &pub_qos,
    NULL,
    DDS_STATUS_MASK_NONE);

  if (nullptr == this->dds_pub) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to create DDS publisher")
    return RMW_RET_ERROR;
  }

  DDS_SubscriberQos sub_qos = DDS_SubscriberQos_INITIALIZER;

  std::unique_ptr<DDS_SubscriberQos, std::function<void(DDS_SubscriberQos *)>>
  sub_qos_guard(&sub_qos, &DDS_SubscriberQos_finalize);
  if (nullptr == sub_qos_guard) {
    return RMW_RET_ERROR;
  }

  if (DDS_RETCODE_OK !=
    DDS_DomainParticipant_get_default_subscriber_qos(
      this->participant, &sub_qos))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to get default Subscriber QoS")
    return RMW_RET_ERROR;
  }

  sub_qos.entity_factory.autoenable_created_entities = DDS_BOOLEAN_FALSE;

  RMW_CONNEXT_LOG_DEBUG("creating default DDS Subscriber")

  this->dds_sub = DDS_DomainParticipant_create_subscriber(
    this->participant,
    &sub_qos,
    NULL,
    DDS_STATUS_MASK_NONE);

  if (nullptr == this->dds_sub) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to create DDS subscriber")
    return RMW_RET_ERROR;
  }

  if (RMW_RET_OK != rmw_connextdds_graph_initialize(this)) {
    RMW_CONNEXT_LOG_ERROR("failed to initialize graph cache")
    return RMW_RET_ERROR;
  }

  scope_exit_dp_finalize.cancel();

  RMW_CONNEXT_LOG_DEBUG("DDS DomainParticipant initialized")

  return RMW_RET_OK;
}

rmw_ret_t
rmw_context_impl_t::enable_participant()
{
  if (DDS_RETCODE_OK !=
    DDS_Entity_enable(
      DDS_DomainParticipant_as_entity(this->participant)))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to enable participant")
    return RMW_RET_ERROR;
  }

  if (DDS_RETCODE_OK !=
    DDS_Entity_enable(DDS_Subscriber_as_entity(this->dds_sub)))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to enable dds subscriber")
    return RMW_RET_ERROR;
  }

  if (DDS_RETCODE_OK !=
    DDS_Entity_enable(DDS_Publisher_as_entity(this->dds_pub)))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to enable dds subscriber")
    return RMW_RET_ERROR;
  }

  if (RMW_RET_OK != rmw_connextdds_graph_enable(this)) {
    RMW_CONNEXT_LOG_ERROR("failed to enable graph cache")
    return RMW_RET_ERROR;
  }

  return RMW_RET_OK;
}

rmw_ret_t
rmw_context_impl_t::finalize_participant()
{
  RMW_CONNEXT_LOG_DEBUG("finalizing DDS DomainParticipant")
#if RMW_CONNEXT_DEBUG && RMW_CONNEXT_DDS_API == RMW_CONNEXT_DDS_API_PRO
  // If we are building in Debug mode, an issue in Connext may prevent the
  // participant from being able to delete any content-filtered topic if
  // the participant has not been enabled.
  // For this reason, make sure to enable the participant before trying to
  // finalize it.
  // TODO(asorbini) reconsider the need for this code in Connext > 6.1.0
  if (DDS_RETCODE_OK !=
    DDS_Entity_enable(DDS_DomainParticipant_as_entity(participant)))
  {
    RMW_CONNEXT_LOG_ERROR_SET(
      "failed to enable DomainParticipant before deletion")
    return RMW_RET_ERROR;
  }
#endif  // RMW_CONNEXT_DEBUG && RMW_CONNEXT_DDS_API == RMW_CONNEXT_DDS_API_PRO
  if (RMW_RET_OK != rmw_connextdds_graph_finalize(this)) {
    RMW_CONNEXT_LOG_ERROR("failed to finalize graph cache")
    return RMW_RET_ERROR;
  }

  if (nullptr != this->dds_pub) {
    // If we are cleaning up after some RMW failure, it is possible for some
    // DataWriter to not have been deleted.
    // Call DDS_Publisher_delete_contained_entities() to make sure we can
    // dispose the publisher.
    if (DDS_RETCODE_OK !=
      DDS_Publisher_delete_contained_entities(this->dds_pub))
    {
      RMW_CONNEXT_LOG_ERROR_SET("failed to delete DDS publisher's entities")
      return RMW_RET_ERROR;
    }

    if (DDS_RETCODE_OK !=
      DDS_DomainParticipant_delete_publisher(
        this->participant, this->dds_pub))
    {
      RMW_CONNEXT_LOG_ERROR_SET("failed to delete DDS publisher")
      return RMW_RET_ERROR;
    }
    this->dds_pub = nullptr;
  }

  if (nullptr != this->dds_sub) {
    // If we are cleaning up after some RMW failure, it is possible for some
    // DataReader to not have been deleted.
    // Call DDS_Subscriber_delete_contained_entities() to make sure we can
    // dispose the publisher.
    if (DDS_RETCODE_OK !=
      DDS_Subscriber_delete_contained_entities(this->dds_sub))
    {
      RMW_CONNEXT_LOG_ERROR_SET("failed to delete DDS subscriber's entities")
      return RMW_RET_ERROR;
    }


    if (DDS_RETCODE_OK !=
      DDS_DomainParticipant_delete_subscriber(
        this->participant, this->dds_sub))
    {
      RMW_CONNEXT_LOG_ERROR_SET("failed to delete DDS subscriber")
      return RMW_RET_ERROR;
    }
    this->dds_sub = nullptr;
  }

  if (nullptr != this->participant) {
    // If we are cleaning up after some RMW failure, it is possible for some
    // DataWriter to not have been deleted.
    // Call DDS_DomainParticipant_delete_contained_entities() to make sure we can
    // dispose the publisher.
    if (DDS_RETCODE_OK !=
      DDS_DomainParticipant_delete_contained_entities(this->participant))
    {
      RMW_CONNEXT_LOG_ERROR_SET("failed to delete DDS participant's entities")
      return RMW_RET_ERROR;
    }

    if (DDS_RETCODE_OK !=
      DDS_DomainParticipantFactory_delete_participant(
        RMW_Connext_gv_DomainParticipantFactory, this->participant))
    {
      RMW_CONNEXT_LOG_ERROR_SET("failed to delete DDS participant")
      return RMW_RET_ERROR;
    }

    this->participant = nullptr;
  }

  RMW_CONNEXT_LOG_DEBUG("DDS DomainParticipant finalized")
  return RMW_RET_OK;
}

rmw_ret_t
rmw_context_impl_t::finalize()
{
  rmw_ret_t rc_exit = RMW_RET_OK;

  RMW_CONNEXT_LOG_DEBUG_A(
    "finalizing RMW context: %p",
    reinterpret_cast<void *>(this))

  RMW_CONNEXT_ASSERT(RMW_Connext_gv_ContextCount > 0)
  RMW_Connext_gv_ContextCount -= 1;

  if (0 == RMW_Connext_gv_ContextCount) {
    if (RMW_RET_OK != rmw_connextdds_finalize_participant_factory_context(this)) {
      RMW_CONNEXT_LOG_ERROR("failed to finalize participant factory")
      rc_exit = RMW_RET_ERROR;
    }

    if (DDS_RETCODE_OK != DDS_DomainParticipantFactory_finalize_instance()) {
      RMW_CONNEXT_LOG_ERROR_SET("failed to finalize domain participant factory")
      rc_exit = RMW_RET_ERROR;
    }
    RMW_Connext_gv_DomainParticipantFactory = nullptr;
  }

  RMW_CONNEXT_LOG_DEBUG_A(
    "RMW context finalized: %p",
    reinterpret_cast<void *>(this))

  return rc_exit;
}

rmw_ret_t
rmw_context_impl_t::finalize_node()
{
  RMW_CONNEXT_LOG_DEBUG_A(
    "finalizing node: total=%lu", this->node_count)
  this->node_count -= 1;
  if (0u != this->node_count) {
    // destruction shouldn't happen yet
    RMW_CONNEXT_LOG_DEBUG_A(
      "finalized node: total=%lu", this->node_count)
    return RMW_RET_OK;
  }

  return this->finalize_participant();
}

uint32_t
rmw_context_impl_t::next_client_id()
{
  const uint32_t res = this->client_service_id;
  this->client_service_id += 1;
  if (0 == this->client_service_id) {
    RMW_CONNEXT_LOG_WARNING("rollover detected in client IDs")
  }
  return res;
}

rmw_ret_t
rmw_context_impl_t::assert_topic(
  DDS_DomainParticipant * const participant,
  const char * const topic_name,
  const char * const type_name,
  const bool internal,
  DDS_Topic ** const topic,
  bool & created)
{
  DDS_TopicDescription * const topic_existing =
    DDS_DomainParticipant_lookup_topicdescription(participant, topic_name);

  if (nullptr != topic_existing) {
    if (internal) {
      /* topics for "internal" endpoints are created while the participant
         is still disabled, so find_topic() cannot be called.
         Instead, we cast the topicdescription to a topic. The deletion
         path will have to keep track that the endpoint didn't create
         its topic, so not to try to delete it */
      *topic = DDS_Topic_narrow(topic_existing);
      created = false;
    } else {
      *topic =
        DDS_DomainParticipant_find_topic(
        participant,
        DDS_TopicDescription_get_name(topic_existing),
        &DDS_DURATION_ZERO);
      if (nullptr == *topic) {
        RMW_CONNEXT_LOG_ERROR_SET("failed to find topic from description")
        return RMW_RET_ERROR;
      }
      created = true;
    }
    RMW_CONNEXT_LOG_DEBUG_A(
      "found topic: name=%s, type=%s\n",
      topic_name, type_name);
  } else {
    *topic = DDS_DomainParticipant_create_topic(
      participant,
      topic_name,
      type_name,
      &DDS_TOPIC_QOS_DEFAULT,
      nullptr,
      DDS_STATUS_MASK_NONE);
    if (nullptr == *topic) {
      RMW_CONNEXT_LOG_ERROR_SET("failed to create reader's topic")
      return RMW_RET_ERROR;
    }
    created = true;
    RMW_CONNEXT_LOG_DEBUG_A(
      "created topic: name=%s, type=%s\n",
      topic_name, type_name);
  }

  if (!internal) {
    if (DDS_RETCODE_OK != DDS_Entity_enable(DDS_Topic_as_entity(*topic))) {
      RMW_CONNEXT_LOG_ERROR_SET("failed to enable topic")
      return RMW_RET_ERROR;
    }
  }

  return RMW_RET_OK;
}

/******************************************************************************
 * Context interface functions
 ******************************************************************************/
const char *
rmw_api_connextdds_get_implementation_identifier()
{
  return RMW_CONNEXTDDS_ID;
}


const char *
rmw_api_connextdds_get_serialization_format()
{
  return RMW_CONNEXTDDS_SERIALIZATION_FORMAT;
}


rmw_ret_t
rmw_api_connextdds_set_log_severity(rmw_log_severity_t severity)
{
  return rmw_connextdds_set_log_verbosity(severity);
}


rmw_ret_t
rmw_api_connextdds_init_options_init(
  rmw_init_options_t * init_options,
  rcutils_allocator_t allocator)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(init_options, RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ALLOCATOR(&allocator, return RMW_RET_INVALID_ARGUMENT);
  if (NULL != init_options->implementation_identifier) {
    RMW_CONNEXT_LOG_ERROR_SET("expected zero-initialized init_options")
    return RMW_RET_INVALID_ARGUMENT;
  }
  init_options->instance_id = 0;
  init_options->implementation_identifier = RMW_CONNEXTDDS_ID;
  init_options->allocator = allocator;
  init_options->impl = nullptr;
  init_options->localhost_only = RMW_LOCALHOST_ONLY_DEFAULT;
  init_options->domain_id = RMW_DEFAULT_DOMAIN_ID;
  init_options->enclave = nullptr;
  init_options->security_options = rmw_get_zero_initialized_security_options();
  return RMW_RET_OK;
}


rmw_ret_t
rmw_api_connextdds_init_options_copy(
  const rmw_init_options_t * src,
  rmw_init_options_t * dst)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(src, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(dst, RMW_RET_INVALID_ARGUMENT);
  if (nullptr == src->implementation_identifier) {
    RMW_CONNEXT_LOG_ERROR_SET("expected initialized src")
    return RMW_RET_INVALID_ARGUMENT;
  }
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    src,
    src->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  if (nullptr != dst->implementation_identifier) {
    RMW_CONNEXT_LOG_ERROR_SET("expected zero-initialized dst")
    return RMW_RET_INVALID_ARGUMENT;
  }

  rmw_init_options_t tmp = *src;

  const rcutils_allocator_t * allocator = &src->allocator;
  tmp.enclave = rcutils_strdup(tmp.enclave, *allocator);
  if (nullptr != src->enclave && nullptr == tmp.enclave) {
    return RMW_RET_BAD_ALLOC;
  }
  tmp.security_options = rmw_get_zero_initialized_security_options();
  rmw_ret_t ret =
    rmw_security_options_copy(&src->security_options, allocator, &tmp.security_options);
  if (RMW_RET_OK != ret) {
    allocator->deallocate(tmp.enclave, allocator->state);
    return ret;
  }

  *dst = tmp;
  return RMW_RET_OK;
}


rmw_ret_t
rmw_api_connextdds_init_options_fini(rmw_init_options_t * init_options)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(init_options, RMW_RET_INVALID_ARGUMENT);
  if (nullptr == init_options->implementation_identifier) {
    RMW_CONNEXT_LOG_ERROR_SET("expected initialized init_options")
    return RMW_RET_INVALID_ARGUMENT;
  }
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    init_options,
    init_options->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  rcutils_allocator_t * allocator = &init_options->allocator;
  RCUTILS_CHECK_ALLOCATOR(allocator, return RMW_RET_INVALID_ARGUMENT);
  allocator->deallocate(init_options->enclave, allocator->state);
  rmw_ret_t ret = rmw_security_options_fini(&init_options->security_options, allocator);

  *init_options = rmw_get_zero_initialized_init_options();
  return ret;
}

static
rmw_ret_t
rmw_connextdds_parse_participant_qos_override_policy(
  const char * const user_input,
  rmw_context_impl_t::participant_qos_override_policy_t & policy)
{
  static const char pfx_never[] = "never";
  static const char pfx_all[] = "all";
  static const char pfx_basic[] = "basic";

  policy = rmw_context_impl_t::participant_qos_override_policy_t::All;

  if (0 == strcmp(user_input, pfx_never)) {
    policy = rmw_context_impl_t::participant_qos_override_policy_t::Never;
  } else if (0 == strcmp(user_input, pfx_basic)) {
    policy = rmw_context_impl_t::participant_qos_override_policy_t::Basic;
  } else if (user_input[0] != '\0' && strcmp(user_input, pfx_all) != 0) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "unexpected value for participant qos override policy. "
      "Allowed values are {all}, {basic}, or {never}: %s",
      user_input);
    return RMW_RET_ERROR;
  }

  return RMW_RET_OK;
}

static
rmw_ret_t
rmw_connextdds_parse_endpoint_qos_override_policy(
  const char * const user_input,
  rmw_context_impl_t::endpoint_qos_override_policy_t & policy,
  std::regex & policy_regex)
{
  static const char pfx_dds_topics[] = "dds_topics: ";
  static const size_t pfx_dds_topics_len = sizeof(pfx_dds_topics) - 1u;
  static const char pfx_never[] = "never";
  static const char pfx_always[] = "always";

  policy = rmw_context_impl_t::endpoint_qos_override_policy_t::Always;

  if (0 == strncmp(user_input, pfx_dds_topics, pfx_dds_topics_len)) {
    policy = rmw_context_impl_t::endpoint_qos_override_policy_t::DDSTopics;
    try {
      policy_regex = &user_input[pfx_dds_topics_len];
    } catch (std::regex_error & err) {
      RMW_CONNEXT_LOG_ERROR_A_SET(
        "failed to parse regex for endpoint qos override policy: %s",
        err.what());
      return RMW_RET_ERROR;
    }
  } else if (0 == strcmp(user_input, pfx_never)) {
    policy = rmw_context_impl_t::endpoint_qos_override_policy_t::Never;
  } else if (user_input[0] != '\0' && strcmp(user_input, pfx_always) != 0) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "unexpected value for endpoint qos override policy. "
      "Allowed values are {always}, {never} or {dds_topics: <regex_expression>}: %s",
      user_input);
    return RMW_RET_ERROR;
  }

  return RMW_RET_OK;
}

rmw_ret_t
rmw_api_connextdds_init(
  const rmw_init_options_t * options,
  rmw_context_t * context)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(options, RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(context, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_FOR_NULL_WITH_MSG(
    options->implementation_identifier,
    "expected initialized init options",
    return RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    options,
    options->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_FOR_NULL_WITH_MSG(
    options->enclave,
    "expected non-null enclave",
    return RMW_RET_INVALID_ARGUMENT);
  if (nullptr != context->implementation_identifier) {
    RMW_CONNEXT_LOG_ERROR_SET("expected a zero-initialized context")
    return RMW_RET_INVALID_ARGUMENT;
  }

  if (options->domain_id >= INT32_MAX &&
    options->domain_id != RMW_DEFAULT_DOMAIN_ID)
  {
    RMW_CONNEXT_LOG_ERROR_SET("domain id out of range")
    return RMW_RET_INVALID_ARGUMENT;
  }

  auto scope_exit_context_reset = rcpputils::make_scope_exit(
    [context]()
    {
      *context = rmw_get_zero_initialized_context();
    });

  context->instance_id = options->instance_id;
  context->implementation_identifier = RMW_CONNEXTDDS_ID;
  const DDS_DomainId_t actual_domain_id =
    (RMW_DEFAULT_DOMAIN_ID != options->domain_id) ?
    static_cast<DDS_DomainId_t>(options->domain_id) : RMW_CONNEXT_DEFAULT_DOMAIN;
  context->actual_domain_id = actual_domain_id;
  rmw_ret_t rc = rmw_api_connextdds_init_options_copy(options, &context->options);
  if (RMW_RET_OK != rc) {
    RMW_CONNEXT_LOG_ERROR("failed to copy RMW context options")
    return rc;
  }

  auto scope_exit_context_opts_finalize =
    rcpputils::make_scope_exit(
    [context]()
    {
      if (RMW_RET_OK != rmw_api_connextdds_init_options_fini(&context->options)) {
        RMW_CONNEXT_LOG_ERROR("failed to finalize RMW context options")
      }
    });

  rmw_context_impl_t * const ctx = new (std::nothrow) rmw_context_impl_t(context);
  if (nullptr == ctx) {
    RMW_CONNEXT_LOG_ERROR_SET(
      "failed to allocate RMW context implementation")
    return RMW_RET_ERROR;
  }
  context->impl = ctx;

  // Increment the count early, since context->finalize() expects it
  // to have been already incremented.
  RMW_Connext_gv_ContextCount += 1;

  auto scope_exit_context_finalize =
    rcpputils::make_scope_exit(
    [ctx]()
    {
      if (RMW_RET_OK != ctx->finalize()) {
        RMW_CONNEXT_LOG_ERROR("failed to finalize RMW context")
      }
    });

  // TODO(asorbini) get rid of context->impl->domain_id, and just use
  // context->actual_domain_id in rmw_context_impl_t::initialize_node()
  ctx->domain_id = actual_domain_id;

  // All publishers will use asynchronous publish mode unless
  // RMW_CONNEXT_ENV_USE_DEFAULT_PUBLISH_MODE is set.
  const char * use_default_publish_mode_env = nullptr;
  const char * lookup_rc = rcutils_get_env(
    RMW_CONNEXT_ENV_USE_DEFAULT_PUBLISH_MODE, &use_default_publish_mode_env);

  if (nullptr != lookup_rc || nullptr == use_default_publish_mode_env) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to lookup from environment: "
      "var=%s, "
      "rc=%s ",
      RMW_CONNEXT_ENV_USE_DEFAULT_PUBLISH_MODE,
      lookup_rc)
    return RMW_RET_ERROR;
  }
  ctx->use_default_publish_mode = '\0' != use_default_publish_mode_env[0];

  // Check if the user specified a custom override policy for participant qos.
  const char * participant_qos_policy = nullptr;
  lookup_rc = rcutils_get_env(
    RMW_CONNEXT_ENV_PARTICIPANT_QOS_OVERRIDE_POLICY, &participant_qos_policy);

  if (nullptr != lookup_rc || nullptr == participant_qos_policy) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to lookup from environment: "
      "var=%s, "
      "rc=%s ",
      RMW_CONNEXT_ENV_PARTICIPANT_QOS_OVERRIDE_POLICY,
      lookup_rc)
    return RMW_RET_ERROR;
  }

  rc = rmw_connextdds_parse_participant_qos_override_policy(
    participant_qos_policy, ctx->participant_qos_override_policy);
  if (RMW_RET_OK != rc) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to parse value for environment variable {%s}",
      RMW_CONNEXT_ENV_PARTICIPANT_QOS_OVERRIDE_POLICY);
    return RMW_RET_ERROR;
  }

  // Check if the user specified a custom override policy for endpoint qos.
  const char * endpoint_qos_policy = nullptr;
  lookup_rc = rcutils_get_env(
    RMW_CONNEXT_ENV_ENDPOINT_QOS_OVERRIDE_POLICY, &endpoint_qos_policy);

  if (nullptr != lookup_rc || nullptr == endpoint_qos_policy) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to lookup from environment: "
      "var=%s, "
      "rc=%s ",
      RMW_CONNEXT_ENV_ENDPOINT_QOS_OVERRIDE_POLICY,
      lookup_rc)
    return RMW_RET_ERROR;
  }

  rc = rmw_connextdds_parse_endpoint_qos_override_policy(
    endpoint_qos_policy,
    ctx->endpoint_qos_override_policy,
    ctx->endpoint_qos_override_policy_topics_regex);
  if (RMW_RET_OK != rc) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to parse value for environment variable {%s}",
      RMW_CONNEXT_ENV_ENDPOINT_QOS_OVERRIDE_POLICY);
    return RMW_RET_ERROR;
  }

  // Check if we should run in "compatibility mode" with Cyclone DDS.
  const char * cyclone_compatible_env = nullptr;
  lookup_rc = rcutils_get_env(
    RMW_CONNEXT_ENV_CYCLONE_COMPATIBILITY_MODE, &cyclone_compatible_env);

  if (nullptr != lookup_rc || nullptr == cyclone_compatible_env) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to lookup from environment: "
      "var=%s, "
      "rc=%s ",
      RMW_CONNEXT_ENV_CYCLONE_COMPATIBILITY_MODE,
      lookup_rc)
    return RMW_RET_ERROR;
  }
  ctx->cyclone_compatible = '\0' != cyclone_compatible_env[0];

#if !RMW_CONNEXT_FORCE_REQUEST_REPLY_MAPPING_BASIC
  // Check if we should use the "basic" mapping profile for Request/Reply
  // endpoints in order to interoperate with Micro, and Cyclone DDS.
  // If "compatibility mode" with Cyclone is enabled, then we always use
  // the "basic" profile.
  if (ctx->cyclone_compatible) {
    ctx->request_reply_mapping = RMW_Connext_RequestReplyMapping::Basic;
  } else {
    const char * request_reply_mapping_env = nullptr;
    lookup_rc = rcutils_get_env(
      RMW_CONNEXT_ENV_REQUEST_REPLY_MAPPING, &request_reply_mapping_env);

    if (nullptr != lookup_rc || nullptr == request_reply_mapping_env) {
      RMW_CONNEXT_LOG_ERROR_A_SET(
        "failed to lookup from environment: "
        "var=%s, "
        "rc=%s ",
        RMW_CONNEXT_ENV_REQUEST_REPLY_MAPPING,
        lookup_rc)
      return RMW_RET_ERROR;
    }
    if ('\0' == request_reply_mapping_env[0] ||
      strncmp("extended", request_reply_mapping_env, 5) == 0)
    {
      ctx->request_reply_mapping = RMW_Connext_RequestReplyMapping::Extended;
    } else if (strncmp("basic", request_reply_mapping_env, 5) == 0) {
      ctx->request_reply_mapping = RMW_Connext_RequestReplyMapping::Basic;
    } else {
      RMW_CONNEXT_LOG_ERROR_A_SET(
        "invalid value for %s: '%s'. Use one of: basic, extended.",
        RMW_CONNEXT_ENV_REQUEST_REPLY_MAPPING, request_reply_mapping_env)
      return RMW_RET_ERROR;
    }
  }
#else
  ctx->request_reply_mapping = RMW_Connext_RequestReplyMapping::Basic;
#endif /* RMW_CONNEXT_FORCE_REQUEST_REPLY_MAPPING_BASIC */

#if RMW_CONNEXT_LEGACY_RMW_COMPATIBILITY_MODE
  // Check if we should run in "compatibility mode" with the old RMW for Connext
  const char * legacy_rmw_compatible_env = nullptr;
  lookup_rc = rcutils_get_env(
    RMW_CONNEXT_ENV_OLD_RMW_COMPATIBILITY_MODE, &legacy_rmw_compatible_env);

  if (nullptr != lookup_rc || nullptr == legacy_rmw_compatible_env) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to lookup from environment: "
      "var=%s, "
      "rc=%s ",
      RMW_CONNEXT_ENV_OLD_RMW_COMPATIBILITY_MODE,
      lookup_rc)
    return RMW_RET_ERROR;
  }
  ctx->legacy_rmw_compatible = '\0' != legacy_rmw_compatible_env[0];
#endif /* RMW_CONNEXT_LEGACY_RMW_COMPATIBILITY_MODE */

#if RMW_CONNEXT_FAST_ENDPOINT_DISCOVERY
  // Check if we should disable modifying the DomainParticipantQos to enable
  // faster endpoint discovery (but also increase discovery traffic).
  const char * disable_fast_endp_discovery_env = nullptr;
  lookup_rc = rcutils_get_env(
    RMW_CONNEXT_ENV_DISABLE_FAST_ENDPOINT_DISCOVERY,
    &disable_fast_endp_discovery_env);

  if (nullptr != lookup_rc || nullptr == disable_fast_endp_discovery_env) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to lookup from environment: "
      "var=%s, "
      "rc=%s ",
      RMW_CONNEXT_ENV_DISABLE_FAST_ENDPOINT_DISCOVERY,
      lookup_rc)
    return RMW_RET_ERROR;
  }
  ctx->fast_endp_discovery = '\0' == disable_fast_endp_discovery_env[0];
#endif /* RMW_CONNEXT_FAST_ENDPOINT_DISCOVERY */

#if RMW_CONNEXT_DEFAULT_LARGE_DATA_OPTIMIZATIONS
  // Check if we should disable modifying automatic tuning of reader and writer
  // QoS to enable to handle "large data".
  const char * disable_optimize_large_data_env = nullptr;
  lookup_rc = rcutils_get_env(
    RMW_CONNEXT_ENV_DISABLE_LARGE_DATA_OPTIMIZATIONS,
    &disable_optimize_large_data_env);

  if (nullptr != lookup_rc || nullptr == disable_optimize_large_data_env) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to lookup from environment: "
      "var=%s, "
      "rc=%s ",
      RMW_CONNEXT_ENV_DISABLE_LARGE_DATA_OPTIMIZATIONS,
      lookup_rc)
    return RMW_RET_ERROR;
  }
  ctx->optimize_large_data = '\0' == disable_optimize_large_data_env[0];
#endif /* RMW_CONNEXT_DEFAULT_LARGE_DATA_OPTIMIZATIONS */

  /* Lookup and configure initial peer from environment */
  const char * initial_peers = nullptr;
  lookup_rc =
    rcutils_get_env(RMW_CONNEXT_ENV_INITIAL_PEERS, &initial_peers);

  if (nullptr != lookup_rc || nullptr == initial_peers) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to lookup from environment: "
      "var=%s, "
      "rc=%s ",
      RMW_CONNEXT_ENV_INITIAL_PEERS,
      lookup_rc)
    return RMW_RET_ERROR;
  }

  if ('\0' != initial_peers[0]) {
    rmw_ret_t rc = rmw_connextdds_parse_string_list(
      initial_peers,
      &ctx->initial_peers,
      ',' /* delimiter */,
      true /* trim_elements */,
      false /* allow_empty_elements */,
      false /* append_values */);
    if (RMW_RET_OK != rc) {
      RMW_CONNEXT_LOG_ERROR_A(
        "failed to parse initial peers: '%s'", initial_peers)
      return rc;
    }
    RMW_CONNEXT_LOG_DEBUG_A("initial DDS peers: %s", initial_peers)
  }

  if (nullptr == RMW_Connext_gv_DomainParticipantFactory) {
    RMW_CONNEXT_LOG_DEBUG("initializing DDS DomainParticipantFactory")

    if (RMW_RET_OK !=
      rmw_connextdds_initialize_participant_factory_context(ctx))
    {
      RMW_CONNEXT_LOG_ERROR(
        "failed to initialize DDS DomainParticipantFactory context")
      return RMW_RET_ERROR;
    }

    RMW_Connext_gv_DomainParticipantFactory =
      DDS_DomainParticipantFactory_get_instance();
    if (nullptr == RMW_Connext_gv_DomainParticipantFactory) {
      RMW_CONNEXT_LOG_ERROR_SET("failed to get DDS participant factory")
      return RMW_RET_ERROR;
    }

    if (RMW_RET_OK !=
      rmw_connextdds_initialize_participant_factory_qos(ctx))
    {
      RMW_CONNEXT_LOG_ERROR_SET("failed to set DDS participant factory QoS")
      return RMW_RET_ERROR;
    }

    RMW_CONNEXT_LOG_DEBUG("DDS DomainParticipantFactory initialized")
  }
  RMW_CONNEXT_ASSERT(nullptr != RMW_Connext_gv_DomainParticipantFactory)
  RMW_CONNEXT_ASSERT(1 == RMW_Connext_gv_ContextCount)

  scope_exit_context_finalize.cancel();
  scope_exit_context_opts_finalize.cancel();
  scope_exit_context_reset.cancel();

  return RMW_RET_OK;
}


rmw_ret_t
rmw_api_connextdds_shutdown(rmw_context_t * context)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(context, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_FOR_NULL_WITH_MSG(
    context->impl,
    "expected initialized context",
    return RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    context,
    context->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  context->impl->is_shutdown = true;
  return RMW_RET_OK;
}


rmw_ret_t
rmw_api_connextdds_context_fini(rmw_context_t * context)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(context, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_FOR_NULL_WITH_MSG(
    context->impl,
    "expected initialized context",
    return RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    context,
    context->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  if (!context->impl->is_shutdown) {
    RMW_CONNEXT_LOG_ERROR_SET("context has not been shutdown")
    return RMW_RET_INVALID_ARGUMENT;
  }

  if (0u != context->impl->node_count) {
    RMW_CONNEXT_LOG_ERROR_A(
      "not all nodes finalized: %lu", context->impl->node_count)
  }

  // TODO(asorbini) keep track of created GuardConditions/WaitSets and make
  // sure that all of them have been cleaned up.

  // From now on, continue even if an error occurs, to satisfy
  // rmw_context_fini()'s contract.
  rmw_ret_t rc_exit = RMW_RET_OK;
  rmw_ret_t rc = context->impl->finalize();
  if (RMW_RET_OK != rc) {
    RMW_CONNEXT_LOG_ERROR("failed to finalize DDS participant factory")
    rc_exit = rc;
  }

  rc = rmw_api_connextdds_init_options_fini(&context->options);
  if (RMW_RET_OK != rc) {
    RMW_CONNEXT_LOG_ERROR("failed to finalize RMW context options")
    rc_exit = rc;
  }

  delete context->impl;
  *context = rmw_get_zero_initialized_context();
  return rc_exit;
}

rmw_ret_t
rmw_connextdds_configure_security(
  rmw_context_impl_t * const ctx,
  DDS_DomainParticipantQos * const qos)
{
  if (nullptr == ctx->base->options.security_options.security_root_path) {
    // Security not enabled;
    return RMW_RET_OK;
  }

  rmw_ret_t rc = rmw_connextdds_enable_security(ctx, qos);
  if (RMW_RET_OK != rc) {
    return rc;
  }

#if !RMW_CONNEXT_DDS_API_PRO_LEGACY
  static const char * const uri_prefix = "file:";
#else
  // Connext Pro 5.3.1 does not support the "file:" prefix
  static const char * const uri_prefix = "";
#endif /* !RMW_CONNEXT_DDS_API_PRO_LEGACY */

  std::unordered_map<std::string, std::string> security_files;
  if (!rmw_dds_common::get_security_files(
      uri_prefix, ctx->base->options.security_options.security_root_path, security_files))
  {
    RMW_CONNEXT_LOG_ERROR("couldn't find all security files");
    return RMW_RET_ERROR;
  }

  /* X509 Certificate of the Identity CA */
  if (DDS_RETCODE_OK !=
    DDS_PropertyQosPolicyHelper_assert_property(
      &qos->property,
      DDS_SECURITY_IDENTITY_CA_PROPERTY,
      security_files["IDENTITY_CA"].c_str(),
      RTI_FALSE))
  {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to assert DDS property: '%s' = '%s'",
      DDS_SECURITY_IDENTITY_CA_PROPERTY, security_files["IDENTITY_CA"].c_str())
    return RMW_RET_ERROR;
  }

  /* X509 Certificate of the Permissions CA */
  if (DDS_RETCODE_OK !=
    DDS_PropertyQosPolicyHelper_assert_property(
      &qos->property,
      DDS_SECURITY_PERMISSIONS_CA_PROPERTY,
      security_files["PERMISSIONS_CA"].c_str(),
      RTI_FALSE))
  {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to assert DDS property: '%s' = '%s'",
      DDS_SECURITY_PERMISSIONS_CA_PROPERTY, security_files["PERMISSIONS_CA"].c_str())
    return RMW_RET_ERROR;
  }

  /* Private Key of the DomainParticipant's identity */
  if (DDS_RETCODE_OK !=
    DDS_PropertyQosPolicyHelper_assert_property(
      &qos->property,
      DDS_SECURITY_PRIVATE_KEY_PROPERTY,
      security_files["PRIVATE_KEY"].c_str(),
      RTI_FALSE))
  {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to assert DDS property: '%s' = '%s'",
      DDS_SECURITY_PRIVATE_KEY_PROPERTY, security_files["PRIVATE_KEY"].c_str())
    return RMW_RET_ERROR;
  }

  /* Public certificate of the DomainParticipant's identity, signed
   * by the Certificate Authority */
  if (DDS_RETCODE_OK !=
    DDS_PropertyQosPolicyHelper_assert_property(
      &qos->property,
      DDS_SECURITY_IDENTITY_CERTIFICATE_PROPERTY,
      security_files["CERTIFICATE"].c_str(),
      RTI_FALSE))
  {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to assert DDS property: '%s' = '%s'",
      DDS_SECURITY_IDENTITY_CERTIFICATE_PROPERTY, security_files["CERTIFICATE"].c_str())
    return RMW_RET_ERROR;
  }
  /* XML file containing domain governance configuration, signed by
   * the Permission CA */
  if (DDS_RETCODE_OK !=
    DDS_PropertyQosPolicyHelper_assert_property(
      &qos->property,
      DDS_SECURITY_GOVERNANCE_PROPERTY,
      security_files["GOVERNANCE"].c_str(),
      RTI_FALSE))
  {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to assert DDS property: '%s' = '%s'",
      DDS_SECURITY_GOVERNANCE_PROPERTY, security_files["GOVERNANCE"].c_str())
    return RMW_RET_ERROR;
  }

  /* XML file containing domain permissions configuration, signed by
   * the Permission CA */
  if (DDS_RETCODE_OK !=
    DDS_PropertyQosPolicyHelper_assert_property(
      &qos->property,
      DDS_SECURITY_PERMISSIONS_PROPERTY,
      security_files["PERMISSIONS"].c_str(),
      RTI_FALSE))
  {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to assert DDS property: '%s' = '%s'",
      DDS_SECURITY_PERMISSIONS_PROPERTY, security_files["PERMISSIONS"].c_str())
    return RMW_RET_ERROR;
  }

  return rmw_connextdds_apply_security_logging_configuration(&qos->property);
}
