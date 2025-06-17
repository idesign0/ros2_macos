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

#include <string>
#include <map>
#include <vector>

#include "rmw/impl/cpp/key_value.hpp"
#include "rmw_connextdds/custom_sql_filter.hpp"

#include "rmw_connextdds/type_support.hpp"
#include "rmw_connextdds/rmw_impl.hpp"
#include "rmw_connextdds/graph_cache.hpp"

const char * const RMW_CONNEXTDDS_ID = "rmw_connextdds";
const char * const RMW_CONNEXTDDS_SERIALIZATION_FORMAT = "cdr";

struct rmw_connextdds_api_pro
{
  rti_connext_dds_custom_sql_filter::CustomSqlFilterData custom_filter_data;
};

rmw_connextdds_api_pro * RMW_Connext_fv_FactoryContext = nullptr;

rmw_ret_t
rmw_connextdds_set_log_verbosity(rmw_log_severity_t severity)
{
  NDDS_Config_Logger * logger = NDDS_Config_Logger_get_instance();

  NDDS_Config_LogVerbosity verbosity = NDDS_CONFIG_LOG_VERBOSITY_SILENT;

  switch (severity) {
    case RMW_LOG_SEVERITY_DEBUG:
      {
        verbosity = NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL;
        break;
      }
    case RMW_LOG_SEVERITY_INFO:
      {
        verbosity = NDDS_CONFIG_LOG_VERBOSITY_STATUS_LOCAL;
        break;
      }
    case RMW_LOG_SEVERITY_WARN:
      {
        verbosity = NDDS_CONFIG_LOG_VERBOSITY_WARNING;
        break;
      }
    case RMW_LOG_SEVERITY_ERROR:
    case RMW_LOG_SEVERITY_FATAL:
      {
        verbosity = NDDS_CONFIG_LOG_VERBOSITY_ERROR;
        break;
      }
    default:
      {
        RMW_CONNEXT_LOG_ERROR_A_SET("invalid log level: %d", severity)
        return RMW_RET_INVALID_ARGUMENT;
      }
  }

  NDDS_Config_Logger_set_verbosity(logger, verbosity);
  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_initialize_participant_factory_context(
  rmw_context_impl_t * const ctx)
{
  RMW_CONNEXT_ASSERT(RMW_Connext_fv_FactoryContext == nullptr)
  // RMW_Connext_gv_DomainParticipantFactory is initialized by
  // rmw_api_connextdds_init().
  RMW_CONNEXT_ASSERT(RMW_Connext_gv_DomainParticipantFactory == nullptr)
  UNUSED_ARG(ctx);

  rmw_connextdds_api_pro * ctx_api = nullptr;
  auto scope_exit_api_delete = rcpputils::make_scope_exit(
    [ctx_api]()
    {
      if (nullptr != ctx_api) {
        delete ctx_api;
      }
    });

  ctx_api = new (std::nothrow) rmw_connextdds_api_pro();
  if (nullptr == ctx_api) {
    return RMW_RET_ERROR;
  }

  scope_exit_api_delete.cancel();
  RMW_Connext_fv_FactoryContext = ctx_api;
  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_finalize_participant_factory_context(
  rmw_context_impl_t * const ctx)
{
  RMW_CONNEXT_ASSERT(nullptr != RMW_Connext_fv_FactoryContext)
  rmw_connextdds_api_pro * const ctx_api = RMW_Connext_fv_FactoryContext;
  RMW_Connext_fv_FactoryContext = nullptr;

  delete ctx_api;

  if (nullptr == RMW_Connext_gv_DomainParticipantFactory) {
    // Nothing else  to do if we didn't even initialize the factory;
    return RMW_RET_OK;
  }

  // There might be some DomainParticipants left-over from a ("failed context
  // initialization" + "failed participant finalization"), so let's try to
  // clean them up.
  DDS_DomainParticipantSeq participants = DDS_SEQUENCE_INITIALIZER;
  auto scope_exit_seq = rcpputils::make_scope_exit(
    [&participants]()
    {
      DDS_DomainParticipantSeq_finalize(&participants);
    });

  if (DDS_RETCODE_OK !=
    DDS_DomainParticipantFactory_get_participants(
      RMW_Connext_gv_DomainParticipantFactory, &participants))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to list existing participants")
    return RMW_RET_ERROR;
  }

  const DDS_Long pending = DDS_DomainParticipantSeq_get_length(&participants);
  for (DDS_Long i = 0; i < pending; i++) {
    DDS_DomainParticipant * const participant =
      *DDS_DomainParticipantSeq_get_reference(&participants, i);
#if RMW_CONNEXT_DEBUG
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
        "failed to enable pending DomainParticipant before deletion")
      return RMW_RET_ERROR;
    }
#endif  // RMW_CONNEXT_DEBUG
    if (DDS_RETCODE_OK !=
      DDS_DomainParticipant_delete_contained_entities(participant))
    {
      RMW_CONNEXT_LOG_ERROR_SET(
        "failed to delete pending DomainParticipant's entities")
      return RMW_RET_ERROR;
    }

    if (DDS_RETCODE_OK !=
      DDS_DomainParticipantFactory_delete_participant(
        RMW_Connext_gv_DomainParticipantFactory, participant))
    {
      RMW_CONNEXT_LOG_ERROR_SET("failed to delete pending DomainParticipant")
      return RMW_RET_ERROR;
    }
  }

  UNUSED_ARG(ctx);
  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_initialize_participant_qos_impl(
  rmw_context_impl_t * const ctx,
  DDS_DomainParticipantQos * const dp_qos)
{
#if RMW_CONNEXT_SHARE_DDS_ENTITIES_WITH_CPP
  // UserObjectQosPolicy is an internal, undocumented Connext policy used by the
  // implementations of different language bindings to control the memory
  // representations of various objects created by user applications. In this
  // case, the settings match the requirements of the "modern C++" API, and they
  // allow the DomainParticipant to be used directly by applications that want
  // to create new entities in C++11, even though the participant was created
  // using the C API. If these settings are not specified, an application will
  // receive a SIGSEGV when trying to create one of these entities.
  //
  // The customizations are always applied regardless of the QoS override policy
  // because UserObjectQosPolicy cannot be modified from XML.
  //
  dp_qos->user_object.flow_controller_user_object.size = sizeof(void *);
  dp_qos->user_object.topic_user_object.size = sizeof(void *);
  dp_qos->user_object.content_filtered_topic_user_object.size = sizeof(void *);
#endif /* RMW_CONNEXT_SHARE_DDS_ENTITIES_WITH_CPP */

  switch (ctx->participant_qos_override_policy) {
    case rmw_context_impl_t::participant_qos_override_policy_t::All:
    case rmw_context_impl_t::participant_qos_override_policy_t::Basic:
      {
        // Parse and apply QoS parameters derived from ROS 2 configuration options.

        if (ctx->localhost_only) {
          if (DDS_RETCODE_OK !=
            DDS_PropertyQosPolicyHelper_assert_property(
              &dp_qos->property,
              "dds.transport.UDPv4.builtin.parent.allow_interfaces",
              RMW_CONNEXT_LOCALHOST_ONLY_ADDRESS,
              DDS_BOOLEAN_FALSE /* propagate */))
          {
            RMW_CONNEXT_LOG_ERROR_A_SET(
              "failed to assert property on participant: %s",
              "dds.transport.UDPv4.builtin.parent.allow_interfaces")
            return RMW_RET_ERROR;
          }
        }

        const size_t user_data_len_in =
          DDS_OctetSeq_get_length(&dp_qos->user_data.value);

        if (user_data_len_in != 0) {
          RMW_CONNEXT_LOG_WARNING(
            "DomainParticipant's USER_DATA will be overwritten to "
            "propagate node enclave")
        }

        const char * const user_data_fmt = "enclave=%s;";

        const int user_data_len =
          std::snprintf(
          nullptr, 0, user_data_fmt, ctx->base->options.enclave) + 1;

        if (!DDS_OctetSeq_ensure_length(
            &dp_qos->user_data.value, user_data_len, user_data_len))
        {
          RMW_CONNEXT_LOG_ERROR_SET("failed to set user_data length")
          return RMW_RET_ERROR;
        }

        char * const user_data_ptr =
          reinterpret_cast<char *>(
          DDS_OctetSeq_get_contiguous_buffer(&dp_qos->user_data.value));

        const int user_data_rc =
          std::snprintf(
          user_data_ptr,
          user_data_len,
          user_data_fmt,
          ctx->base->options.enclave);

        if (user_data_rc < 0 || user_data_rc != user_data_len - 1) {
          RMW_CONNEXT_LOG_ERROR_SET("failed to set user_data")
          return RMW_RET_ERROR;
        }
        break;
      }
    default:
      {
        // No customization of DomainParticipantQos request, return immediately.
        RMW_CONNEXT_LOG_DEBUG("using default Connext's DomainParticipantQos")
        return RMW_RET_OK;
      }
  }

  if (rmw_context_impl_t::participant_qos_override_policy_t::Basic ==
    ctx->participant_qos_override_policy)
  {
    RMW_CONNEXT_LOG_DEBUG("applied only ROS 2 configuration to DomainParticipantQos")
    return RMW_RET_OK;
  }

#if RMW_CONNEXT_RTPS_AUTO_ID_FROM_UUID
  dp_qos->wire_protocol.rtps_auto_id_kind = DDS_RTPS_AUTO_ID_FROM_UUID;
#endif /* RMW_CONNEXT_RTPS_AUTO_ID_FROM_UUID */

  if (dp_qos->resource_limits.contentfilter_property_max_length <
    RMW_CONNEXT_CONTENTFILTER_PROPERTY_MAX_LENGTH)
  {
    dp_qos->resource_limits.contentfilter_property_max_length =
      RMW_CONNEXT_CONTENTFILTER_PROPERTY_MAX_LENGTH;
  }

  dp_qos->resource_limits.type_code_max_serialized_length =
    RMW_CONNEXT_TYPE_CODE_MAX_SERIALIZED_SIZE;
  dp_qos->resource_limits.type_object_max_serialized_length =
    RMW_CONNEXT_TYPE_OBJECT_MAX_SERIALIZED_SIZE;

  dp_qos->database.shutdown_cleanup_period.sec =
    RMW_CONNEXT_SHUTDOWN_CLEANUP_PERIOD_SEC;
  dp_qos->database.shutdown_cleanup_period.nanosec =
    RMW_CONNEXT_SHUTDOWN_CLEANUP_PERIOD_NSEC;

#if RMW_CONNEXT_FAST_ENDPOINT_DISCOVERY
  if (ctx->fast_endp_discovery) {
    // Apply Optimization.Discovery.Endpoint.Fast:
    //
    // QoS Snippet to optimize Endpoint Discovery to be faster. This
    // is useful when using security, to prevent a noticeable delay.
    //
    // Modified QoS Parameters:
    //     participant_qos.discovery_config.publication_writer
    //     participant_qos.discovery_config.subscription_writer
    dp_qos->discovery_config.publication_writer.fast_heartbeat_period.sec = 0;
    dp_qos->discovery_config.publication_writer.fast_heartbeat_period.nanosec = 100000000;
    dp_qos->discovery_config.publication_writer.late_joiner_heartbeat_period.sec = 0;
    dp_qos->discovery_config.publication_writer.late_joiner_heartbeat_period.nanosec = 100000000;
    dp_qos->discovery_config.publication_writer.max_heartbeat_retries = 300;

    dp_qos->discovery_config.subscription_writer.fast_heartbeat_period.sec = 0;
    dp_qos->discovery_config.subscription_writer.fast_heartbeat_period.nanosec = 100000000;
    dp_qos->discovery_config.subscription_writer.late_joiner_heartbeat_period.sec = 0;
    dp_qos->discovery_config.subscription_writer.late_joiner_heartbeat_period.nanosec = 100000000;
    dp_qos->discovery_config.subscription_writer.max_heartbeat_retries = 300;
  }
#endif /* RMW_CONNEXT_FAST_ENDPOINT_DISCOVERY */

  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_configure_participant(
  rmw_context_impl_t * const ctx,
  DDS_DomainParticipant * const participant)
{
  UNUSED_ARG(ctx);

  if (DDS_RETCODE_OK !=
    rti_connext_dds_custom_sql_filter::register_content_filter(
      participant, &RMW_Connext_fv_FactoryContext->custom_filter_data))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to register custom SQL filter")
    return RMW_RET_ERROR;
  }

  return RMW_RET_OK;
}

static
rmw_ret_t
rmw_connextdds_initialize_cft_parameters(
  struct DDS_StringSeq * const cft_parameters,
  const rcutils_string_array_t * const cft_expression_parameters)
{
  // Cache value locally to avoid conversion warnings on Windows.
  const DDS_Long params_len = static_cast<DDS_Long>(cft_expression_parameters->size);

  if (!DDS_StringSeq_ensure_length(cft_parameters, params_len, params_len)) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to ensure length for cft parameters sequence")
    return RMW_RET_ERROR;
  }
  if (!DDS_StringSeq_from_array(
      cft_parameters,
      const_cast<const char **>(cft_expression_parameters->data),
      params_len))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to copy data for cft parameters sequence")
    return RMW_RET_ERROR;
  }
  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_create_contentfilteredtopic(
  rmw_context_impl_t * const ctx,
  DDS_DomainParticipant * const dp,
  DDS_Topic * const base_topic,
  const char * const cft_name,
  const char * const cft_filter,
  const rcutils_string_array_t * const cft_expression_parameters,
  DDS_TopicDescription ** const cft_out)
{
  UNUSED_ARG(ctx);
  RMW_CONNEXT_ASSERT(nullptr != cft_name)
  RMW_CONNEXT_ASSERT(nullptr != cft_filter)

  struct DDS_StringSeq cft_parameters = DDS_SEQUENCE_INITIALIZER;
  auto scope_exit_cft_params = rcpputils::make_scope_exit(
    [&cft_parameters]() {
      if (!DDS_StringSeq_finalize(&cft_parameters)) {
        RMW_CONNEXT_LOG_ERROR_SET("failed to finalize cft parameters sequence")
      }
    });
  if (cft_expression_parameters) {
    if (RMW_RET_OK !=
      rmw_connextdds_initialize_cft_parameters(&cft_parameters, cft_expression_parameters))
    {
      RMW_CONNEXT_LOG_ERROR_SET("failed to rmw_connextdds_initialize_cft_parameters")
      return RMW_RET_ERROR;
    }
  }

  *cft_out = nullptr;

  DDS_ContentFilteredTopic * cft_topic =
    DDS_DomainParticipant_create_contentfilteredtopic_with_filter(
    dp, cft_name, base_topic, cft_filter, &cft_parameters,
    rti_connext_dds_custom_sql_filter::PLUGIN_NAME);

  if (nullptr == cft_topic) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to create content-filtered topic: "
      "name=%s, filter=%s", cft_name, cft_filter)
    return RMW_RET_ERROR;
  }

  *cft_out = DDS_ContentFilteredTopic_as_topicdescription(cft_topic);

  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_delete_contentfilteredtopic(
  rmw_context_impl_t * const ctx,
  DDS_DomainParticipant * const dp,
  DDS_TopicDescription * const cft_topicdesc)
{
  UNUSED_ARG(ctx);

  DDS_ContentFilteredTopic * const cft_topic =
    DDS_ContentFilteredTopic_narrow(cft_topicdesc);

  if (DDS_RETCODE_OK !=
    DDS_DomainParticipant_delete_contentfilteredtopic(dp, cft_topic))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to delete content-filtered topic")
    return RMW_RET_ERROR;
  }

  return RMW_RET_OK;
}

static
rmw_ret_t
rmw_connextdds_get_qos_policies(
  const bool writer_qos,
  RMW_Connext_MessageTypeSupport * const type_support,
  DDS_PropertyQosPolicy * const property,
  const rmw_qos_profile_t * const qos_policies,
  const rmw_publisher_options_t * const pub_options,
  const rmw_subscription_options_t * const sub_options)
{
  UNUSED_ARG(qos_policies);
  UNUSED_ARG(pub_options);
  UNUSED_ARG(sub_options);

  /* if type is unbound set property to force allocation of samples from heap */
  if (type_support->unbounded()) {
    const char * property_name = nullptr;
    if (writer_qos) {
      property_name =
        "dds.data_writer.history.memory_manager.fast_pool.pool_buffer_max_size";
    } else {
      property_name =
        "dds.data_reader.history.memory_manager.fast_pool.pool_buffer_max_size";
    }
    if (DDS_RETCODE_OK !=
      DDS_PropertyQosPolicyHelper_assert_property(
        property, property_name, "0",
        DDS_BOOLEAN_FALSE /* propagate */))
    {
      RMW_CONNEXT_LOG_ERROR_SET("failed to set property qos on writer")
      return RMW_RET_ERROR;
    }
  }

  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_get_datawriter_qos(
  rmw_context_impl_t * const ctx,
  RMW_Connext_MessageTypeSupport * const type_support,
  DDS_Topic * const topic,
  DDS_DataWriterQos * const qos,
  const rmw_qos_profile_t * const qos_policies,
  const rmw_publisher_options_t * const pub_options)
{
  UNUSED_ARG(topic);

  bool ignore_ros_profile = (
    ctx->endpoint_qos_override_policy ==
    rmw_context_impl_t::endpoint_qos_override_policy_t::Never);

  const char * topic_name = DDS_TopicDescription_get_name(DDS_Topic_as_topicdescription(topic));
  ignore_ros_profile = ignore_ros_profile || (ctx->endpoint_qos_override_policy ==
    rmw_context_impl_t::endpoint_qos_override_policy_t::DDSTopics &&
    std::regex_match(topic_name, ctx->endpoint_qos_override_policy_topics_regex));

  if (!ignore_ros_profile) {
    if (RMW_RET_OK !=
      rmw_connextdds_get_readerwriter_qos(
        true /* writer_qos */,
        type_support,
        &qos->history,
        &qos->reliability,
        &qos->durability,
        &qos->deadline,
        &qos->liveliness,
        &qos->resource_limits,
        // TODO(asorbini) this value is not actually used, remove it
        &qos->publish_mode,
        &qos->lifespan,
        qos_policies,
        pub_options,
        nullptr /* sub_options */))
    {
      return RMW_RET_ERROR;
    }
  }

  if (!ctx->use_default_publish_mode) {
    qos->publish_mode.kind = DDS_ASYNCHRONOUS_PUBLISH_MODE_QOS;
  }

#if RMW_CONNEXT_DEFAULT_LARGE_DATA_OPTIMIZATIONS
  // Unless disabled, optimize the DataWriter's reliability protocol to
  // better handle large data samples. These are *bounded* types whose
  // size exceeds the threshold defined by RMW_CONNEXT_LARGE_DATA_MIN_SERIALIZED_SIZE
  // These optmizations are mostly derived from Connext's built-in QoS profile
  // 'Generic.KeepLastReliable.LargeData'. They consist of limiting the RTPS
  // writer's "send window" to a limited amount of samples, and then configuring
  // the reliability protocol to use a faster "heartbeat period" when sending
  // data to a "late joiner" reader or a reader with many missed samples.
  if (ctx->optimize_large_data && type_support->large_data()) {
    qos->protocol.rtps_reliable_writer.min_send_window_size =
      RMW_CONNEXT_LARGE_DATA_SEND_WINDOW_SIZE_MIN;
    qos->protocol.rtps_reliable_writer.max_send_window_size =
      RMW_CONNEXT_LARGE_DATA_SEND_WINDOW_SIZE_MAX;
    qos->protocol.rtps_reliable_writer.heartbeats_per_max_samples =
      RMW_CONNEXT_LARGE_DATA_SEND_WINDOW_SIZE_MAX;

    qos->protocol.rtps_reliable_writer.heartbeat_period =
      RMW_CONNEXT_LARGE_DATA_HEARTBEAT_PERIOD;
    qos->protocol.rtps_reliable_writer.late_joiner_heartbeat_period =
      RMW_CONNEXT_LARGE_DATA_HEARTBEAT_PERIOD_FAST;
    qos->protocol.rtps_reliable_writer.fast_heartbeat_period =
      RMW_CONNEXT_LARGE_DATA_HEARTBEAT_PERIOD_FAST;

    qos->protocol.rtps_reliable_writer.max_nack_response_delay = DDS_DURATION_ZERO;

    qos->protocol.rtps_reliable_writer.high_watermark =
      RMW_CONNEXT_LARGE_DATA_SEND_WINDOW_SIZE_MIN;
    qos->protocol.rtps_reliable_writer.low_watermark = 0;

    qos->protocol.rtps_reliable_writer.max_heartbeat_retries = 500;
  }
#endif /* RMW_CONNEXT_DEFAULT_LARGE_DATA_OPTIMIZATIONS */

  return rmw_connextdds_get_qos_policies(
    true /* writer_qos */,
    type_support,
    &qos->property,
    qos_policies,
    pub_options,
    nullptr /* sub_options */);
}

rmw_ret_t
rmw_connextdds_get_datareader_qos(
  rmw_context_impl_t * const ctx,
  RMW_Connext_MessageTypeSupport * const type_support,
  DDS_TopicDescription * const topic_desc,
  DDS_DataReaderQos * const qos,
  const rmw_qos_profile_t * const qos_policies,
  const rmw_subscription_options_t * const sub_options)
{
  UNUSED_ARG(ctx);
  UNUSED_ARG(topic_desc);

  bool ignore_ros_profile = (
    ctx->endpoint_qos_override_policy ==
    rmw_context_impl_t::endpoint_qos_override_policy_t::Never);

  const char * topic_name = DDS_TopicDescription_get_name(topic_desc);
  ignore_ros_profile = ignore_ros_profile || (ctx->endpoint_qos_override_policy ==
    rmw_context_impl_t::endpoint_qos_override_policy_t::DDSTopics &&
    std::regex_match(topic_name, ctx->endpoint_qos_override_policy_topics_regex));

  if (!ignore_ros_profile) {
    if (RMW_RET_OK !=
      rmw_connextdds_get_readerwriter_qos(
        false /* writer_qos */,
        type_support,
        &qos->history,
        &qos->reliability,
        &qos->durability,
        &qos->deadline,
        &qos->liveliness,
        &qos->resource_limits,
        nullptr /* publish_mode */,
        nullptr /* Lifespan is a writer-only qos policy */,
        qos_policies,
        nullptr /* pub_options */,
        sub_options))
    {
      return RMW_RET_ERROR;
    }
  }

#if RMW_CONNEXT_DEFAULT_LARGE_DATA_OPTIMIZATIONS
  // Unless disabled, optimize the DataReader's reliability protocol to
  // better handle large data samples. These are *bounded* types whose
  // size exceeds the threshold defined by RMW_CONNEXT_LARGE_DATA_MIN_SERIALIZED_SIZE
  // These optmizations are mostly derived from Connext's built-in QoS profile
  // 'Generic.KeepLastReliable.LargeData'.
  if (ctx->optimize_large_data && type_support->large_data()) {
    qos->protocol.rtps_reliable_reader.min_heartbeat_response_delay = DDS_DURATION_ZERO;
    qos->protocol.rtps_reliable_reader.max_heartbeat_response_delay = DDS_DURATION_ZERO;

    // Determines whether the DataReader pre-allocates storage for storing
    // fragmented samples. This setting can be used to limit up-front memory
    // allocation costs in applications that deal with large data.
    qos->reader_resource_limits.dynamically_allocate_fragmented_samples = DDS_BOOLEAN_TRUE;
  }
#endif /* RMW_CONNEXT_DEFAULT_LARGE_DATA_OPTIMIZATIONS */

  return rmw_connextdds_get_qos_policies(
    false /* writer_qos */,
    type_support,
    &qos->property,
    qos_policies,
    nullptr /* pub_options */,
    sub_options);
}

DDS_DataWriter *
rmw_connextdds_create_datawriter(
  rmw_context_impl_t * const ctx,
  DDS_DomainParticipant * const participant,
  DDS_Publisher * const pub,
  const rmw_qos_profile_t * const qos_policies,
  const rmw_publisher_options_t * const publisher_options,
  const bool internal,
  RMW_Connext_MessageTypeSupport * const type_support,
  DDS_Topic * const topic,
  DDS_DataWriterQos * const dw_qos)
{
  UNUSED_ARG(ctx);
  UNUSED_ARG(participant);
  UNUSED_ARG(internal);

  if (RMW_RET_OK !=
    rmw_connextdds_get_datawriter_qos(
      ctx, type_support, topic, dw_qos, qos_policies, publisher_options))
  {
    RMW_CONNEXT_LOG_ERROR("failed to convert writer QoS")
    return nullptr;
  }

  DDS_DataWriter * const writer =
    DDS_Publisher_create_datawriter(
    pub, topic, dw_qos,
    NULL, DDS_STATUS_MASK_NONE);

  return writer;
}

DDS_DataReader *
rmw_connextdds_create_datareader(
  rmw_context_impl_t * const ctx,
  DDS_DomainParticipant * const participant,
  DDS_Subscriber * const sub,
  const rmw_qos_profile_t * const qos_policies,
  const rmw_subscription_options_t * const subscriber_options,
  const bool internal,
  RMW_Connext_MessageTypeSupport * const type_support,
  DDS_TopicDescription * const topic_desc,
  DDS_DataReaderQos * const dr_qos)
{
  UNUSED_ARG(ctx);
  UNUSED_ARG(participant);
  UNUSED_ARG(internal);

  if (RMW_RET_OK !=
    rmw_connextdds_get_datareader_qos(
      ctx, type_support, topic_desc, dr_qos, qos_policies, subscriber_options))
  {
    RMW_CONNEXT_LOG_ERROR("failed to convert reader QoS")
    return nullptr;
  }

  return DDS_Subscriber_create_datareader(
    sub, topic_desc, dr_qos, NULL, DDS_STATUS_MASK_NONE);
}

rmw_ret_t
rmw_connextdds_write_message(
  RMW_Connext_Publisher * const pub,
  RMW_Connext_Message * const message,
  int64_t * const sn_out)
{
  if (pub->message_type_support()->type_requestreply() &&
    pub->message_type_support()->ctx()->request_reply_mapping ==
    RMW_Connext_RequestReplyMapping::Extended)
  {
    const RMW_Connext_RequestReplyMessage * const rr_msg =
      reinterpret_cast<const RMW_Connext_RequestReplyMessage *>(message->user_data);
    DDS_WriteParams_t write_params = DDS_WRITEPARAMS_DEFAULT;

    if (!rr_msg->request) {
      /* If this is a reply, propagate the request's sample identity
         via the related_sample_identity field */
      rmw_ret_t rc = RMW_RET_ERROR;

      rmw_connextdds_sn_ros_to_dds(
        rr_msg->sn,
        write_params.related_sample_identity.sequence_number);

      rc = rmw_connextdds_gid_to_guid(
        rr_msg->gid,
        write_params.related_sample_identity.writer_guid);
      if (RMW_RET_OK != rc) {
        return rc;
      }
    } else {
      // enable WriteParams::replace_auto to retrieve SN of published message
      write_params.replace_auto = DDS_BOOLEAN_TRUE;
    }
    if (DDS_RETCODE_OK !=
      DDS_DataWriter_write_w_params_untypedI(
        pub->writer(), message, &write_params))
    {
      RMW_CONNEXT_LOG_ERROR_SET(
        "failed to write request/reply message to DDS")
      return RMW_RET_ERROR;
    }

    if (rr_msg->request) {
      int64_t sn = 0;

      // Read assigned sn from write_params
      rmw_connextdds_sn_dds_to_ros(
        write_params.identity.sequence_number, sn);

      *sn_out = sn;
    }

    return RMW_RET_OK;
  }

  if (DDS_RETCODE_OK !=
    DDS_DataWriter_write_untypedI(
      pub->writer(), message, &DDS_HANDLE_NIL))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to write message to DDS")
    return RMW_RET_ERROR;
  }

  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_take_samples(
  RMW_Connext_Subscriber * const sub)
{
  DDS_Boolean is_loan = DDS_BOOLEAN_TRUE;
  DDS_Long data_len = 0;
  void ** data_buffer = nullptr;

  DDS_ReturnCode_t rc =
    DDS_DataReader_read_or_take_instance_untypedI(
    sub->reader(),
    &is_loan,
    &data_buffer,
    &data_len,
    sub->info_seq(),
    0 /* data_seq_len */,
    0 /* data_seq_max_len */,
    DDS_BOOLEAN_TRUE /* data_seq_has_ownership */,
    NULL /* data_seq_contiguous_buffer_for_copy */,
    1 /* data_size -- ignored because loaning*/,
    DDS_LENGTH_UNLIMITED /* max_samples */,
    &DDS_HANDLE_NIL /* a_handle */,
#if !RMW_CONNEXT_DDS_API_PRO_LEGACY
    NULL /* topic_query_guid */,
#endif /* RMW_CONNEXT_DDS_API_PRO_LEGACY */
    DDS_ANY_SAMPLE_STATE,
    DDS_ANY_VIEW_STATE,
    DDS_ANY_INSTANCE_STATE,
    DDS_BOOLEAN_TRUE /* take */);
  if (DDS_RETCODE_OK != rc) {
    if (DDS_RETCODE_NO_DATA == rc) {
      return RMW_RET_OK;
    }
    RMW_CONNEXT_LOG_ERROR_SET("failed to take data from DDS reader")
    return RMW_RET_ERROR;
  }
  RMW_CONNEXT_ASSERT(data_len > 0)(void) RMW_Connext_MessagePtrSeq_loan_contiguous(
    sub->data_seq(),
    reinterpret_cast<RMW_Connext_Message **>(data_buffer),
    data_len,
    data_len);

  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_return_samples(
  RMW_Connext_Subscriber * const sub)
{
  void ** data_buffer = reinterpret_cast<void **>(
    RMW_Connext_MessagePtrSeq_get_contiguous_buffer(sub->data_seq()));

  if (!RMW_Connext_MessagePtrSeq_unloan(sub->data_seq())) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to unloan sample sequence")
    return RMW_RET_ERROR;
  }
  // DDS_DataReader_return_loan_untypedI is an internal API, and
  // its signature changed slightly in Connext 7.x.
#if RTI_DDS_VERSION_MAJOR < 7
  const DDS_Long data_len =
    RMW_Connext_MessagePtrSeq_get_length(sub->data_seq());
  if (DDS_RETCODE_OK !=
    DDS_DataReader_return_loan_untypedI(
      sub->reader(),
      data_buffer,
      data_len,
      sub->info_seq()))
#else
  if (DDS_RETCODE_OK !=
    DDS_DataReader_return_loan_untypedI(
      sub->reader(),
      data_buffer,
      sub->info_seq()))
#endif  // RTI_DDS_VERSION_MAJOR < 7
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to return loan to DDS reader")
    return RMW_RET_ERROR;
  }
  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_count_unread_samples(
  RMW_Connext_Subscriber * const sub,
  size_t & unread_count)
{
  DDS_Boolean is_loan = DDS_BOOLEAN_TRUE;
  DDS_Long data_len = 0;
  void ** data_buffer = nullptr;
  DDS_SampleInfoSeq info_seq = DDS_SEQUENCE_INITIALIZER;

  unread_count = 0;
  DDS_ReturnCode_t rc = DDS_RETCODE_ERROR;
  do {
    rc = DDS_DataReader_read_or_take_instance_untypedI(
      sub->reader(),
      &is_loan,
      &data_buffer,
      &data_len,
      &info_seq,
      0 /* data_seq_len */,
      0 /* data_seq_max_len */,
      DDS_BOOLEAN_TRUE /* data_seq_has_ownership */,
      NULL /* data_seq_contiguous_buffer_for_copy */,
      1 /* data_size -- ignored because loaning*/,
      DDS_LENGTH_UNLIMITED /* max_samples */,
      &DDS_HANDLE_NIL /* a_handle */,
  #if !RMW_CONNEXT_DDS_API_PRO_LEGACY
      NULL /* topic_query_guid */,
  #endif /* RMW_CONNEXT_DDS_API_PRO_LEGACY */
      DDS_NOT_READ_SAMPLE_STATE,
      DDS_ANY_VIEW_STATE,
      DDS_ANY_INSTANCE_STATE,
      DDS_BOOLEAN_FALSE /* take */);
    if (DDS_RETCODE_NO_DATA == rc) {
      continue;
    }
    if (DDS_RETCODE_OK != rc && DDS_RETCODE_NO_DATA != rc) {
      RMW_CONNEXT_LOG_ERROR_SET("failed to read data from DDS reader")
      return RMW_RET_ERROR;
    }
    if (DDS_RETCODE_OK == rc) {
      unread_count += data_len;
      // DDS_DataReader_return_loan_untypedI is an internal API, and
      // its signature changed slightly in Connext 7.x.
#if RTI_DDS_VERSION_MAJOR < 7
      rc = DDS_DataReader_return_loan_untypedI(
        sub->reader(),
        data_buffer,
        data_len,
        &info_seq);
#else
      rc = DDS_DataReader_return_loan_untypedI(
        sub->reader(),
        data_buffer,
        &info_seq);
#endif  // RTI_DDS_VERSION_MAJOR < 7
      if (DDS_RETCODE_OK != rc) {
        RMW_CONNEXT_LOG_ERROR_SET("failed to return loan to DDS reader")
        return RMW_RET_ERROR;
      }
    }
  } while (rc == DDS_RETCODE_OK);

  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_filter_sample(
  RMW_Connext_Subscriber * const sub,
  const void * const sample,
  const DDS_SampleInfo * const info,
  const DDS_InstanceHandle_t * const request_writer_handle,
  bool * const accepted)
{
  UNUSED_ARG(info);

  *accepted = true;

  if (sub->ignore_local) {
    DDS_InstanceHandle_t reader_ih = sub->participant_instance_handle();

    *accepted = (0 != memcmp(
        reader_ih.keyHash.value,
        info->original_publication_virtual_guid.value,
        12));
  }

  if (!*accepted) {
    return RMW_RET_OK;
  }

  if (nullptr != request_writer_handle) {
    if (!sub->message_type_support()->type_requestreply()) {
      return RMW_RET_ERROR;
    }

    const RMW_Connext_RequestReplyMessage * const rr_msg =
      reinterpret_cast<const RMW_Connext_RequestReplyMessage *>(sample);
    DDS_SampleIdentity_t related_sample_identity;

    switch (sub->message_type_support()->ctx()->request_reply_mapping) {
      case RMW_Connext_RequestReplyMapping::Extended:
        {
          DDS_SampleInfo_get_related_sample_identity(
            info, &related_sample_identity);
          break;
        }
      case RMW_Connext_RequestReplyMapping::Basic:
        {
          rmw_connextdds_gid_to_guid(
            rr_msg->gid, related_sample_identity.writer_guid);
          break;
        }
      default:
        return RMW_RET_ERROR;
    }

    // Convert instance handle to guid
    DDS_GUID_t writer_guid = DDS_GUID_DEFAULT;
    memcpy(writer_guid.value, request_writer_handle->keyHash.value, MIG_RTPS_KEY_HASH_MAX_LENGTH);

    if (sub->message_type_support()->ctx()->cyclone_compatible) {
      // Compare only the 8 MSB from writer_guid
      *accepted = (memcmp(
          static_cast<void *>(writer_guid.value + 8),
          static_cast<void *>(related_sample_identity.writer_guid.value + 8), 8) == 0);
    } else {
      *accepted =
        (DDS_GUID_compare(
          &writer_guid, &related_sample_identity.writer_guid) == 0);
    }
  }

  return RMW_RET_OK;
}


rmw_ret_t
rmw_connextdds_dcps_participant_get_reader(
  rmw_context_impl_t * const ctx,
  DDS_DataReader ** const reader_out)
{
  DDS_Subscriber * const sub =
    DDS_DomainParticipant_get_builtin_subscriber(ctx->participant);
  if (nullptr == sub) {
    return RMW_RET_ERROR;
  }

  DDS_DataReader * const reader =
    DDS_Subscriber_lookup_datareader(sub, DDS_PARTICIPANT_TOPIC_NAME);

  if (nullptr == reader) {
    return RMW_RET_ERROR;
  }

  *reader_out = reader;
  return RMW_RET_OK;
}


rmw_ret_t
rmw_connextdds_dcps_publication_get_reader(
  rmw_context_impl_t * const ctx,
  DDS_DataReader ** const reader_out)
{
  DDS_Subscriber * const sub =
    DDS_DomainParticipant_get_builtin_subscriber(ctx->participant);
  if (nullptr == sub) {
    return RMW_RET_ERROR;
  }

  DDS_DataReader * const reader =
    DDS_Subscriber_lookup_datareader(sub, DDS_PUBLICATION_TOPIC_NAME);

  if (nullptr == reader) {
    return RMW_RET_ERROR;
  }

  *reader_out = reader;
  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_dcps_subscription_get_reader(
  rmw_context_impl_t * const ctx,
  DDS_DataReader ** const reader_out)
{
  DDS_Subscriber * const sub =
    DDS_DomainParticipant_get_builtin_subscriber(ctx->participant);
  if (nullptr == sub) {
    return RMW_RET_ERROR;
  }

  DDS_DataReader * const reader =
    DDS_Subscriber_lookup_datareader(sub, DDS_SUBSCRIPTION_TOPIC_NAME);

  if (nullptr == reader) {
    return RMW_RET_ERROR;
  }

  *reader_out = reader;
  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_enable_builtin_readers(rmw_context_impl_t * const ctx)
{
  /* Enable builtin subscriber and endpoints */
  DDS_Subscriber * const sub =
    DDS_DomainParticipant_get_builtin_subscriber(ctx->participant);
  if (nullptr == sub) {
    return RMW_RET_ERROR;
  }

  if (DDS_RETCODE_OK != DDS_Entity_enable(DDS_Subscriber_as_entity(sub))) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to enable builtin subscriber")
    return RMW_RET_ERROR;
  }

  if (DDS_RETCODE_OK !=
    DDS_Entity_enable(
      DDS_Topic_as_entity(
        DDS_Topic_narrow(
          DDS_DataReader_get_topicdescription(
            ctx->dr_participants)))))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to enable builtin topic (participants)")
    return RMW_RET_ERROR;
  }

  if (DDS_RETCODE_OK !=
    DDS_Entity_enable(DDS_DataReader_as_entity(ctx->dr_participants)))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to enable builtin reader (participants)")
    return RMW_RET_ERROR;
  }

  if (DDS_RETCODE_OK !=
    DDS_Entity_enable(
      DDS_Topic_as_entity(
        DDS_Topic_narrow(
          DDS_DataReader_get_topicdescription(
            ctx->dr_publications)))))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to enable builtin topic (publications)")
    return RMW_RET_ERROR;
  }

  if (DDS_RETCODE_OK !=
    DDS_Entity_enable(DDS_DataReader_as_entity(ctx->dr_publications)))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to enable builtin reader (publications)")
    return RMW_RET_ERROR;
  }

  if (DDS_RETCODE_OK !=
    DDS_Entity_enable(
      DDS_Topic_as_entity(
        DDS_Topic_narrow(
          DDS_DataReader_get_topicdescription(
            ctx->dr_subscriptions)))))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to enable builtin topic (subscriptions)")
    return RMW_RET_ERROR;
  }

  if (DDS_RETCODE_OK !=
    DDS_Entity_enable(DDS_DataReader_as_entity(ctx->dr_subscriptions)))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to enable builtin reader (subscriptions)")
    return RMW_RET_ERROR;
  }

  return RMW_RET_OK;
}


static std::map<std::string, std::vector<uint8_t>>
rmw_connextdds_parse_map(uint8_t * const data, const DDS_Long data_len)
{
  std::vector<uint8_t> data_vec(data, data + data_len);
  std::map<std::string, std::vector<uint8_t>> map =
    rmw::impl::cpp::parse_key_value(data_vec);
  return map;
}

static rmw_ret_t
rmw_connextdds_get_user_data_key(
  const DDS_ParticipantBuiltinTopicData * const data,
  const std::string key,
  std::string & value,
  bool & found)
{
  found = false;
  uint8_t * const user_data =
    reinterpret_cast<uint8_t *>(
    DDS_OctetSeq_get_contiguous_buffer(&data->user_data.value));
  const DDS_Long user_data_len =
    DDS_OctetSeq_get_length(&data->user_data.value);
  if (nullptr == user_data || user_data_len == 0) {
    return RMW_RET_OK;
  }
  auto map = rmw_connextdds_parse_map(user_data, user_data_len);
  auto name_found = map.find(key);
  if (name_found != map.end()) {
    value = std::string(name_found->second.begin(), name_found->second.end());
    found = true;
  }
  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_dcps_participant_on_data(rmw_context_impl_t * const ctx)
{
  DDS_ParticipantBuiltinTopicDataSeq data_seq = DDS_SEQUENCE_INITIALIZER;
  DDS_SampleInfoSeq info_seq = DDS_SEQUENCE_INITIALIZER;
  DDS_ReturnCode_t rc = DDS_RETCODE_OK;

  DDS_ParticipantBuiltinTopicDataDataReader * const reader =
    DDS_ParticipantBuiltinTopicDataDataReader_narrow(ctx->dr_participants);

  do {
    rc = DDS_ParticipantBuiltinTopicDataDataReader_take(
      reader,
      &data_seq,
      &info_seq,
      DDS_LENGTH_UNLIMITED,
      DDS_ANY_SAMPLE_STATE,
      DDS_ANY_VIEW_STATE,
      DDS_ANY_INSTANCE_STATE);
    if (DDS_RETCODE_OK != rc) {
      continue;
    }

    const DDS_Long data_len =
      DDS_ParticipantBuiltinTopicDataSeq_get_length(&data_seq);
    for (DDS_Long i = 0; i < data_len; i++) {
      DDS_ParticipantBuiltinTopicData * const data =
        DDS_ParticipantBuiltinTopicDataSeq_get_reference(&data_seq, i);
      DDS_SampleInfo * const info =
        DDS_SampleInfoSeq_get_reference(&info_seq, i);

      if (!info->valid_data) {
        /* TODO(asorbini): Check for instance_state != ALIVE to remove
           the remote participant from the graph_cache by calling:
           graph_cache.remove_participant(gid) */
        if (info->instance_state == DDS_NOT_ALIVE_DISPOSED_INSTANCE_STATE ||
          info->instance_state == DDS_NOT_ALIVE_NO_WRITERS_INSTANCE_STATE)
        {
          if (RMW_RET_OK !=
            rmw_connextdds_graph_remove_participant(ctx, &info->instance_handle))
          {
            // TODO(asorbini) log without lock
            continue;
          }
        } else {
          RMW_CONNEXT_LOG_DEBUG(
            "[discovery thread] ignored participant invalid data")
        }

        continue;
      }

      std::string enclave_str;
      bool enclave_found;

      rmw_ret_t rc = rmw_connextdds_get_user_data_key(
        data, "enclave", enclave_str, enclave_found);
      if (RMW_RET_OK != rc) {
        RMW_CONNEXT_LOG_ERROR("failed to parse user data for enclave")
        continue;
      }
      const char * enclave = nullptr;
      if (enclave_found) {
        enclave = enclave_str.c_str();
      }

      if (RMW_RET_OK !=
        rmw_connextdds_graph_add_participant(ctx, data, enclave))
      {
        RMW_CONNEXT_LOG_ERROR(
          "failed to asser remote participant in graph")
        continue;
      }
    }

    if (DDS_RETCODE_OK !=
      DDS_ParticipantBuiltinTopicDataDataReader_return_loan(
        reader, &data_seq, &info_seq))
    {
      RMW_CONNEXT_LOG_ERROR_SET("failed to return loan to dds reader")
      return RMW_RET_ERROR;
    }
  } while (DDS_RETCODE_OK == rc);

  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_dcps_publication_on_data(rmw_context_impl_t * const ctx)
{
  DDS_PublicationBuiltinTopicDataSeq data_seq = DDS_SEQUENCE_INITIALIZER;
  DDS_SampleInfoSeq info_seq = DDS_SEQUENCE_INITIALIZER;
  DDS_ReturnCode_t rc = DDS_RETCODE_OK;

  DDS_PublicationBuiltinTopicDataDataReader * const reader =
    DDS_PublicationBuiltinTopicDataDataReader_narrow(ctx->dr_publications);

  do {
    rc = DDS_PublicationBuiltinTopicDataDataReader_take(
      reader,
      &data_seq,
      &info_seq,
      DDS_LENGTH_UNLIMITED,
      DDS_ANY_SAMPLE_STATE,
      DDS_ANY_VIEW_STATE,
      DDS_ANY_INSTANCE_STATE);
    if (DDS_RETCODE_OK != rc) {
      continue;
    }

    const DDS_Long data_len =
      DDS_PublicationBuiltinTopicDataSeq_get_length(&data_seq);
    for (DDS_Long i = 0; i < data_len; i++) {
      DDS_PublicationBuiltinTopicData * const data =
        DDS_PublicationBuiltinTopicDataSeq_get_reference(&data_seq, i);
      DDS_SampleInfo * const info =
        DDS_SampleInfoSeq_get_reference(&info_seq, i);

      if (!info->valid_data) {
        if (info->instance_state == DDS_NOT_ALIVE_DISPOSED_INSTANCE_STATE ||
          info->instance_state == DDS_NOT_ALIVE_NO_WRITERS_INSTANCE_STATE)
        {
          if (RMW_RET_OK !=
            rmw_connextdds_graph_remove_entity(
              ctx, &info->instance_handle, false /* is_reader */))
          {
            // TODO(asorbini) log without lock
            continue;
          }
        } else {
          RMW_CONNEXT_LOG_DEBUG("[discovery thread] ignored publication invalid data")
        }
        continue;
      }

      DDS_GUID_t endp_guid;
      DDS_GUID_t dp_guid;

      DDS_BuiltinTopicKey_to_guid(&data->key, &endp_guid);
      DDS_BuiltinTopicKey_to_guid(&data->participant_key, &dp_guid);

      // Ignore return code since we can't do anything about a failure
      // in this listener.
      rmw_connextdds_graph_add_remote_entity(
        ctx,
        &endp_guid,
        &dp_guid,
        data->topic_name,
        data->type_name,
        &data->reliability,
        &data->durability,
        &data->deadline,
        &data->liveliness,
        &data->lifespan,
        false /* is_reader */);
    }

    if (DDS_RETCODE_OK !=
      DDS_PublicationBuiltinTopicDataDataReader_return_loan(
        reader, &data_seq, &info_seq))
    {
      RMW_CONNEXT_LOG_ERROR_SET("failed to return loan to dds reader")
      return RMW_RET_ERROR;
    }
  } while (DDS_RETCODE_OK == rc);

  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_dcps_subscription_on_data(rmw_context_impl_t * const ctx)
{
  DDS_SubscriptionBuiltinTopicDataSeq data_seq = DDS_SEQUENCE_INITIALIZER;
  DDS_SampleInfoSeq info_seq = DDS_SEQUENCE_INITIALIZER;
  DDS_ReturnCode_t rc = DDS_RETCODE_OK;

  DDS_SubscriptionBuiltinTopicDataDataReader * const reader =
    DDS_SubscriptionBuiltinTopicDataDataReader_narrow(
    ctx->dr_subscriptions);

  do {
    rc = DDS_SubscriptionBuiltinTopicDataDataReader_take(
      reader,
      &data_seq,
      &info_seq,
      DDS_LENGTH_UNLIMITED,
      DDS_ANY_SAMPLE_STATE,
      DDS_ANY_VIEW_STATE,
      DDS_ANY_INSTANCE_STATE);
    if (DDS_RETCODE_OK != rc) {
      continue;
    }

    const DDS_Long data_len =
      DDS_SubscriptionBuiltinTopicDataSeq_get_length(&data_seq);
    for (DDS_Long i = 0; i < data_len; i++) {
      DDS_SubscriptionBuiltinTopicData * const data =
        DDS_SubscriptionBuiltinTopicDataSeq_get_reference(&data_seq, i);
      DDS_SampleInfo * const info =
        DDS_SampleInfoSeq_get_reference(&info_seq, i);

      if (!info->valid_data) {
        if (info->instance_state == DDS_NOT_ALIVE_DISPOSED_INSTANCE_STATE ||
          info->instance_state == DDS_NOT_ALIVE_NO_WRITERS_INSTANCE_STATE)
        {
          if (RMW_RET_OK !=
            rmw_connextdds_graph_remove_entity(
              ctx, &info->instance_handle, true /* is_reader */))
          {
            // TODO(asorbini) log without lock
            continue;
          }
        } else {
          RMW_CONNEXT_LOG_DEBUG("[discovery thread] ignored subscription invalid data")
        }
        continue;
      }

      DDS_GUID_t endp_guid;
      DDS_GUID_t dp_guid;

      DDS_BuiltinTopicKey_to_guid(&data->key, &endp_guid);
      DDS_BuiltinTopicKey_to_guid(&data->participant_key, &dp_guid);

      // Ignore return code since we can't do anything about a failure
      // in this listener.
      rmw_connextdds_graph_add_remote_entity(
        ctx,
        &endp_guid,
        &dp_guid,
        data->topic_name,
        data->type_name,
        &data->reliability,
        &data->durability,
        &data->deadline,
        &data->liveliness,
        nullptr /* Lifespan is a writer-only qos policy */,
        true /* is_reader */);
    }

    if (DDS_RETCODE_OK !=
      DDS_SubscriptionBuiltinTopicDataDataReader_return_loan(
        reader, &data_seq, &info_seq))
    {
      RMW_CONNEXT_LOG_ERROR_SET("failed to return loan to dds reader")
      return RMW_RET_ERROR;
    }
  } while (DDS_RETCODE_OK == rc);

  return RMW_RET_OK;
}

void
rmw_connextdds_ih_to_gid(const DDS_InstanceHandle_t & ih, rmw_gid_t & gid)
{
  static_assert(
    RMW_GID_STORAGE_SIZE >= MIG_RTPS_KEY_HASH_MAX_LENGTH,
    "rmw_gid_t type too small for an RTI Connext DDS GUID");

  memset(&gid, 0, sizeof(gid));
  gid.implementation_identifier = RMW_CONNEXTDDS_ID;
  memcpy(gid.data, ih.keyHash.value, MIG_RTPS_KEY_HASH_MAX_LENGTH);
}

void
rmw_connextdds_configure_subscriber_condition_listener(
  RMW_Connext_SubscriberStatusCondition * cond,
  DDS_DataReaderListener * const listener,
  DDS_StatusMask * const listener_mask)
{
  UNUSED_ARG(cond);
  UNUSED_ARG(listener_mask);
  UNUSED_ARG(listener);
}

void
rmw_connextdds_builtinkey_to_guid(
  const DDS_BuiltinTopicKey_t * const self,
  DDS_GUID_t * const dst)
{
  DDS_BuiltinTopicKey_to_guid(self, dst);
}

rmw_ret_t
rmw_connextdds_enable_security(
  rmw_context_impl_t * const ctx,
  DDS_DomainParticipantQos * const qos)
{
  UNUSED_ARG(ctx);

  if (DDS_RETCODE_OK !=
    DDS_PropertyQosPolicyHelper_assert_property(
      &qos->property,
      "com.rti.serv.load_plugin",
      "com.rti.serv.secure",
      RTI_FALSE))
  {
    return RMW_RET_ERROR;
  }

  if (DDS_RETCODE_OK !=
    DDS_PropertyQosPolicyHelper_assert_property(
      &qos->property,
      "com.rti.serv.secure.library",
      "nddssecurity",
      RTI_FALSE))
  {
    return RMW_RET_ERROR;
  }

  if (DDS_RETCODE_OK !=
    DDS_PropertyQosPolicyHelper_assert_property(
      &qos->property,
      "com.rti.serv.secure.create_function",
      "RTI_Security_PluginSuite_create",
      RTI_FALSE))
  {
    return RMW_RET_ERROR;
  }

  return RMW_RET_OK;
}


rmw_ret_t
rmw_connextdds_set_cft_filter_expression(
  DDS_TopicDescription * const topic_desc,
  const char * const cft_expression,
  const rcutils_string_array_t * const cft_expression_parameters)
{
  DDS_ContentFilteredTopic * const cft_topic =
    DDS_ContentFilteredTopic_narrow(topic_desc);

  struct DDS_StringSeq cft_parameters = DDS_SEQUENCE_INITIALIZER;
  auto scope_exit_cft_parameters = rcpputils::make_scope_exit(
    [&cft_parameters]() {
      if (!DDS_StringSeq_finalize(&cft_parameters)) {
        RMW_CONNEXT_LOG_ERROR_SET("failed to finalize cft parameters sequence")
      }
    });
  if (nullptr != cft_expression_parameters) {
    if (RMW_RET_OK !=
      rmw_connextdds_initialize_cft_parameters(&cft_parameters, cft_expression_parameters))
    {
      RMW_CONNEXT_LOG_ERROR_SET("failed to rmw_connextdds_initialize_cft_parameters")
      return RMW_RET_ERROR;
    }
  }

  DDS_ReturnCode_t ret =
    DDS_ContentFilteredTopic_set_expression(cft_topic, cft_expression, &cft_parameters);
  if (DDS_RETCODE_OK != ret) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to set content-filtered topic")
    return RMW_RET_ERROR;
  }
  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_get_cft_filter_expression(
  DDS_TopicDescription * const topic_desc,
  rcutils_allocator_t * const allocator,
  rmw_subscription_content_filter_options_t * const options)
{
  DDS_ContentFilteredTopic * const cft_topic =
    DDS_ContentFilteredTopic_narrow(topic_desc);

  // get filter_expression
  const char * filter_expression = DDS_ContentFilteredTopic_get_filter_expression(cft_topic);
  if (!filter_expression) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to get filter expression")
    return RMW_RET_ERROR;
  }

  // get parameters
  struct DDS_StringSeq parameters = DDS_SEQUENCE_INITIALIZER;
  DDS_ReturnCode_t status =
    DDS_ContentFilteredTopic_get_expression_parameters(cft_topic, &parameters);
  if (DDS_RETCODE_OK != status) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to get expression parameters")
    return RMW_RET_ERROR;
  }
  auto scope_exit_parameters_delete =
    rcpputils::make_scope_exit(
    [&parameters]()
    {
      DDS_StringSeq_finalize(&parameters);
    });

  const DDS_Long parameters_len = DDS_StringSeq_get_length(&parameters);
  std::vector<const char *> expression_parameters;
  expression_parameters.reserve(parameters_len);

  for (DDS_Long i = 0; i < parameters_len; ++i) {
    const char * ref = *DDS_StringSeq_get_reference(&parameters, i);
    if (!ref) {
      RMW_CONNEXT_LOG_ERROR_A_SET(
        "failed to get a reference for parameters with index %d", i)
      return RMW_RET_ERROR;
    }

    expression_parameters.push_back(ref);
  }

  if (RMW_RET_OK != rmw_subscription_content_filter_options_init(
      filter_expression,
      expression_parameters.size(),
      expression_parameters.data(),
      allocator,
      options))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to rmw_subscription_content_filter_options_init")
    return RMW_RET_ERROR;
  }

  return RMW_RET_OK;
}
