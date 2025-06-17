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
#include "rmw_connextdds/graph_cache.hpp"

#include "rmw/validate_full_topic_name.h"

/******************************************************************************
 * Subscription functions
 ******************************************************************************/


rmw_ret_t
rmw_api_connextdds_init_subscription_allocation(
  const rosidl_message_type_support_t * type_support,
  const rosidl_runtime_c__Sequence__bound * message_bounds,
  rmw_subscription_allocation_t * allocation)
{
  UNUSED_ARG(type_support);
  UNUSED_ARG(message_bounds);
  UNUSED_ARG(allocation);
  RMW_CONNEXT_LOG_NOT_IMPLEMENTED
  return RMW_RET_ERROR;
}


rmw_ret_t
rmw_api_connextdds_fini_subscription_allocation(
  rmw_subscription_allocation_t * allocation)
{
  UNUSED_ARG(allocation);
  RMW_CONNEXT_LOG_NOT_IMPLEMENTED
  return RMW_RET_ERROR;
}


rmw_subscription_t *
rmw_api_connextdds_create_subscription(
  const rmw_node_t * node,
  const rosidl_message_type_support_t * type_supports,
  const char * topic_name,
  const rmw_qos_profile_t * qos_policies,
  const rmw_subscription_options_t * subscription_options)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(node, nullptr);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    node,
    node->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return nullptr);
  RMW_CHECK_ARGUMENT_FOR_NULL(type_supports, nullptr);
  RMW_CHECK_ARGUMENT_FOR_NULL(topic_name, nullptr);
  RMW_CHECK_ARGUMENT_FOR_NULL(qos_policies, nullptr);
  RMW_CHECK_ARGUMENT_FOR_NULL(subscription_options, nullptr);
  if (subscription_options->require_unique_network_flow_endpoints ==
    RMW_UNIQUE_NETWORK_FLOW_ENDPOINTS_STRICTLY_REQUIRED)
  {
    RMW_CONNEXT_LOG_ERROR_SET(
      "Strict requirement on unique network flow endpoints for subscriptions not supported");
    return nullptr;
  }

  RMW_CONNEXT_LOG_DEBUG_A(
    "creating new subscription: topic=%s",
    topic_name)

  if (0 == strlen(topic_name)) {
    RMW_CONNEXT_LOG_ERROR_SET("empty topic_name provided")
    return nullptr;
  }

  if (!qos_policies->avoid_ros_namespace_conventions) {
    int validation_result = RMW_TOPIC_VALID;
    rmw_ret_t ret = rmw_validate_full_topic_name(
      topic_name, &validation_result, nullptr);
    if (RMW_RET_OK != ret) {
      return nullptr;
    }
    if (RMW_TOPIC_VALID != validation_result) {
      const char * reason =
        rmw_full_topic_name_validation_result_string(
        validation_result);
      UNUSED_ARG(reason);
      RMW_CONNEXT_LOG_ERROR_A_SET("invalid topic name: %s", reason)
      return nullptr;
    }
  }

  rmw_context_impl_t * ctx = node->context->impl;

  rmw_subscription_t * const rmw_sub =
    rmw_connextdds_create_subscriber(
    ctx,
    node,
    ctx->participant,
    ctx->dds_sub,
    type_supports,
    topic_name,
    qos_policies,
    subscription_options);

  if (nullptr == rmw_sub) {
    RMW_CONNEXT_LOG_ERROR("failed to create RMW subscription")
    return nullptr;
  }

  RMW_CONNEXT_LOG_DEBUG_A(
    "new subscription created: topic=%s, sub=%p",
    topic_name, (void *)rmw_sub->data)

  return rmw_sub;
}


rmw_ret_t
rmw_api_connextdds_subscription_count_matched_publishers(
  const rmw_subscription_t * subscription, size_t * publisher_count)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    subscription,
    subscription->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  RMW_CHECK_ARGUMENT_FOR_NULL(publisher_count, RMW_RET_INVALID_ARGUMENT);

  RMW_Connext_Subscriber * const sub_impl =
    reinterpret_cast<RMW_Connext_Subscriber *>(subscription->data);

  *publisher_count = sub_impl->publications_count();

  return RMW_RET_OK;
}


rmw_ret_t
rmw_api_connextdds_subscription_get_actual_qos(
  const rmw_subscription_t * subscription,
  rmw_qos_profile_t * qos)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    subscription,
    subscription->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  RMW_CHECK_ARGUMENT_FOR_NULL(qos, RMW_RET_INVALID_ARGUMENT);

  RMW_Connext_Subscriber * const sub_impl =
    reinterpret_cast<RMW_Connext_Subscriber *>(subscription->data);

  return sub_impl->qos(qos);
}

rmw_ret_t
rmw_api_connextdds_subscription_set_content_filter(
  rmw_subscription_t * subscription,
  const rmw_subscription_content_filter_options_t * options)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    subscription,
    subscription->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_ARGUMENT_FOR_NULL(options, RMW_RET_INVALID_ARGUMENT);

  RMW_Connext_Subscriber * const sub_impl =
    reinterpret_cast<RMW_Connext_Subscriber *>(subscription->data);

  rmw_ret_t rc = sub_impl->set_content_filter(options);
  subscription->is_cft_enabled = sub_impl->is_cft_enabled();

  return rc;
}


rmw_ret_t
rmw_api_connextdds_subscription_get_content_filter(
  const rmw_subscription_t * subscription,
  rcutils_allocator_t * const allocator,
  rmw_subscription_content_filter_options_t * options)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    subscription,
    subscription->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_ARGUMENT_FOR_NULL(allocator, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(options, RMW_RET_INVALID_ARGUMENT);

  RMW_Connext_Subscriber * const sub_impl =
    reinterpret_cast<RMW_Connext_Subscriber *>(subscription->data);

  if (!sub_impl->is_cft_enabled()) {
    RMW_CONNEXT_LOG_ERROR_SET("no content-filter associated with subscription")
    return RMW_RET_ERROR;
  }

  return sub_impl->get_content_filter(allocator, options);
}

rmw_ret_t
rmw_api_connextdds_destroy_subscription(
  rmw_node_t * node,
  rmw_subscription_t * subscription)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    node,
    node->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    subscription,
    subscription->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  RMW_Connext_Subscriber * const sub_impl =
    reinterpret_cast<RMW_Connext_Subscriber *>(subscription->data);

  rmw_context_impl_t * ctx = node->context->impl;

  if (RMW_RET_OK !=
    rmw_connextdds_graph_on_subscriber_deleted(ctx, node, sub_impl))
  {
    RMW_CONNEXT_LOG_ERROR("failed to update graph for subscriber")
    return RMW_RET_ERROR;
  }

  return rmw_connextdds_destroy_subscriber(ctx, subscription);
}


rmw_ret_t
rmw_api_connextdds_take(
  const rmw_subscription_t * subscription,
  void * ros_message,
  bool * taken,
  rmw_subscription_allocation_t * allocation)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    subscription,
    subscription->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_ARGUMENT_FOR_NULL(ros_message, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(taken, RMW_RET_INVALID_ARGUMENT);

  UNUSED_ARG(allocation);

  RMW_Connext_Subscriber * const sub_impl =
    reinterpret_cast<RMW_Connext_Subscriber *>(subscription->data);

  rmw_ret_t rc = sub_impl->take_message(ros_message, nullptr, taken);

  return rc;
}


rmw_ret_t
rmw_api_connextdds_take_with_info(
  const rmw_subscription_t * subscription,
  void * ros_message,
  bool * taken,
  rmw_message_info_t * message_info,
  rmw_subscription_allocation_t * allocation)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    subscription,
    subscription->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_ARGUMENT_FOR_NULL(ros_message, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(taken, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(message_info, RMW_RET_INVALID_ARGUMENT);

  UNUSED_ARG(allocation);

  RMW_Connext_Subscriber * const sub_impl =
    reinterpret_cast<RMW_Connext_Subscriber *>(subscription->data);

  rmw_ret_t rc = sub_impl->take_message(ros_message, message_info, taken);

  return rc;
}

rmw_ret_t
rmw_api_connextdds_take_sequence(
  const rmw_subscription_t * subscription,
  size_t count,
  rmw_message_sequence_t * message_sequence,
  rmw_message_info_sequence_t * message_info_sequence,
  size_t * taken,
  rmw_subscription_allocation_t * allocation)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    subscription,
    subscription->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_ARGUMENT_FOR_NULL(message_sequence, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(
    message_info_sequence, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(taken, RMW_RET_INVALID_ARGUMENT);

  UNUSED_ARG(allocation);

  RMW_Connext_Subscriber * const sub_impl =
    reinterpret_cast<RMW_Connext_Subscriber *>(subscription->data);

  // Reset length of output sequences
  message_sequence->size = 0;
  message_info_sequence->size = 0;

  rmw_ret_t rc = sub_impl->take(
    message_sequence, message_info_sequence, count, taken);

  // Update length of output sequences if we received any data
  if (*taken > 0) {
    message_sequence->size = *taken;
    message_info_sequence->size = *taken;
  }

  return rc;
}

rmw_ret_t
rmw_api_connextdds_take_serialized_message(
  const rmw_subscription_t * subscription,
  rmw_serialized_message_t * serialized_message,
  bool * taken,
  rmw_subscription_allocation_t * allocation)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    subscription,
    subscription->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_ARGUMENT_FOR_NULL(serialized_message, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(taken, RMW_RET_INVALID_ARGUMENT);

  UNUSED_ARG(allocation);

  RMW_Connext_Subscriber * const sub_impl =
    reinterpret_cast<RMW_Connext_Subscriber *>(subscription->data);

  return sub_impl->take_serialized(serialized_message, nullptr, taken);
}


rmw_ret_t
rmw_api_connextdds_take_serialized_message_with_info(
  const rmw_subscription_t * subscription,
  rmw_serialized_message_t * serialized_message,
  bool * taken,
  rmw_message_info_t * message_info,
  rmw_subscription_allocation_t * allocation)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    subscription,
    subscription->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_ARGUMENT_FOR_NULL(serialized_message, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(taken, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(message_info, RMW_RET_INVALID_ARGUMENT);

  UNUSED_ARG(allocation);

  RMW_Connext_Subscriber * const sub_impl =
    reinterpret_cast<RMW_Connext_Subscriber *>(subscription->data);

  return sub_impl->take_serialized(serialized_message, message_info, taken);
}


rmw_ret_t
rmw_api_connextdds_take_loaned_message(
  const rmw_subscription_t * subscription,
  void ** loaned_message,
  bool * taken,
  rmw_subscription_allocation_t * allocation)
{
  UNUSED_ARG(subscription);
  UNUSED_ARG(loaned_message);
  UNUSED_ARG(taken);
  UNUSED_ARG(allocation);
  RMW_CONNEXT_LOG_NOT_IMPLEMENTED
  return RMW_RET_UNSUPPORTED;
}


rmw_ret_t
rmw_api_connextdds_take_loaned_message_with_info(
  const rmw_subscription_t * subscription,
  void ** loaned_message,
  bool * taken,
  rmw_message_info_t * message_info,
  rmw_subscription_allocation_t * allocation)
{
  UNUSED_ARG(subscription);
  UNUSED_ARG(loaned_message);
  UNUSED_ARG(taken);
  UNUSED_ARG(message_info);
  UNUSED_ARG(allocation);
  RMW_CONNEXT_LOG_NOT_IMPLEMENTED
  return RMW_RET_UNSUPPORTED;
}


rmw_ret_t
rmw_api_connextdds_return_loaned_message_from_subscription(
  const rmw_subscription_t * subscription,
  void * loaned_message)
{
  UNUSED_ARG(subscription);
  UNUSED_ARG(loaned_message);
  RMW_CONNEXT_LOG_NOT_IMPLEMENTED
  return RMW_RET_UNSUPPORTED;
}
