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

/******************************************************************************
 * Conditions & Waitset interface functions
 ******************************************************************************/

rmw_guard_condition_t *
rmw_api_connextdds_create_guard_condition(
  rmw_context_t * context)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(context, nullptr);
  RMW_CHECK_FOR_NULL_WITH_MSG(
    context->impl,
    "expected initialized context",
    return nullptr);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    context,
    context->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return nullptr);
  if (context->impl->is_shutdown) {
    RMW_CONNEXT_LOG_ERROR_SET("RMW context already shutdown")
    return nullptr;
  }
  rmw_guard_condition_t * const ret =
    rmw_connextdds_create_guard_condition(false /* internal */);
  RMW_CONNEXT_LOG_DEBUG_A(
    "new guard condition: %p",
    (ret != nullptr) ? reinterpret_cast<void *>(ret->data) : nullptr)
  return ret;
}

rmw_ret_t
rmw_api_connextdds_destroy_guard_condition(
  rmw_guard_condition_t * guard_condition_handle)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(
    guard_condition_handle, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    guard_condition_handle,
    guard_condition_handle->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  RMW_CONNEXT_LOG_DEBUG_A(
    "destroying guard condition: %p",
    reinterpret_cast<void *>(guard_condition_handle->data))
  return rmw_connextdds_destroy_guard_condition(guard_condition_handle);
}

rmw_ret_t
rmw_api_connextdds_trigger_guard_condition(
  const rmw_guard_condition_t * guard_condition_handle)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(
    guard_condition_handle, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    guard_condition_handle,
    guard_condition_handle->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  RMW_CONNEXT_LOG_TRACE_A(
    "triggering guard condition: %p",
    reinterpret_cast<void *>(guard_condition_handle->data))

  return rmw_connextdds_trigger_guard_condition(guard_condition_handle);
}

rmw_wait_set_t *
rmw_api_connextdds_create_wait_set(
  rmw_context_t * context,
  size_t max_conditions)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(context, nullptr);
  RMW_CHECK_FOR_NULL_WITH_MSG(
    context->impl,
    "expected initialized context",
    return nullptr);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    context,
    context->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return nullptr);

  rmw_wait_set_t * const ret = rmw_connextdds_create_waitset(max_conditions);
  RMW_CONNEXT_LOG_DEBUG_A("new waitset: %p", (void *)ret)
  return ret;
}

rmw_ret_t
rmw_api_connextdds_destroy_wait_set(rmw_wait_set_t * rmw_ws)
{
  // TODO(asorbini): Return RMW_RET_INVALID_ARGUMENT. We return RMW_RET_ERROR
  // because that's what's expected by test_rmw_implementation
  RMW_CHECK_ARGUMENT_FOR_NULL(rmw_ws, RMW_RET_ERROR);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    rmw_ws,
    rmw_ws->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  RMW_CONNEXT_LOG_DEBUG_A("destroying waitset: %p", (void *)rmw_ws)

  return rmw_connextdds_destroy_waitset(rmw_ws);
}

rmw_ret_t
rmw_api_connextdds_wait(
  rmw_subscriptions_t * subs,
  rmw_guard_conditions_t * gcs,
  rmw_services_t * srvs,
  rmw_clients_t * cls,
  rmw_events_t * evs,
  rmw_wait_set_t * wait_set,
  const rmw_time_t * wait_timeout)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(wait_set, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    wait_set,
    wait_set->implementation_identifier,
    RMW_CONNEXTDDS_ID,
    return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  RMW_Connext_WaitSet * const ws_impl =
    reinterpret_cast<RMW_Connext_WaitSet *>(wait_set->data);

  return ws_impl->wait(subs, gcs, srvs, cls, evs, wait_timeout);
}
