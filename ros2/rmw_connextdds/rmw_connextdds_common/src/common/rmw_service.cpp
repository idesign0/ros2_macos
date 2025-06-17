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
 * Clients/Servers functions
 ******************************************************************************/


rmw_ret_t
rmw_api_connextdds_take_response(
  const rmw_client_t * client,
  rmw_service_info_t * request_header,
  void * ros_response,
  bool * taken)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(client, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    client,
    client->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_ARGUMENT_FOR_NULL(request_header, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(ros_response, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(taken, RMW_RET_INVALID_ARGUMENT);

  RMW_Connext_Client * const client_impl =
    reinterpret_cast<RMW_Connext_Client *>(client->data);

  return client_impl->take_response(request_header, ros_response, taken);
}


rmw_ret_t
rmw_api_connextdds_take_request(
  const rmw_service_t * service,
  rmw_service_info_t * request_header,
  void * ros_request,
  bool * taken)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(service, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    service,
    service->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_ARGUMENT_FOR_NULL(request_header, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(ros_request, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(taken, RMW_RET_INVALID_ARGUMENT);

  RMW_Connext_Service * const svc_impl =
    reinterpret_cast<RMW_Connext_Service *>(service->data);

  return svc_impl->take_request(request_header, ros_request, taken);
}


rmw_ret_t
rmw_api_connextdds_send_response(
  const rmw_service_t * service,
  rmw_request_id_t * request_id,
  void * ros_response)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(service, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    service,
    service->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_ARGUMENT_FOR_NULL(request_id, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(ros_response, RMW_RET_INVALID_ARGUMENT);

  RMW_Connext_Service * const svc_impl =
    reinterpret_cast<RMW_Connext_Service *>(service->data);

  return svc_impl->send_response(request_id, ros_response);
}


rmw_ret_t
rmw_api_connextdds_send_request(
  const rmw_client_t * client,
  const void * ros_request,
  int64_t * sequence_id)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(client, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    client,
    client->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_ARGUMENT_FOR_NULL(ros_request, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(sequence_id, RMW_RET_INVALID_ARGUMENT);

  RMW_Connext_Client * const client_impl =
    reinterpret_cast<RMW_Connext_Client *>(client->data);

  return client_impl->send_request(ros_request, sequence_id);
}


rmw_client_t *
rmw_api_connextdds_create_client(
  const rmw_node_t * node,
  const rosidl_service_type_support_t * type_supports,
  const char * service_name,
  const rmw_qos_profile_t * qos_policies)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(node, nullptr);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    node,
    node->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return nullptr);
  RMW_CHECK_ARGUMENT_FOR_NULL(type_supports, nullptr);
  RMW_CHECK_ARGUMENT_FOR_NULL(service_name, nullptr);
  RMW_CHECK_ARGUMENT_FOR_NULL(qos_policies, nullptr);

  if (strlen(service_name) == 0) {
    RMW_CONNEXT_LOG_ERROR_SET("invalid service name")
    return nullptr;
  }

  if (!qos_policies->avoid_ros_namespace_conventions) {
    int validation_result = RMW_TOPIC_VALID;
    rmw_ret_t ret =
      rmw_validate_full_topic_name(service_name, &validation_result, nullptr);
    if (RMW_RET_OK != ret) {
      return nullptr;
    }
    if (RMW_TOPIC_VALID != validation_result) {
      const char * reason =
        rmw_full_topic_name_validation_result_string(validation_result);
      RMW_CONNEXT_LOG_ERROR_A_SET("invalid service name: %s", reason)
      return nullptr;
    }
  }

  RMW_CONNEXT_LOG_DEBUG_A(
    "creating new client: "
    "name=%s",
    service_name)

  rmw_context_impl_t * ctx = node->context->impl;
  std::lock_guard<std::mutex> guard(ctx->endpoint_mutex);

  RMW_Connext_Client * const client_impl =
    RMW_Connext_Client::create(
    ctx,
    ctx->participant,
    ctx->dds_pub,
    ctx->dds_sub,
    type_supports,
    service_name,
    qos_policies);

  if (nullptr == client_impl) {
    RMW_CONNEXT_LOG_ERROR(
      "failed to create RMW client implementation")
    return nullptr;
  }

  auto scope_exit_client_impl_delete = rcpputils::make_scope_exit(
    [client_impl]()
    {
      if (RMW_RET_OK != client_impl->finalize()) {
        RMW_CONNEXT_LOG_ERROR("failed to finalize client on error")
      }
      delete client_impl;
    });

  rmw_client_t * rmw_client = rmw_client_allocate();
  if (nullptr == rmw_client) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to create RMW client")
    return nullptr;
  }

  rmw_client->implementation_identifier = RMW_CONNEXTDDS_ID;
  rmw_client->data = client_impl;
  const size_t svc_name_len = strlen(service_name) + 1;
  rmw_client->service_name =
    reinterpret_cast<const char *>(rmw_allocate(svc_name_len));
  if (nullptr == rmw_client->service_name) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to allocate client name")
    return nullptr;
  }
  memcpy(
    const_cast<char *>(rmw_client->service_name),
    service_name,
    svc_name_len);

  if (RMW_RET_OK != client_impl->enable()) {
    RMW_CONNEXT_LOG_ERROR("failed to enable client")
    return nullptr;
  }

  if (RMW_RET_OK !=
    rmw_connextdds_graph_on_client_created(ctx, node, client_impl))
  {
    RMW_CONNEXT_LOG_ERROR("failed to update graph for client")
    return nullptr;
  }

  scope_exit_client_impl_delete.cancel();
  return rmw_client;
}


rmw_ret_t
rmw_api_connextdds_destroy_client(
  rmw_node_t * node,
  rmw_client_t * client)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    node,
    node->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_ARGUMENT_FOR_NULL(client, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    client,
    client->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  RMW_Connext_Client * const client_impl =
    reinterpret_cast<RMW_Connext_Client *>(client->data);

  rmw_context_impl_t * ctx = node->context->impl;
  std::lock_guard<std::mutex> guard(ctx->endpoint_mutex);

  if (RMW_RET_OK !=
    rmw_connextdds_graph_on_client_deleted(
      node->context->impl, node, client_impl))
  {
    RMW_CONNEXT_LOG_ERROR("failed to update graph for client")
    return RMW_RET_ERROR;
  }

  if (RMW_RET_OK != client_impl->finalize()) {
    RMW_CONNEXT_LOG_ERROR("failed to finalize RMW client implementation")
    return RMW_RET_ERROR;
  }

  delete client_impl;

  rmw_free(const_cast<char *>(client->service_name));
  rmw_client_free(client);

  return RMW_RET_OK;
}


rmw_ret_t
rmw_api_connextdds_client_request_publisher_get_actual_qos(
  const rmw_client_t * client,
  rmw_qos_profile_t * qos)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(client, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    client,
    client->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  RMW_CHECK_ARGUMENT_FOR_NULL(qos, RMW_RET_INVALID_ARGUMENT);

  RMW_Connext_Client * const client_impl =
    reinterpret_cast<RMW_Connext_Client *>(client->data);

  return client_impl->request_publisher_qos(qos);
}


rmw_ret_t
rmw_api_connextdds_client_response_subscription_get_actual_qos(
  const rmw_client_t * client,
  rmw_qos_profile_t * qos)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(client, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    client,
    client->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  RMW_CHECK_ARGUMENT_FOR_NULL(qos, RMW_RET_INVALID_ARGUMENT);

  RMW_Connext_Client * const client_impl =
    reinterpret_cast<RMW_Connext_Client *>(client->data);

  return client_impl->response_subscription_qos(qos);
}


rmw_service_t *
rmw_api_connextdds_create_service(
  const rmw_node_t * node,
  const rosidl_service_type_support_t * type_supports,
  const char * service_name,
  const rmw_qos_profile_t * qos_policies)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(node, nullptr);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    node,
    node->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return nullptr);
  RMW_CHECK_ARGUMENT_FOR_NULL(type_supports, nullptr);
  RMW_CHECK_ARGUMENT_FOR_NULL(service_name, nullptr);
  RMW_CHECK_ARGUMENT_FOR_NULL(qos_policies, nullptr);

  if (strlen(service_name) == 0) {
    RMW_CONNEXT_LOG_ERROR_SET("invalid service name")
    return nullptr;
  }

  if (!qos_policies->avoid_ros_namespace_conventions) {
    int validation_result = RMW_TOPIC_VALID;
    rmw_ret_t ret =
      rmw_validate_full_topic_name(service_name, &validation_result, nullptr);
    if (RMW_RET_OK != ret) {
      return nullptr;
    }
    if (RMW_TOPIC_VALID != validation_result) {
      const char * reason =
        rmw_full_topic_name_validation_result_string(validation_result);
      RMW_CONNEXT_LOG_ERROR_A_SET("invalid service name: %s", reason)
      return nullptr;
    }
  }

  RMW_CONNEXT_LOG_DEBUG_A(
    "creating new service: "
    "name=%s",
    service_name)

  rmw_context_impl_t * ctx = node->context->impl;
  std::lock_guard<std::mutex> guard(ctx->endpoint_mutex);

  RMW_Connext_Service * const svc_impl =
    RMW_Connext_Service::create(
    ctx,
    ctx->participant,
    ctx->dds_pub,
    ctx->dds_sub,
    type_supports,
    service_name,
    qos_policies);

  if (nullptr == svc_impl) {
    RMW_CONNEXT_LOG_ERROR(
      "failed to create RMW service implementation")
    return nullptr;
  }

  auto scope_exit_svc_impl_delete = rcpputils::make_scope_exit(
    [svc_impl]()
    {
      if (RMW_RET_OK != svc_impl->finalize()) {
        RMW_CONNEXT_LOG_ERROR("failed to finalize service on error")
      }
      delete svc_impl;
    });

  rmw_service_t * rmw_service = rmw_service_allocate();
  if (nullptr == rmw_service) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to create RMW service")
    return nullptr;
  }

  rmw_service->implementation_identifier = RMW_CONNEXTDDS_ID;
  rmw_service->data = svc_impl;
  const size_t svc_name_len = strlen(service_name) + 1;
  rmw_service->service_name =
    reinterpret_cast<const char *>(rmw_allocate(svc_name_len));
  if (nullptr == rmw_service->service_name) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to allocate service name")
    return nullptr;
  }
  memcpy(
    const_cast<char *>(rmw_service->service_name),
    service_name,
    svc_name_len);

  if (RMW_RET_OK != svc_impl->enable()) {
    RMW_CONNEXT_LOG_ERROR("failed to enable service")
    return nullptr;
  }

  if (RMW_RET_OK !=
    rmw_connextdds_graph_on_service_created(ctx, node, svc_impl))
  {
    RMW_CONNEXT_LOG_ERROR("failed to update graph for service")
    return nullptr;
  }

  scope_exit_svc_impl_delete.cancel();
  return rmw_service;
}


rmw_ret_t
rmw_api_connextdds_service_response_publisher_get_actual_qos(
  const rmw_service_t * service,
  rmw_qos_profile_t * qos)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(service, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    service,
    service->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  RMW_CHECK_ARGUMENT_FOR_NULL(qos, RMW_RET_INVALID_ARGUMENT);

  RMW_Connext_Service * const svc_impl =
    reinterpret_cast<RMW_Connext_Service *>(service->data);

  return svc_impl->response_publisher_qos(qos);
}


rmw_ret_t
rmw_api_connextdds_service_request_subscription_get_actual_qos(
  const rmw_service_t * service,
  rmw_qos_profile_t * qos)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(service, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    service,
    service->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  RMW_CHECK_ARGUMENT_FOR_NULL(qos, RMW_RET_INVALID_ARGUMENT);

  RMW_Connext_Service * const svc_impl =
    reinterpret_cast<RMW_Connext_Service *>(service->data);

  return svc_impl->request_subscription_qos(qos);
}


rmw_ret_t
rmw_api_connextdds_destroy_service(
  rmw_node_t * node,
  rmw_service_t * service)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    node,
    node->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_ARGUMENT_FOR_NULL(service, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    service,
    service->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  RMW_Connext_Service * const svc_impl =
    reinterpret_cast<RMW_Connext_Service *>(service->data);

  rmw_context_impl_t * ctx = node->context->impl;
  std::lock_guard<std::mutex> guard(ctx->endpoint_mutex);

  if (RMW_RET_OK !=
    rmw_connextdds_graph_on_service_deleted(
      node->context->impl, node, svc_impl))
  {
    RMW_CONNEXT_LOG_ERROR("failed to update graph for service")
    return RMW_RET_ERROR;
  }

  if (RMW_RET_OK != svc_impl->finalize()) {
    RMW_CONNEXT_LOG_ERROR("failed to finalize RMW service implementation")
    return RMW_RET_ERROR;
  }

  delete svc_impl;

  rmw_free(const_cast<char *>(service->service_name));
  rmw_service_free(service);

  return RMW_RET_OK;
}
