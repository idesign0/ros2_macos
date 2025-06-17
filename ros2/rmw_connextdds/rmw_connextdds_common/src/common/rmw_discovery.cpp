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
#include "rmw_connextdds/discovery.hpp"
#include "rmw_connextdds/graph_cache.hpp"

/******************************************************************************
 * Discovery Thread
 ******************************************************************************/

static
DDS_Condition *
rmw_connextdds_attach_reader_to_waitset(
  DDS_DataReader * const reader,
  DDS_WaitSet * const waitset)
{
  DDS_StatusCondition * const status_cond =
    DDS_Entity_get_statuscondition(
    DDS_DataReader_as_entity(reader));
  DDS_Condition * const cond = DDS_StatusCondition_as_condition(status_cond);

  if (DDS_RETCODE_OK !=
    DDS_StatusCondition_set_enabled_statuses(
      status_cond, DDS_DATA_AVAILABLE_STATUS))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to set datareader condition mask")
    return nullptr;
  }

  if (DDS_RETCODE_OK != DDS_WaitSet_attach_condition(waitset, cond)) {
    RMW_CONNEXT_LOG_ERROR_SET(
      "failed to attach status condition to waitset")
    return nullptr;
  }

  return cond;
}

static
void
rmw_connextdds_discovery_thread(rmw_context_impl_t * ctx)
{
  RMW_CONNEXT_LOG_DEBUG("[discovery thread] starting up...")

  RMW_Connext_Subscriber * const sub_partinfo =
    reinterpret_cast<RMW_Connext_Subscriber *>(ctx->common.sub->data);

  DDS_ConditionSeq active_conditions = DDS_SEQUENCE_INITIALIZER;
  DDS_ReturnCode_t rc = DDS_RETCODE_ERROR;
  DDS_UnsignedLong active_len = 0,
    i = 0;

  bool attached_exit = false,
    attached_partinfo = false;

  RMW_Connext_GuardCondition * const gcond_exit =
    reinterpret_cast<RMW_Connext_GuardCondition *>(
    ctx->common.listener_thread_gc->data);

  DDS_Condition * cond_active = nullptr;
  bool attached_dcps_part = false,
    attached_dcps_sub = false,
    attached_dcps_pub = false;

  DDS_Condition * cond_dcps_part = nullptr,
    * cond_dcps_pub = nullptr,
    * cond_dcps_sub = nullptr;

  DDS_Long attached_conditions = 0;
  bool active = false;

  DDS_WaitSet * waitset = DDS_WaitSet_new();
  if (nullptr == waitset) {
    RMW_CONNEXT_LOG_ERROR_SET(
      "failed to create waitset for discovery thread")
    return;
  }

  if (nullptr != ctx->dr_participants) {
    cond_dcps_part =
      rmw_connextdds_attach_reader_to_waitset(
      ctx->dr_participants, waitset);
    if (nullptr == cond_dcps_part) {
      goto cleanup;
    }
    attached_dcps_part = true;
    attached_conditions += 1;
  }
  if (nullptr != ctx->dr_publications) {
    cond_dcps_pub =
      rmw_connextdds_attach_reader_to_waitset(
      ctx->dr_publications, waitset);
    if (nullptr == cond_dcps_pub) {
      goto cleanup;
    }
    attached_dcps_pub = true;
    attached_conditions += 1;
  }
  if (nullptr != ctx->dr_subscriptions) {
    cond_dcps_sub =
      rmw_connextdds_attach_reader_to_waitset(
      ctx->dr_subscriptions, waitset);
    if (nullptr == cond_dcps_sub) {
      goto cleanup;
    }
    attached_dcps_sub = true;
    attached_conditions += 1;
  }

  if (RMW_RET_OK != sub_partinfo->condition()->reset_statuses()) {
    RMW_CONNEXT_LOG_ERROR("failed to reset participant info condition")
    goto cleanup;
  }

  if (RMW_RET_OK !=
    sub_partinfo->condition()->enable_statuses(DDS_DATA_AVAILABLE_STATUS))
  {
    RMW_CONNEXT_LOG_ERROR_SET(
      "failed to enable statuses on participant info condition")
    goto cleanup;
  }

  if (RMW_RET_OK != sub_partinfo->condition()->_attach(waitset)) {
    RMW_CONNEXT_LOG_ERROR_SET(
      "failed to attach participant info condition to "
      "discovery thread waitset")
    goto cleanup;
  }
  attached_partinfo = true;
  attached_conditions += 1;

  if (RMW_RET_OK != gcond_exit->_attach(waitset)) {
    RMW_CONNEXT_LOG_ERROR_SET(
      "failed to attach exit condition to discovery thread waitset")
    goto cleanup;
  }
  attached_exit = true;
  attached_conditions += 1;

  if (!DDS_ConditionSeq_set_maximum(&active_conditions, attached_conditions)) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to set condition seq maximum")
    goto cleanup;
  }

  RMW_CONNEXT_LOG_DEBUG("[discovery thread] main loop")

  active = ctx->common.thread_is_running.load();

  do {
    // Just in case we were killed before we event got a chance to run
    if (!active) {
      continue;
    }
    RMW_CONNEXT_LOG_TRACE("[discovery thread] waiting...")
    rc = DDS_WaitSet_wait(
      waitset, &active_conditions, &DDS_DURATION_INFINITE);

    if (DDS_RETCODE_OK != rc) {
      RMW_CONNEXT_LOG_ERROR_SET("wait failed for discovery thread")
      goto cleanup;
    }

    active_len = DDS_ConditionSeq_get_length(&active_conditions);

    RMW_CONNEXT_LOG_TRACE_A(
      "[discovery thread] active=%u", active_len)

    // First scan list of active conditions to check if we should terminate
    for (i = 0; i < active_len && active; i++) {
      cond_active =
        *DDS_ConditionSeq_get_reference(&active_conditions, i);
      if (gcond_exit->owns(cond_active)) {
        RMW_CONNEXT_LOG_DEBUG(
          "[discovery thread] exit condition active")
        /* exit without processing any further */
        active = false;
        continue;
      }
    }

    for (i = 0; i < active_len && active; i++) {
      cond_active =
        *DDS_ConditionSeq_get_reference(&active_conditions, i);
      if (sub_partinfo->condition()->owns(cond_active)) {
        RMW_CONNEXT_LOG_DEBUG(
          "[discovery thread] participant-info active")
        rmw_connextdds_graph_on_participant_info(ctx);
      } else if (nullptr != cond_dcps_part && cond_dcps_part == cond_active) {
        RMW_CONNEXT_LOG_DEBUG(
          "[discovery thread] dcps-participants active")
        rmw_connextdds_dcps_participant_on_data(ctx);
      } else if (nullptr != cond_dcps_pub && cond_dcps_pub == cond_active) {
        RMW_CONNEXT_LOG_DEBUG(
          "[discovery thread] dcps-publications active")
        rmw_connextdds_dcps_publication_on_data(ctx);
      } else if (nullptr != cond_dcps_sub && cond_dcps_sub == cond_active) {
        RMW_CONNEXT_LOG_DEBUG(
          "[discovery thread] dcps-subscriptions active")
        rmw_connextdds_dcps_subscription_on_data(ctx);
      } else {
        RMW_CONNEXT_LOG_ERROR_SET("unexpected active condition")
        goto cleanup;
      }
    }

    active = active && ctx->common.thread_is_running.load();
  } while (active);

  RMW_CONNEXT_LOG_DEBUG("[discovery thread] main loop terminated")

  cleanup :

  RMW_CONNEXT_LOG_DEBUG("[discovery thread] cleaning up...")

  DDS_ConditionSeq_finalize(&active_conditions);

  if (nullptr != waitset) {
    if (attached_exit) {
      if (RMW_RET_OK != gcond_exit->_detach(waitset)) {
        RMW_CONNEXT_LOG_ERROR_SET(
          "failed to detach graph condition from "
          "discovery thread waitset")
        return;
      }
    }
    if (attached_partinfo) {
      if (RMW_RET_OK != sub_partinfo->condition()->_detach(waitset)) {
        RMW_CONNEXT_LOG_ERROR_SET(
          "failed to detach participant info condition from "
          "discovery thread waitset")
        return;
      }
    }
    if (attached_dcps_part) {
      if (DDS_RETCODE_OK !=
        DDS_WaitSet_detach_condition(waitset, cond_dcps_part))
      {
        RMW_CONNEXT_LOG_ERROR_SET(
          "failed to detach DCPS Participant condition from "
          "discovery thread waitset")
        return;
      }
    }
    if (attached_dcps_sub) {
      if (DDS_RETCODE_OK !=
        DDS_WaitSet_detach_condition(waitset, cond_dcps_sub))
      {
        RMW_CONNEXT_LOG_ERROR_SET(
          "failed to detach DCPS Subscription condition from "
          "discovery thread waitset")
        return;
      }
    }
    if (attached_dcps_pub) {
      if (DDS_RETCODE_OK !=
        DDS_WaitSet_detach_condition(waitset, cond_dcps_pub))
      {
        RMW_CONNEXT_LOG_ERROR_SET(
          "failed to detach DCPS Publication condition from "
          "discovery thread waitset")
        return;
      }
    }
    DDS_WaitSet_delete(waitset);
  }

  RMW_CONNEXT_LOG_DEBUG("[discovery thread] done")
}

rmw_ret_t
rmw_connextdds_discovery_thread_start(rmw_context_impl_t * ctx)
{
  rmw_dds_common::Context * const common_ctx = &ctx->common;

  RMW_CONNEXT_LOG_DEBUG("starting discovery thread...")

  common_ctx->listener_thread_gc =
    rmw_connextdds_create_guard_condition(true /* internal */);
  if (nullptr == common_ctx->listener_thread_gc) {
    RMW_CONNEXT_LOG_ERROR(
      "failed to create discovery thread condition")
    return RMW_RET_ERROR;
  }

  common_ctx->thread_is_running.store(true);

  try {
    common_ctx->listener_thread =
      std::thread(rmw_connextdds_discovery_thread, ctx);

    RMW_CONNEXT_LOG_DEBUG("discovery thread started")

    return RMW_RET_OK;
  } catch (const std::exception & exc) {
    RMW_CONNEXT_LOG_ERROR_A_SET("Failed to create std::thread: %s", exc.what())
  } catch (...) {
    RMW_CONNEXT_LOG_ERROR_SET("Failed to create std::thread")
  }

  /* We'll get here only on error, so clean up things accordingly */

  common_ctx->thread_is_running.store(false);
  if (RMW_RET_OK !=
    rmw_connextdds_destroy_guard_condition(
      common_ctx->listener_thread_gc))
  {
    RMW_CONNEXT_LOG_ERROR(
      "Failed to destroy discovery thread guard condition")
  }

  return RMW_RET_ERROR;
}

rmw_ret_t
rmw_connextdds_discovery_thread_stop(rmw_context_impl_t * ctx)
{
  rmw_dds_common::Context * const common_ctx = &ctx->common;

  RMW_CONNEXT_LOG_DEBUG("stopping discovery thread...")

  if (common_ctx->thread_is_running.exchange(false)) {
    rmw_ret_t rmw_ret =
      rmw_api_connextdds_trigger_guard_condition(common_ctx->listener_thread_gc);

    if (RMW_RET_OK != rmw_ret) {
      return rmw_ret;
    }

    try {
      common_ctx->listener_thread.join();
    } catch (const std::exception & exc) {
      RMW_CONNEXT_LOG_ERROR_A_SET("Failed to join std::thread: %s", exc.what())
      return RMW_RET_ERROR;
    } catch (...) {
      RMW_CONNEXT_LOG_ERROR_SET("Failed to join std::thread")
      return RMW_RET_ERROR;
    }

    rmw_ret = rmw_connextdds_destroy_guard_condition(
      common_ctx->listener_thread_gc);
    if (RMW_RET_OK != rmw_ret) {
      return rmw_ret;
    }
  }

  RMW_CONNEXT_LOG_DEBUG("discovery thread stopped")
  return RMW_RET_OK;
}
