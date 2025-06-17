// Copyright 2021 Real-Time Innovations, Inc. (RTI)
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

/******************************************************************************
 * Event Listener API
 ******************************************************************************/
rmw_ret_t
rmw_api_connextdds_event_set_callback(
  rmw_event_t * const event,
  const rmw_event_callback_t callback,
  const void * const user_data)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(event, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    event,
    event->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INVALID_ARGUMENT);

  RMW_Connext_StatusCondition * condition = nullptr;
  if (RMW_Connext_Event::reader_event(event)) {
    condition = RMW_Connext_Event::subscriber(event)->condition();
  } else {
    condition = RMW_Connext_Event::publisher(event)->condition();
  }
  condition->set_new_event_callback(event->event_type, callback, user_data);
  return RMW_RET_OK;
}

/******************************************************************************
 * Service Listener API
 ******************************************************************************/
rmw_ret_t
rmw_api_connextdds_service_set_on_new_request_callback(
  rmw_service_t * const service,
  const rmw_event_callback_t callback,
  const void * const user_data)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(service, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    service,
    service->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  RMW_Connext_Service * const svc_impl =
    reinterpret_cast<RMW_Connext_Service *>(service->data);
  svc_impl->subscriber()->condition()->set_on_new_data_callback(callback, user_data);
  return RMW_RET_OK;
}

rmw_ret_t
rmw_api_connextdds_client_set_on_new_response_callback(
  rmw_client_t * const client,
  const rmw_event_callback_t callback,
  const void * const user_data)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(client, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    client,
    client->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  RMW_Connext_Client * const client_impl =
    reinterpret_cast<RMW_Connext_Client *>(client->data);
  client_impl->subscriber()->condition()->set_on_new_data_callback(callback, user_data);
  return RMW_RET_OK;
}

/******************************************************************************
 * Subscription Listener API
 ******************************************************************************/
rmw_ret_t
rmw_api_connextdds_subscription_set_on_new_message_callback(
  rmw_subscription_t * const subscription,
  const rmw_event_callback_t callback,
  const void * const user_data)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    subscription,
    subscription->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  RMW_Connext_Subscriber * const sub_impl =
    reinterpret_cast<RMW_Connext_Subscriber *>(subscription->data);
  sub_impl->condition()->set_on_new_data_callback(callback, user_data);
  return RMW_RET_OK;
}
