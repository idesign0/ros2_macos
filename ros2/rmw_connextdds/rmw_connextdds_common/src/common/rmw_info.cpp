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

#include "rmw_connextdds/rmw_impl.hpp"

#include "rmw/sanity_checks.h"
#include "rmw/validate_full_topic_name.h"
#include "rmw/validate_namespace.h"
#include "rmw/validate_node_name.h"

#include "rmw_connextdds/demangle.hpp"


/******************************************************************************
 * Introspection functions
 ******************************************************************************/


rmw_ret_t
rmw_api_connextdds_get_node_names(
  const rmw_node_t * node,
  rcutils_string_array_t * node_names,
  rcutils_string_array_t * node_namespaces)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    node,
    node->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_ARGUMENT_FOR_NULL(node_names, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(node_namespaces, RMW_RET_INVALID_ARGUMENT);

  if (RMW_RET_OK != rmw_check_zero_rmw_string_array(node_names) ||
    RMW_RET_OK != rmw_check_zero_rmw_string_array(node_namespaces))
  {
    return RMW_RET_INVALID_ARGUMENT;
  }

  auto common_ctx = &node->context->impl->common;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rmw_ret_t rc = common_ctx->graph_cache.get_node_names(
    node_names,
    node_namespaces,
    nullptr,
    &allocator);
  if (RMW_RET_OK != rc) {
    return rc;
  }
  RMW_CONNEXT_LOG_DEBUG_A(
    "[GRAPH] get_node_names result: "
    "names=%lu, namespaces=%lu",
    node_names->size,
    node_namespaces->size)
  for (size_t i = 0; i < node_names->size; i++) {
    RMW_CONNEXT_LOG_DEBUG_A(
      "[GRAPH]   Node: %s::%s",
      node_namespaces->data[i],
      node_names->data[i])
  }

  return rc;
}


rmw_ret_t
rmw_api_connextdds_get_node_names_with_enclaves(
  const rmw_node_t * node,
  rcutils_string_array_t * node_names,
  rcutils_string_array_t * node_namespaces,
  rcutils_string_array_t * enclaves)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    node,
    node->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_ARGUMENT_FOR_NULL(node_names, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(node_namespaces, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(enclaves, RMW_RET_INVALID_ARGUMENT);

  if (RMW_RET_OK != rmw_check_zero_rmw_string_array(node_names) ||
    RMW_RET_OK != rmw_check_zero_rmw_string_array(node_namespaces))
  {
    return RMW_RET_INVALID_ARGUMENT;
  }

  auto common_ctx = &node->context->impl->common;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  rmw_ret_t rc = common_ctx->graph_cache.get_node_names(
    node_names,
    node_namespaces,
    enclaves,
    &allocator);
  if (RMW_RET_OK != rc) {
    return rc;
  }
  RMW_CONNEXT_LOG_DEBUG_A(
    "[GRAPH] get_node_names_with_enclaves result: "
    "names=%lu, namespaces=%lu",
    node_names->size,
    node_namespaces->size)
  for (size_t i = 0; i < node_names->size; i++) {
    RMW_CONNEXT_LOG_DEBUG_A(
      "[GRAPH]   Node: %s::%s",
      node_namespaces->data[i],
      node_names->data[i])
  }

  return rc;
}


rmw_ret_t
rmw_api_connextdds_get_topic_names_and_types(
  const rmw_node_t * node,
  rcutils_allocator_t * allocator,
  bool no_demangle,
  rmw_names_and_types_t * tptyp)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    node,
    node->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_ARGUMENT_FOR_NULL(allocator, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(tptyp, RMW_RET_INVALID_ARGUMENT);

  if (RMW_RET_OK != rmw_names_and_types_check_zero(tptyp)) {
    return RMW_RET_INVALID_ARGUMENT;
  }

  DemangleFunction demangle_topic = _demangle_ros_topic_from_topic;
  DemangleFunction demangle_type = _demangle_if_ros_type;
  if (no_demangle) {
    demangle_topic = _identity_demangle;
    demangle_type = _identity_demangle;
  }
  auto common_ctx = &node->context->impl->common;
  return common_ctx->graph_cache.get_names_and_types(
    demangle_topic, demangle_type, allocator, tptyp);
}


rmw_ret_t
rmw_api_connextdds_get_service_names_and_types(
  const rmw_node_t * node,
  rcutils_allocator_t * allocator,
  rmw_names_and_types_t * sntyp)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    node,
    node->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_ARGUMENT_FOR_NULL(allocator, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(sntyp, RMW_RET_INVALID_ARGUMENT);

  auto common_ctx = &node->context->impl->common;
  return common_ctx->graph_cache.get_names_and_types(
    _demangle_service_from_topic,
    _demangle_service_type_only,
    allocator,
    sntyp);
}


rmw_ret_t
rmw_api_connextdds_service_server_is_available(
  const rmw_node_t * node,
  const rmw_client_t * client,
  bool * is_available)
{
  // TODO(asorbini): Return RMW_RET_INVALID_ARGUMENT. We return RMW_RET_ERROR
  // because that's what's expected by test_rmw_implementation
  RMW_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_ERROR);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    node,
    node->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  // TODO(asorbini): Return RMW_RET_INVALID_ARGUMENT. We return RMW_RET_ERROR
  // because that's what's expected by test_rmw_implementation
  RMW_CHECK_ARGUMENT_FOR_NULL(client, RMW_RET_ERROR);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    client,
    client->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  // TODO(asorbini): Return RMW_RET_INVALID_ARGUMENT. We return RMW_RET_ERROR
  // because that's what's expected by test_rmw_implementation
  RMW_CHECK_ARGUMENT_FOR_NULL(is_available, RMW_RET_ERROR);

  RMW_Connext_Client * const client_impl =
    reinterpret_cast<RMW_Connext_Client *>(client->data);

  *is_available = false;
  bool available = false;
  rmw_ret_t rc = client_impl->is_service_available(available);
  if (RMW_RET_OK == rc) {
    *is_available = available;
  }
  return rc;
}

rmw_ret_t
rmw_api_connextdds_count_publishers(
  const rmw_node_t * node,
  const char * topic_name,
  size_t * count)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    node,
    node->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_ARGUMENT_FOR_NULL(topic_name, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(count, RMW_RET_INVALID_ARGUMENT);

  int validation_result = RMW_TOPIC_VALID;
  rmw_ret_t ret =
    rmw_validate_full_topic_name(topic_name, &validation_result, nullptr);
  if (RMW_RET_OK != ret) {
    return ret;
  }
  if (RMW_TOPIC_VALID != validation_result) {
    const char * reason =
      rmw_full_topic_name_validation_result_string(validation_result);
    RMW_CONNEXT_LOG_ERROR_A_SET("invalid topic name: %s", reason)
    return RMW_RET_INVALID_ARGUMENT;
  }

  auto common_context = &node->context->impl->common;
  const std::string mangled_topic_name =
    rmw_connextdds_create_topic_name(
    ROS_TOPIC_PREFIX, topic_name, "", false);
  return common_context->graph_cache.get_writer_count(
    mangled_topic_name, count);
}


rmw_ret_t
rmw_api_connextdds_count_subscribers(
  const rmw_node_t * node,
  const char * topic_name,
  size_t * count)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    node,
    node->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_ARGUMENT_FOR_NULL(topic_name, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(count, RMW_RET_INVALID_ARGUMENT);

  int validation_result = RMW_TOPIC_VALID;
  rmw_ret_t ret =
    rmw_validate_full_topic_name(topic_name, &validation_result, nullptr);
  if (RMW_RET_OK != ret) {
    return ret;
  }
  if (RMW_TOPIC_VALID != validation_result) {
    const char * reason =
      rmw_full_topic_name_validation_result_string(validation_result);
    RMW_CONNEXT_LOG_ERROR_A_SET("invalid topic name: %s", reason)
    return RMW_RET_INVALID_ARGUMENT;
  }

  auto common_context = &node->context->impl->common;
  const std::string mangled_topic_name =
    rmw_connextdds_create_topic_name(
    ROS_TOPIC_PREFIX, topic_name, "", false);
  return common_context->graph_cache.get_reader_count(
    mangled_topic_name, count);
}


using GetNamesAndTypesByNodeFunction = rmw_ret_t (*)(
  rmw_dds_common::Context *,
  const std::string &,
  const std::string &,
  DemangleFunction,
  DemangleFunction,
  rcutils_allocator_t *,
  rmw_names_and_types_t *);

static rmw_ret_t get_topic_names_and_types_by_node(
  const rmw_node_t * node,
  rcutils_allocator_t * allocator,
  const char * node_name,
  const char * node_namespace,
  DemangleFunction demangle_topic,
  DemangleFunction demangle_type,
  bool no_demangle,
  GetNamesAndTypesByNodeFunction get_names_and_types_by_node,
  rmw_names_and_types_t * topic_names_and_types)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    node,
    node->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_ARGUMENT_FOR_NULL(allocator, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(node_name, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(node_namespace, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(topic_names_and_types, RMW_RET_INVALID_ARGUMENT);

  int validation_result = RMW_NODE_NAME_VALID;
  rmw_ret_t ret = rmw_validate_node_name(node_name, &validation_result, nullptr);
  if (RMW_RET_OK != ret) {
    return ret;
  }
  if (RMW_NODE_NAME_VALID != validation_result) {
    const char * reason = rmw_node_name_validation_result_string(validation_result);
    RMW_CONNEXT_LOG_ERROR_A_SET("invalid node name: %s", reason)
    return RMW_RET_INVALID_ARGUMENT;
  }
  validation_result = RMW_NAMESPACE_VALID;
  ret = rmw_validate_namespace(node_namespace, &validation_result, nullptr);
  if (RMW_RET_OK != ret) {
    return ret;
  }
  if (RMW_NAMESPACE_VALID != validation_result) {
    const char * reason = rmw_namespace_validation_result_string(validation_result);
    RMW_CONNEXT_LOG_ERROR_A_SET("invalid node namespace: %s", reason)
    return RMW_RET_INVALID_ARGUMENT;
  }

  auto common_context = &node->context->impl->common;
  if (no_demangle) {
    demangle_topic = _identity_demangle;
    demangle_type = _identity_demangle;
  }

  return get_names_and_types_by_node(
    common_context,
    node_name,
    node_namespace,
    demangle_topic,
    demangle_type,
    allocator,
    topic_names_and_types);
}

static rmw_ret_t get_reader_names_and_types_by_node(
  rmw_dds_common::Context * common_context,
  const std::string & node_name,
  const std::string & node_namespace,
  DemangleFunction demangle_topic,
  DemangleFunction demangle_type,
  rcutils_allocator_t * allocator,
  rmw_names_and_types_t * topic_names_and_types)
{
  return common_context->graph_cache.get_reader_names_and_types_by_node(
    node_name,
    node_namespace,
    demangle_topic,
    demangle_type,
    allocator,
    topic_names_and_types);
}

static rmw_ret_t get_writer_names_and_types_by_node(
  rmw_dds_common::Context * common_context,
  const std::string & node_name,
  const std::string & node_namespace,
  DemangleFunction demangle_topic,
  DemangleFunction demangle_type,
  rcutils_allocator_t * allocator,
  rmw_names_and_types_t * topic_names_and_types)
{
  RMW_CONNEXT_LOG_DEBUG_A(
    "get_writer_names_and_types_by_node: "
    "node=%s::%s", node_namespace.c_str(), node_name.c_str())
  rmw_ret_t rc =
    common_context->graph_cache.get_writer_names_and_types_by_node(
    node_name,
    node_namespace,
    demangle_topic,
    demangle_type,
    allocator,
    topic_names_and_types);
  if (RMW_RET_OK != rc) {
    return rc;
  }

  RMW_CONNEXT_LOG_DEBUG_A(
    "get_writer_names_and_types_by_node result: "
    "node=%s::%s, "
    "names.size=%lu, "
    "types.size=%lu",
    node_name.c_str(), node_namespace.c_str(),
    topic_names_and_types->names.size,
    (topic_names_and_types->types != NULL) ?
    topic_names_and_types->types->size : 0)

  return rc;
}


rmw_ret_t
rmw_api_connextdds_get_subscriber_names_and_types_by_node(
  const rmw_node_t * node,
  rcutils_allocator_t * allocator,
  const char * node_name,
  const char * node_namespace,
  bool no_demangle,
  rmw_names_and_types_t * tptyp)
{
  RMW_CONNEXT_LOG_DEBUG_A(
    "rmw_get_subscriber_names_and_types_by_node: "
    "node=%s::%s, demangle=%d",
    node_namespace, node_name, !no_demangle)

  return get_topic_names_and_types_by_node(
    node,
    allocator,
    node_name,
    node_namespace,
    _demangle_ros_topic_from_topic,
    _demangle_if_ros_type,
    no_demangle,
    get_reader_names_and_types_by_node,
    tptyp);
}


rmw_ret_t
rmw_api_connextdds_get_publisher_names_and_types_by_node(
  const rmw_node_t * node,
  rcutils_allocator_t * allocator,
  const char * node_name,
  const char * node_namespace,
  bool no_demangle,
  rmw_names_and_types_t * tptyp)
{
  RMW_CONNEXT_LOG_DEBUG_A(
    "rmw_get_publisher_names_and_types_by_node: "
    "node=%s%s, demangle=%d",
    node_namespace, node_name, !no_demangle)

  return get_topic_names_and_types_by_node(
    node,
    allocator,
    node_name,
    node_namespace,
    _demangle_ros_topic_from_topic,
    _demangle_if_ros_type,
    no_demangle,
    get_writer_names_and_types_by_node,
    tptyp);
}


rmw_ret_t
rmw_api_connextdds_get_service_names_and_types_by_node(
  const rmw_node_t * node,
  rcutils_allocator_t * allocator,
  const char * node_name,
  const char * node_namespace,
  rmw_names_and_types_t * sntyp)
{
  RMW_CONNEXT_LOG_DEBUG_A(
    "rmw_get_service_names_and_types_by_node: "
    "node=%s%s", node_namespace, node_name)
  return get_topic_names_and_types_by_node(
    node,
    allocator,
    node_name,
    node_namespace,
    _demangle_service_request_from_topic,
    _demangle_service_type_only,
    false /* no_demangle */,
    get_reader_names_and_types_by_node,
    sntyp);
}


rmw_ret_t
rmw_api_connextdds_get_client_names_and_types_by_node(
  const rmw_node_t * node,
  rcutils_allocator_t * allocator,
  const char * node_name,
  const char * node_namespace,
  rmw_names_and_types_t * sntyp)
{
  RMW_CONNEXT_LOG_DEBUG_A(
    "rmw_get_client_names_and_types_by_node: "
    "node=%s%s", node_namespace, node_name)
  return get_topic_names_and_types_by_node(
    node,
    allocator,
    node_name,
    node_namespace,
    _demangle_service_reply_from_topic,
    _demangle_service_type_only,
    false /* no demangle */,
    get_reader_names_and_types_by_node,
    sntyp);
}


rmw_ret_t
rmw_api_connextdds_get_publishers_info_by_topic(
  const rmw_node_t * node,
  rcutils_allocator_t * allocator,
  const char * topic_name,
  bool no_mangle,
  rmw_topic_endpoint_info_array_t * publishers_info)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    node,
    node->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_ARGUMENT_FOR_NULL(allocator, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(topic_name, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(publishers_info, RMW_RET_INVALID_ARGUMENT);

  RCUTILS_CHECK_ALLOCATOR_WITH_MSG(
    allocator, "allocator argument is invalid", return RMW_RET_INVALID_ARGUMENT);

  if (RMW_RET_OK != rmw_topic_endpoint_info_array_check_zero(publishers_info)) {
    return RMW_RET_INVALID_ARGUMENT;
  }

  auto common_context = &node->context->impl->common;
  std::string mangled_topic_name = topic_name;
  DemangleFunction demangle_type = _identity_demangle;
  if (!no_mangle) {
    mangled_topic_name =
      rmw_connextdds_create_topic_name(
      ROS_TOPIC_PREFIX, topic_name, "", false);
    demangle_type = _demangle_if_ros_type;
  }
  return common_context->graph_cache.get_writers_info_by_topic(
    mangled_topic_name,
    demangle_type,
    allocator,
    publishers_info);
}


rmw_ret_t
rmw_api_connextdds_get_subscriptions_info_by_topic(
  const rmw_node_t * node,
  rcutils_allocator_t * allocator,
  const char * topic_name,
  bool no_mangle,
  rmw_topic_endpoint_info_array_t * subscriptions_info)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    node,
    node->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_ARGUMENT_FOR_NULL(allocator, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(topic_name, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(subscriptions_info, RMW_RET_INVALID_ARGUMENT);

  RCUTILS_CHECK_ALLOCATOR_WITH_MSG(
    allocator, "allocator argument is invalid", return RMW_RET_INVALID_ARGUMENT);

  if (RMW_RET_OK != rmw_topic_endpoint_info_array_check_zero(subscriptions_info)) {
    return RMW_RET_INVALID_ARGUMENT;
  }

  auto common_context = &node->context->impl->common;
  std::string mangled_topic_name = topic_name;
  DemangleFunction demangle_type = _identity_demangle;
  if (!no_mangle) {
    mangled_topic_name =
      rmw_connextdds_create_topic_name(
      ROS_TOPIC_PREFIX, topic_name, "", false);
    demangle_type = _demangle_if_ros_type;
  }
  return common_context->graph_cache.get_readers_info_by_topic(
    mangled_topic_name,
    demangle_type,
    allocator,
    subscriptions_info);
}
