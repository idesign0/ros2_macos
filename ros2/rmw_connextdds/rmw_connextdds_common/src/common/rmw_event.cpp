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

#include "rmw/event.h"

/******************************************************************************
 * Event functions
 ******************************************************************************/

static bool is_event_supported(const rmw_event_type_t event_type)
{
  bool invalid = false;
  ros_event_to_dds(event_type, &invalid);
  return !invalid;
}


rmw_ret_t
rmw_api_connextdds_publisher_event_init(
  rmw_event_t * rmw_event,
  const rmw_publisher_t * publisher,
  rmw_event_type_t event_type)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(publisher, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    publisher,
    publisher->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(rmw_event, RMW_RET_INVALID_ARGUMENT);

  if (!is_event_supported(event_type)) {
    RMW_CONNEXT_LOG_ERROR_SET("unsupported publisher event")
    return RMW_RET_UNSUPPORTED;
  }

  rmw_event->implementation_identifier = RMW_CONNEXTDDS_ID;
  rmw_event->data = publisher->data;
  rmw_event->event_type = event_type;

  RMW_CONNEXT_LOG_DEBUG_A(
    "new event: "
    "publisher=%p, "
    "event=%p, "
    "event.type=%s",
    reinterpret_cast<void *>(publisher->data),
    reinterpret_cast<void *>(rmw_event),
    dds_event_to_str(ros_event_to_dds(rmw_event->event_type, nullptr)))

  return RMW_RET_OK;
}


rmw_ret_t
rmw_api_connextdds_subscription_event_init(
  rmw_event_t * rmw_event,
  const rmw_subscription_t * subscription,
  rmw_event_type_t event_type)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    subscription,
    subscription->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(rmw_event, RMW_RET_INVALID_ARGUMENT);

  if (!is_event_supported(event_type)) {
    RMW_CONNEXT_LOG_ERROR_SET("unsupported subscription event")
    return RMW_RET_UNSUPPORTED;
  }

  rmw_event->implementation_identifier = RMW_CONNEXTDDS_ID;
  rmw_event->data = subscription->data;
  rmw_event->event_type = event_type;

  RMW_CONNEXT_LOG_DEBUG_A(
    "new event: "
    "subscriber=%p, "
    "event=%p, "
    "event.type=%s",
    reinterpret_cast<void *>(subscription->data),
    reinterpret_cast<void *>(rmw_event),
    dds_event_to_str(ros_event_to_dds(rmw_event->event_type, nullptr)))


  return RMW_RET_OK;
}


rmw_ret_t
rmw_api_connextdds_take_event(
  const rmw_event_t * event_handle,
  void * event_info,
  bool * taken)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(event_handle, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    event_handle,
    event_handle->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(event_info, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(taken, RMW_RET_INVALID_ARGUMENT);

  *taken = false;

  RMW_Connext_StatusCondition * condition = nullptr;
  if (RMW_Connext_Event::reader_event(event_handle)) {
    condition = RMW_Connext_Event::subscriber(event_handle)->condition();
  } else {
    condition = RMW_Connext_Event::publisher(event_handle)->condition();
  }
  rmw_ret_t rc = condition->get_status(event_handle->event_type, event_info);
  if (RMW_RET_OK != rc) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to get status from DDS entity")
    return rc;
  }

  *taken = true;
  return RMW_RET_OK;
}
