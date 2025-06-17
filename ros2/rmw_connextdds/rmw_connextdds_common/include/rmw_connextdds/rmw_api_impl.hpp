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

#ifndef RMW_CONNEXTDDS__RMW_API_IMPL_HPP_
#define RMW_CONNEXTDDS__RMW_API_IMPL_HPP_

#include "rmw_connextdds/context.hpp"

#include "rmw/features.h"

/*****************************************************************************
 * Context API
 *****************************************************************************/
RMW_CONNEXTDDS_PUBLIC
const char *
rmw_api_connextdds_get_implementation_identifier();

RMW_CONNEXTDDS_PUBLIC
const char *
rmw_api_connextdds_get_serialization_format();

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_set_log_severity(rmw_log_severity_t severity);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_init_options_init(
  rmw_init_options_t * init_options,
  rcutils_allocator_t allocator);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_init_options_copy(
  const rmw_init_options_t * src,
  rmw_init_options_t * dst);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_init_options_fini(rmw_init_options_t * init_options);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_init(
  const rmw_init_options_t * options,
  rmw_context_t * context);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_shutdown(rmw_context_t * context);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_context_fini(rmw_context_t * context);

/*****************************************************************************
 * Event API
 *****************************************************************************/
RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_publisher_event_init(
  rmw_event_t * rmw_event,
  const rmw_publisher_t * publisher,
  rmw_event_type_t event_type);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_subscription_event_init(
  rmw_event_t * rmw_event,
  const rmw_subscription_t * subscription,
  rmw_event_type_t event_type);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_take_event(
  const rmw_event_t * event_handle,
  void * event_info,
  bool * taken);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_event_set_callback(
  rmw_event_t * event,
  const rmw_event_callback_t callback,
  const void * const user_data);

/*****************************************************************************
 * Info API
 *****************************************************************************/
RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_get_node_names(
  const rmw_node_t * node,
  rcutils_string_array_t * node_names,
  rcutils_string_array_t * node_namespaces);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_get_node_names_with_enclaves(
  const rmw_node_t * node,
  rcutils_string_array_t * node_names,
  rcutils_string_array_t * node_namespaces,
  rcutils_string_array_t * enclaves);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_get_topic_names_and_types(
  const rmw_node_t * node,
  rcutils_allocator_t * allocator,
  bool no_demangle,
  rmw_names_and_types_t * tptyp);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_get_service_names_and_types(
  const rmw_node_t * node,
  rcutils_allocator_t * allocator,
  rmw_names_and_types_t * sntyp);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_service_server_is_available(
  const rmw_node_t * node,
  const rmw_client_t * client,
  bool * is_available);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_count_publishers(
  const rmw_node_t * node,
  const char * topic_name,
  size_t * count);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_count_subscribers(
  const rmw_node_t * node,
  const char * topic_name,
  size_t * count);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_get_subscriber_names_and_types_by_node(
  const rmw_node_t * node,
  rcutils_allocator_t * allocator,
  const char * node_name,
  const char * node_namespace,
  bool no_demangle,
  rmw_names_and_types_t * tptyp);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_get_publisher_names_and_types_by_node(
  const rmw_node_t * node,
  rcutils_allocator_t * allocator,
  const char * node_name,
  const char * node_namespace,
  bool no_demangle,
  rmw_names_and_types_t * tptyp);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_get_service_names_and_types_by_node(
  const rmw_node_t * node,
  rcutils_allocator_t * allocator,
  const char * node_name,
  const char * node_namespace,
  rmw_names_and_types_t * sntyp);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_get_client_names_and_types_by_node(
  const rmw_node_t * node,
  rcutils_allocator_t * allocator,
  const char * node_name,
  const char * node_namespace,
  rmw_names_and_types_t * sntyp);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_get_publishers_info_by_topic(
  const rmw_node_t * node,
  rcutils_allocator_t * allocator,
  const char * topic_name,
  bool no_mangle,
  rmw_topic_endpoint_info_array_t * publishers_info);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_get_subscriptions_info_by_topic(
  const rmw_node_t * node,
  rcutils_allocator_t * allocator,
  const char * topic_name,
  bool no_mangle,
  rmw_topic_endpoint_info_array_t * subscriptions_info);

/*****************************************************************************
 * Node API
 *****************************************************************************/
RMW_CONNEXTDDS_PUBLIC
rmw_node_t *
rmw_api_connextdds_create_node(
  rmw_context_t * context,
  const char * name,
  const char * ns);

RMW_CONNEXTDDS_PUBLIC

rmw_ret_t
rmw_api_connextdds_destroy_node(rmw_node_t * rmw_node);

RMW_CONNEXTDDS_PUBLIC
const rmw_guard_condition_t *
rmw_api_connextdds_node_get_graph_guard_condition(const rmw_node_t * rmw_node);

/*****************************************************************************
 * Publication API
 *****************************************************************************/
RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_publish(
  const rmw_publisher_t * publisher,
  const void * ros_message,
  rmw_publisher_allocation_t * allocation);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_publish_serialized_message(
  const rmw_publisher_t * publisher,
  const rmw_serialized_message_t * serialized_message,
  rmw_publisher_allocation_t * allocation);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_publish_loaned_message(
  const rmw_publisher_t * publisher,
  void * ros_message,
  rmw_publisher_allocation_t * allocation);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_init_publisher_allocation(
  const rosidl_message_type_support_t * type_support,
  const rosidl_runtime_c__Sequence__bound * message_bounds,
  rmw_publisher_allocation_t * allocation);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_fini_publisher_allocation(
  rmw_publisher_allocation_t * allocation);

RMW_CONNEXTDDS_PUBLIC
rmw_publisher_t *
rmw_api_connextdds_create_publisher(
  const rmw_node_t * node,
  const rosidl_message_type_support_t * type_supports,
  const char * topic_name,
  const rmw_qos_profile_t * qos_policies,
  const rmw_publisher_options_t * publisher_options);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_get_gid_for_publisher(
  const rmw_publisher_t * publisher,
  rmw_gid_t * gid);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_compare_gids_equal(
  const rmw_gid_t * gid1,
  const rmw_gid_t * gid2,
  bool * result);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_publisher_count_matched_subscriptions(
  const rmw_publisher_t * publisher,
  size_t * subscription_count);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_publisher_assert_liveliness(
  const rmw_publisher_t * publisher);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_publisher_wait_for_all_acked(
  const rmw_publisher_t * publisher,
  rmw_time_t wait_timeout);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_publisher_get_actual_qos(
  const rmw_publisher_t * publisher,
  rmw_qos_profile_t * qos);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_borrow_loaned_message(
  const rmw_publisher_t * publisher,
  const rosidl_message_type_support_t * type_support,
  void ** ros_message);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_return_loaned_message_from_publisher(
  const rmw_publisher_t * publisher,
  void * loaned_message);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_destroy_publisher(
  rmw_node_t * node,
  rmw_publisher_t * publisher);

/*****************************************************************************
 * Serialization API
 *****************************************************************************/
RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_get_serialized_message_size(
  const rosidl_message_type_support_t * type_supports,
  const rosidl_runtime_c__Sequence__bound * message_bounds,
  size_t * size);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_serialize(
  const void * ros_message,
  const rosidl_message_type_support_t * type_supports,
  rmw_serialized_message_t * serialized_message);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_deserialize(
  const rmw_serialized_message_t * serialized_message,
  const rosidl_message_type_support_t * type_supports,
  void * ros_message);

/*****************************************************************************
 * Service API
 *****************************************************************************/
RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_take_response(
  const rmw_client_t * client,
  rmw_service_info_t * request_header,
  void * ros_response,
  bool * taken);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_take_request(
  const rmw_service_t * service,
  rmw_service_info_t * request_header,
  void * ros_request,
  bool * taken);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_send_response(
  const rmw_service_t * service,
  rmw_request_id_t * request_id,
  void * ros_response);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_send_request(
  const rmw_client_t * client,
  const void * ros_request,
  int64_t * sequence_id);

RMW_CONNEXTDDS_PUBLIC
rmw_client_t *
rmw_api_connextdds_create_client(
  const rmw_node_t * node,
  const rosidl_service_type_support_t * type_supports,
  const char * service_name,
  const rmw_qos_profile_t * qos_policies);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_client_request_publisher_get_actual_qos(
  const rmw_client_t * client,
  rmw_qos_profile_t * qos);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_client_response_subscription_get_actual_qos(
  const rmw_client_t * client,
  rmw_qos_profile_t * qos);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_destroy_client(
  rmw_node_t * node,
  rmw_client_t * client);

RMW_CONNEXTDDS_PUBLIC
rmw_service_t *
rmw_api_connextdds_create_service(
  const rmw_node_t * node,
  const rosidl_service_type_support_t * type_supports,
  const char * service_name,
  const rmw_qos_profile_t * qos_policies);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_service_response_publisher_get_actual_qos(
  const rmw_service_t * service,
  rmw_qos_profile_t * qos);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_service_request_subscription_get_actual_qos(
  const rmw_service_t * service,
  rmw_qos_profile_t * qos);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_destroy_service(
  rmw_node_t * node,
  rmw_service_t * service);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_service_set_on_new_request_callback(
  rmw_service_t * rmw_service,
  const rmw_event_callback_t callback,
  const void * const user_data);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_client_set_on_new_response_callback(
  rmw_client_t * rmw_client,
  const rmw_event_callback_t callback,
  const void * const user_data);

/*****************************************************************************
 * Subscription API
 *****************************************************************************/
RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_init_subscription_allocation(
  const rosidl_message_type_support_t * type_support,
  const rosidl_runtime_c__Sequence__bound * message_bounds,
  rmw_subscription_allocation_t * allocation);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_fini_subscription_allocation(
  rmw_subscription_allocation_t * allocation);

RMW_CONNEXTDDS_PUBLIC
rmw_subscription_t *
rmw_api_connextdds_create_subscription(
  const rmw_node_t * node,
  const rosidl_message_type_support_t * type_supports,
  const char * topic_name,
  const rmw_qos_profile_t * qos_policies,
  const rmw_subscription_options_t * subscription_options);


RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_subscription_count_matched_publishers(
  const rmw_subscription_t * subscription, size_t * publisher_count);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_subscription_get_actual_qos(
  const rmw_subscription_t * subscription,
  rmw_qos_profile_t * qos);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_subscription_set_content_filter(
  rmw_subscription_t * subscription,
  const rmw_subscription_content_filter_options_t * options);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_subscription_get_content_filter(
  const rmw_subscription_t * subscription,
  rcutils_allocator_t * const allocator,
  rmw_subscription_content_filter_options_t * options);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_destroy_subscription(
  rmw_node_t * node,
  rmw_subscription_t * subscription);


RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_take(
  const rmw_subscription_t * subscription,
  void * ros_message,
  bool * taken,
  rmw_subscription_allocation_t * allocation);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_take_with_info(
  const rmw_subscription_t * subscription,
  void * ros_message,
  bool * taken,
  rmw_message_info_t * message_info,
  rmw_subscription_allocation_t * allocation);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_take_sequence(
  const rmw_subscription_t * subscription,
  size_t count,
  rmw_message_sequence_t * message_sequence,
  rmw_message_info_sequence_t * message_info_sequence,
  size_t * taken,
  rmw_subscription_allocation_t * allocation);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_take_serialized_message(
  const rmw_subscription_t * subscription,
  rmw_serialized_message_t * serialized_message,
  bool * taken,
  rmw_subscription_allocation_t * allocation);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_take_serialized_message_with_info(
  const rmw_subscription_t * subscription,
  rmw_serialized_message_t * serialized_message,
  bool * taken,
  rmw_message_info_t * message_info,
  rmw_subscription_allocation_t * allocation);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_take_loaned_message(
  const rmw_subscription_t * subscription,
  void ** loaned_message,
  bool * taken,
  rmw_subscription_allocation_t * allocation);


RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_take_loaned_message_with_info(
  const rmw_subscription_t * subscription,
  void ** loaned_message,
  bool * taken,
  rmw_message_info_t * message_info,
  rmw_subscription_allocation_t * allocation);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_return_loaned_message_from_subscription(
  const rmw_subscription_t * subscription,
  void * loaned_message);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_subscription_set_on_new_message_callback(
  rmw_subscription_t * const rmw_subscription,
  const rmw_event_callback_t callback,
  const void * const user_data);

/*****************************************************************************
 * WaitSet API
 *****************************************************************************/
RMW_CONNEXTDDS_PUBLIC
rmw_guard_condition_t *
rmw_api_connextdds_create_guard_condition(
  rmw_context_t * context);


RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_destroy_guard_condition(
  rmw_guard_condition_t * guard_condition_handle);


RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_trigger_guard_condition(
  const rmw_guard_condition_t * guard_condition_handle);


RMW_CONNEXTDDS_PUBLIC
rmw_wait_set_t *
rmw_api_connextdds_create_wait_set(
  rmw_context_t * context,
  size_t max_conditions);


RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_destroy_wait_set(rmw_wait_set_t * rmw_ws);


RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_wait(
  rmw_subscriptions_t * subs,
  rmw_guard_conditions_t * gcs,
  rmw_services_t * srvs,
  rmw_clients_t * cls,
  rmw_events_t * evs,
  rmw_wait_set_t * wait_set,
  const rmw_time_t * wait_timeout);

/******************************************************************************
 * QoS Profile functions
 ******************************************************************************/
RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_qos_profile_check_compatible(
  const rmw_qos_profile_t publisher_profile,
  const rmw_qos_profile_t subscription_profile,
  rmw_qos_compatibility_type_t * compatibility,
  char * reason,
  size_t reason_size);

/*****************************************************************************
 * Network Flow Endpoints API
 *****************************************************************************/
RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_publisher_get_network_flow_endpoints(
  const rmw_publisher_t * publisher,
  rcutils_allocator_t * allocator,
  rmw_network_flow_endpoint_array_t * network_flow_endpoint_array);

RMW_CONNEXTDDS_PUBLIC
rmw_ret_t
rmw_api_connextdds_subscription_get_network_flow_endpoints(
  const rmw_subscription_t * subscription,
  rcutils_allocator_t * allocator,
  rmw_network_flow_endpoint_array_t * network_flow_endpoint_array);

/******************************************************************************
 * Feature support functions
 ******************************************************************************/
RMW_CONNEXTDDS_PUBLIC
bool
rmw_api_connextdds_feature_supported(rmw_feature_t feature);

#endif  // RMW_CONNEXTDDS__RMW_API_IMPL_HPP_
