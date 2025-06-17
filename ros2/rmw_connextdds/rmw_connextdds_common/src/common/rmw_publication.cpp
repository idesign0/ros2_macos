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
 * Publication functions
 ******************************************************************************/


rmw_ret_t
rmw_api_connextdds_publish(
  const rmw_publisher_t * publisher,
  const void * ros_message,
  rmw_publisher_allocation_t * allocation)
{
  UNUSED_ARG(allocation);

  RMW_CHECK_ARGUMENT_FOR_NULL(publisher, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(ros_message, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    publisher,
    publisher->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  RMW_CONNEXT_LOG_DEBUG_A(
    "publishing data: publisher=%p, topic=%s, msg=%p",
    (void *)publisher, publisher->topic_name, (void *)ros_message)

  auto pub_impl = static_cast<RMW_Connext_Publisher *>(publisher->data);
  RMW_CHECK_ARGUMENT_FOR_NULL(pub_impl, RMW_RET_INVALID_ARGUMENT);

  return pub_impl->write(ros_message, false /* serialized */);
}


rmw_ret_t
rmw_api_connextdds_publish_serialized_message(
  const rmw_publisher_t * publisher,
  const rmw_serialized_message_t * serialized_message,
  rmw_publisher_allocation_t * allocation)
{
  UNUSED_ARG(allocation);

  RMW_CHECK_ARGUMENT_FOR_NULL(publisher, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(serialized_message, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    publisher,
    publisher->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  RMW_CONNEXT_LOG_DEBUG_A(
    "publishing serialized data: publisher=%p, topic=%s",
    (void *)publisher, publisher->topic_name)

  auto pub_impl = static_cast<RMW_Connext_Publisher *>(publisher->data);
  RMW_CHECK_ARGUMENT_FOR_NULL(pub_impl, RMW_RET_INVALID_ARGUMENT);

  return pub_impl->write(serialized_message, true /* serialized */);
}


rmw_ret_t
rmw_api_connextdds_publish_loaned_message(
  const rmw_publisher_t * publisher,
  void * ros_message,
  rmw_publisher_allocation_t * allocation)
{
  UNUSED_ARG(publisher);
  UNUSED_ARG(ros_message);
  UNUSED_ARG(allocation);
  RMW_CONNEXT_LOG_NOT_IMPLEMENTED
  return RMW_RET_UNSUPPORTED;
}


rmw_ret_t
rmw_api_connextdds_init_publisher_allocation(
  const rosidl_message_type_support_t * type_support,
  const rosidl_runtime_c__Sequence__bound * message_bounds,
  rmw_publisher_allocation_t * allocation)
{
  UNUSED_ARG(type_support);
  UNUSED_ARG(message_bounds);
  UNUSED_ARG(allocation);
  RMW_CONNEXT_LOG_NOT_IMPLEMENTED
  return RMW_RET_UNSUPPORTED;
}


rmw_ret_t
rmw_api_connextdds_fini_publisher_allocation(
  rmw_publisher_allocation_t * allocation)
{
  UNUSED_ARG(allocation);
  RMW_CONNEXT_LOG_NOT_IMPLEMENTED
  return RMW_RET_UNSUPPORTED;
}


rmw_publisher_t *
rmw_api_connextdds_create_publisher(
  const rmw_node_t * node,
  const rosidl_message_type_support_t * type_supports,
  const char * topic_name,
  const rmw_qos_profile_t * qos_policies,
  const rmw_publisher_options_t * publisher_options)
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
  RMW_CHECK_ARGUMENT_FOR_NULL(publisher_options, nullptr);
  if (publisher_options->require_unique_network_flow_endpoints ==
    RMW_UNIQUE_NETWORK_FLOW_ENDPOINTS_STRICTLY_REQUIRED)
  {
    RMW_CONNEXT_LOG_ERROR_SET(
      "Strict requirement on unique network flow endpoints for publishers not supported");
    return nullptr;
  }

  RMW_CONNEXT_LOG_DEBUG_A(
    "creating new publisher: topic=%s",
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

  rmw_publisher_t * const rmw_pub =
    rmw_connextdds_create_publisher(
    ctx,
    node,
    ctx->participant,
    ctx->dds_pub,
    type_supports,
    topic_name,
    qos_policies,
    publisher_options);

  if (nullptr == rmw_pub) {
    RMW_CONNEXT_LOG_ERROR("failed to create RMW publisher")
    return nullptr;
  }

  RMW_CONNEXT_LOG_DEBUG_A(
    "new publisher created: topic=%s, pub=%p",
    topic_name, (void *)rmw_pub->data)

  return rmw_pub;
}


rmw_ret_t
rmw_api_connextdds_get_gid_for_publisher(
  const rmw_publisher_t * publisher,
  rmw_gid_t * gid)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(publisher, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    publisher,
    publisher->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_ARGUMENT_FOR_NULL(gid, RMW_RET_INVALID_ARGUMENT);

  RMW_Connext_Publisher * const pub_impl =
    reinterpret_cast<RMW_Connext_Publisher *>(publisher->data);

  *gid = *pub_impl->gid();

  return RMW_RET_OK;
}


rmw_ret_t
rmw_api_connextdds_compare_gids_equal(
  const rmw_gid_t * gid1,
  const rmw_gid_t * gid2,
  bool * result)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(gid1, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    gid1,
    gid1->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  RMW_CHECK_ARGUMENT_FOR_NULL(gid2, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    gid2,
    gid2->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  RMW_CHECK_ARGUMENT_FOR_NULL(result, RMW_RET_INVALID_ARGUMENT);

  *result = memcmp(gid1->data, gid2->data, sizeof(gid1->data)) == 0;

  return RMW_RET_OK;
}


rmw_ret_t
rmw_api_connextdds_publisher_count_matched_subscriptions(
  const rmw_publisher_t * publisher,
  size_t * subscription_count)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(publisher, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    publisher,
    publisher->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  RMW_CHECK_ARGUMENT_FOR_NULL(subscription_count, RMW_RET_INVALID_ARGUMENT);

  RMW_Connext_Publisher * const pub_impl =
    reinterpret_cast<RMW_Connext_Publisher *>(publisher->data);

  *subscription_count = pub_impl->subscriptions_count();

  return RMW_RET_OK;
}


rmw_ret_t
rmw_api_connextdds_publisher_assert_liveliness(
  const rmw_publisher_t * publisher)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(publisher, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    publisher,
    publisher->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  RMW_Connext_Publisher * const pub_impl =
    reinterpret_cast<RMW_Connext_Publisher *>(publisher->data);

  return pub_impl->assert_liveliness();
}


rmw_ret_t
rmw_api_connextdds_publisher_wait_for_all_acked(
  const rmw_publisher_t * publisher,
  rmw_time_t wait_timeout)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(publisher, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    publisher,
    publisher->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  RMW_Connext_Publisher * const pub_impl =
    reinterpret_cast<RMW_Connext_Publisher *>(publisher->data);
  RMW_CHECK_ARGUMENT_FOR_NULL(pub_impl, RMW_RET_INVALID_ARGUMENT);

  return pub_impl->wait_for_all_acked(wait_timeout);
}


rmw_ret_t
rmw_api_connextdds_publisher_get_actual_qos(
  const rmw_publisher_t * publisher,
  rmw_qos_profile_t * qos)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(publisher, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    publisher,
    publisher->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  RMW_CHECK_ARGUMENT_FOR_NULL(qos, RMW_RET_INVALID_ARGUMENT);

  RMW_Connext_Publisher * const pub_impl =
    reinterpret_cast<RMW_Connext_Publisher *>(publisher->data);

  return pub_impl->qos(qos);
}


rmw_ret_t
rmw_api_connextdds_borrow_loaned_message(
  const rmw_publisher_t * publisher,
  const rosidl_message_type_support_t * type_support,
  void ** ros_message)
{
  UNUSED_ARG(publisher);
  UNUSED_ARG(type_support);
  UNUSED_ARG(ros_message);
  RMW_CONNEXT_LOG_NOT_IMPLEMENTED
  return RMW_RET_UNSUPPORTED;
}


rmw_ret_t
rmw_api_connextdds_return_loaned_message_from_publisher(
  const rmw_publisher_t * publisher,
  void * loaned_message)
{
  UNUSED_ARG(publisher);
  UNUSED_ARG(loaned_message);
  RMW_CONNEXT_LOG_NOT_IMPLEMENTED
  return RMW_RET_UNSUPPORTED;
}


rmw_ret_t
rmw_api_connextdds_destroy_publisher(
  rmw_node_t * node,
  rmw_publisher_t * publisher)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(publisher, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    node,
    node->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    publisher,
    publisher->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  rmw_context_impl_t * ctx = node->context->impl;

  if (RMW_RET_OK !=
    rmw_connextdds_graph_on_publisher_deleted(
      ctx, node, reinterpret_cast<RMW_Connext_Publisher *>(publisher->data)))
  {
    RMW_CONNEXT_LOG_ERROR("failed to update graph for publisher")
    return RMW_RET_ERROR;
  }

  return rmw_connextdds_destroy_publisher(ctx, publisher);
}
