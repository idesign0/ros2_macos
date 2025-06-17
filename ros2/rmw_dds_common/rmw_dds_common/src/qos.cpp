// Copyright 2021 Open Source Robotics Foundation, Inc.
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

#include "rmw_dds_common/qos.hpp"

#include <cstdarg>
#include <cstring>

#include "rcutils/snprintf.h"
#include "rmw/error_handling.h"
#include "rmw/qos_profiles.h"
#include "rmw/qos_string_conversions.h"

namespace rmw_dds_common
{

static bool
operator==(rmw_time_t t1, rmw_time_t t2)
{
  return t1.sec == t2.sec && t1.nsec == t2.nsec;
}

static bool
operator!=(rmw_time_t t1, rmw_time_t t2)
{
  return !(t1 == t2);
}

static bool
operator<(rmw_time_t t1, rmw_time_t t2)
{
  if (t1.sec < t2.sec) {
    return true;
  } else if (t1.sec == t2.sec && t1.nsec < t2.nsec) {
    return true;
  }
  return false;
}

// Returns RMW_RET_OK if successful or no buffer was provided
// Returns RMW_RET_ERROR if there as an error copying the message to the buffer
static rmw_ret_t
_append_to_buffer(char * buffer, size_t buffer_size, const char * format, ...)
{
  // Only write if a buffer is provided
  if (!buffer || buffer_size == 0u) {
    return RMW_RET_OK;
  }
  // Determine available space left in buffer
  size_t offset = strnlen(buffer, buffer_size);
  size_t write_size = buffer_size - offset;
  std::va_list args;
  va_start(args, format);
  int snprintf_ret = rcutils_vsnprintf(buffer + offset, write_size, format, args);
  va_end(args);
  if (snprintf_ret < 0) {
    RMW_SET_ERROR_MSG("failed to append to character buffer");
    return RMW_RET_ERROR;
  }
  return RMW_RET_OK;
}

rmw_ret_t
qos_profile_check_compatible(
  const rmw_qos_profile_t publisher_qos,
  const rmw_qos_profile_t subscription_qos,
  rmw_qos_compatibility_type_t * compatibility,
  char * reason,
  size_t reason_size)
{
  if (!compatibility) {
    RMW_SET_ERROR_MSG("compatibility parameter is null");
    return RMW_RET_INVALID_ARGUMENT;
  }

  if (!reason && reason_size != 0u) {
    RMW_SET_ERROR_MSG("reason parameter is null, but reason_size parameter is not zero");
    return RMW_RET_INVALID_ARGUMENT;
  }

  // Presume profiles are compatible until proven otherwise
  *compatibility = RMW_QOS_COMPATIBILITY_OK;

  // Initialize reason buffer
  if (reason && reason_size != 0u) {
    reason[0] = '\0';
  }

  // Best effort publisher and reliable subscription
  if (publisher_qos.reliability == RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT &&
    subscription_qos.reliability == RMW_QOS_POLICY_RELIABILITY_RELIABLE)
  {
    *compatibility = RMW_QOS_COMPATIBILITY_ERROR;
    rmw_ret_t append_ret = _append_to_buffer(
      reason,
      reason_size,
      "ERROR: Best effort publisher and reliable subscription;");
    if (RMW_RET_OK != append_ret) {
      return append_ret;
    }
  }

  // Volatile publisher and transient local subscription
  if (publisher_qos.durability == RMW_QOS_POLICY_DURABILITY_VOLATILE &&
    subscription_qos.durability == RMW_QOS_POLICY_DURABILITY_TRANSIENT_LOCAL)
  {
    *compatibility = RMW_QOS_COMPATIBILITY_ERROR;
    rmw_ret_t append_ret = _append_to_buffer(
      reason,
      reason_size,
      "ERROR: Volatile publisher and transient local subscription;");
    if (RMW_RET_OK != append_ret) {
      return append_ret;
    }
  }

  const rmw_time_t & pub_deadline = publisher_qos.deadline;
  const rmw_time_t & sub_deadline = subscription_qos.deadline;
  const rmw_time_t deadline_default = RMW_QOS_DEADLINE_DEFAULT;

  // No deadline for publisher and deadline for subscription
  if (pub_deadline == deadline_default && sub_deadline != deadline_default) {
    *compatibility = RMW_QOS_COMPATIBILITY_ERROR;
    rmw_ret_t ret = _append_to_buffer(
      reason,
      reason_size,
      "ERROR: Subscription has a deadline, but publisher does not;");
    if (RMW_RET_OK != ret) {
      return ret;
    }
  }

  // Subscription deadline is less than publisher deadline
  if (pub_deadline != deadline_default && sub_deadline != deadline_default) {
    if (sub_deadline < pub_deadline) {
      *compatibility = RMW_QOS_COMPATIBILITY_ERROR;
      rmw_ret_t append_ret = _append_to_buffer(
        reason,
        reason_size,
        "ERROR: Subscription deadline is less than publisher deadline;");
      if (RMW_RET_OK != append_ret) {
        return append_ret;
      }
    }
  }

  // Automatic liveliness for publisher and manual by topic for subscription
  if (publisher_qos.liveliness == RMW_QOS_POLICY_LIVELINESS_AUTOMATIC &&
    subscription_qos.liveliness == RMW_QOS_POLICY_LIVELINESS_MANUAL_BY_TOPIC)
  {
    *compatibility = RMW_QOS_COMPATIBILITY_ERROR;
    rmw_ret_t append_ret = _append_to_buffer(
      reason,
      reason_size,
      "ERROR: Publisher's liveliness is automatic and subscription's is manual by topic;");
    if (RMW_RET_OK != append_ret) {
      return append_ret;
    }
  }

  const rmw_time_t & pub_lease = publisher_qos.liveliness_lease_duration;
  const rmw_time_t & sub_lease = subscription_qos.liveliness_lease_duration;
  const rmw_time_t lease_default = RMW_QOS_LIVELINESS_LEASE_DURATION_DEFAULT;

  // No lease duration for publisher and lease duration for subscription
  if (pub_lease == lease_default && sub_lease != lease_default) {
    *compatibility = RMW_QOS_COMPATIBILITY_ERROR;
    rmw_ret_t append_ret = _append_to_buffer(
      reason,
      reason_size,
      "ERROR: Subscription has a liveliness lease duration, but publisher does not;");
    if (RMW_RET_OK != append_ret) {
      return append_ret;
    }
  }

  // Subscription lease duration is less than publisher lease duration
  if (pub_lease != lease_default && sub_lease != lease_default) {
    if (sub_lease < pub_lease) {
      *compatibility = RMW_QOS_COMPATIBILITY_ERROR;
      rmw_ret_t append_ret = _append_to_buffer(
        reason,
        reason_size,
        "ERROR: Subscription liveliness lease duration is less than publisher;");
      if (RMW_RET_OK != append_ret) {
        return append_ret;
      }
    }
  }

  // Only check for warnings if there are no errors
  if (RMW_QOS_COMPATIBILITY_OK == *compatibility) {
    // We don't know the policy if the value is "system default" or "unknown"
    const bool pub_reliability_unknown =
      publisher_qos.reliability == RMW_QOS_POLICY_RELIABILITY_SYSTEM_DEFAULT ||
      publisher_qos.reliability == RMW_QOS_POLICY_RELIABILITY_UNKNOWN;
    const bool sub_reliability_unknown =
      subscription_qos.reliability == RMW_QOS_POLICY_RELIABILITY_SYSTEM_DEFAULT ||
      subscription_qos.reliability == RMW_QOS_POLICY_RELIABILITY_UNKNOWN;
    const bool pub_durability_unknown =
      publisher_qos.durability == RMW_QOS_POLICY_DURABILITY_SYSTEM_DEFAULT ||
      publisher_qos.durability == RMW_QOS_POLICY_DURABILITY_UNKNOWN;
    const bool sub_durability_unknown =
      subscription_qos.durability == RMW_QOS_POLICY_DURABILITY_SYSTEM_DEFAULT ||
      subscription_qos.durability == RMW_QOS_POLICY_DURABILITY_UNKNOWN;
    const bool pub_liveliness_unknown =
      publisher_qos.liveliness == RMW_QOS_POLICY_LIVELINESS_SYSTEM_DEFAULT ||
      publisher_qos.liveliness == RMW_QOS_POLICY_LIVELINESS_UNKNOWN;
    const bool sub_liveliness_unknown =
      subscription_qos.liveliness == RMW_QOS_POLICY_LIVELINESS_SYSTEM_DEFAULT ||
      subscription_qos.liveliness == RMW_QOS_POLICY_LIVELINESS_UNKNOWN;

    const char * pub_reliability_str = rmw_qos_reliability_policy_to_str(publisher_qos.reliability);
    if (!pub_reliability_str) {
      pub_reliability_str = "unknown";
    }
    const char * sub_reliability_str = rmw_qos_reliability_policy_to_str(
      subscription_qos.reliability);
    if (!sub_reliability_str) {
      sub_reliability_str = "unknown";
    }
    const char * pub_durability_str = rmw_qos_durability_policy_to_str(publisher_qos.durability);
    if (!pub_durability_str) {
      pub_durability_str = "unknown";
    }
    const char * sub_durability_str = rmw_qos_durability_policy_to_str(subscription_qos.durability);
    if (!sub_durability_str) {
      sub_durability_str = "unknown";
    }
    const char * pub_liveliness_str = rmw_qos_liveliness_policy_to_str(publisher_qos.liveliness);
    if (!pub_liveliness_str) {
      pub_liveliness_str = "unknown";
    }
    const char * sub_liveliness_str = rmw_qos_liveliness_policy_to_str(subscription_qos.liveliness);
    if (!sub_liveliness_str) {
      sub_liveliness_str = "unknown";
    }

    // Reliability warnings
    if (pub_reliability_unknown && sub_reliability_unknown) {
      // Reliability for publisher and subscription is unknown
      *compatibility = RMW_QOS_COMPATIBILITY_WARNING;
      rmw_ret_t append_ret = _append_to_buffer(
        reason,
        reason_size,
        "WARNING: Publisher reliability is %s and subscription reliability is %s;",
        pub_reliability_str,
        sub_reliability_str);
      if (RMW_RET_OK != append_ret) {
        return append_ret;
      }
    } else if (pub_reliability_unknown &&  // NOLINT
      subscription_qos.reliability == RMW_QOS_POLICY_RELIABILITY_RELIABLE)
    {
      // Reliability for publisher is unknown and subscription is reliable
      *compatibility = RMW_QOS_COMPATIBILITY_WARNING;
      rmw_ret_t append_ret = _append_to_buffer(
        reason,
        reason_size,
        "WARNING: Reliable subscription, but publisher is %s;",
        pub_reliability_str);
      if (RMW_RET_OK != append_ret) {
        return append_ret;
      }
    } else if (publisher_qos.reliability == RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT &&  // NOLINT
      sub_reliability_unknown)
    {
      // Reliability for publisher is best effort and subscription is unknown
      *compatibility = RMW_QOS_COMPATIBILITY_WARNING;
      rmw_ret_t append_ret = _append_to_buffer(
        reason,
        reason_size,
        "WARNING: Best effort publisher, but subscription is %s;",
        sub_reliability_str);
      if (RMW_RET_OK != append_ret) {
        return append_ret;
      }
    }

    // Durability warnings
    if (pub_durability_unknown && sub_durability_unknown) {
      // Durability for publisher and subscription is unknown
      *compatibility = RMW_QOS_COMPATIBILITY_WARNING;
      rmw_ret_t append_ret = _append_to_buffer(
        reason,
        reason_size,
        "WARNING: Publisher durabilty is %s and subscription durability is %s;",
        pub_durability_str,
        sub_durability_str);
      if (RMW_RET_OK != append_ret) {
        return append_ret;
      }
    } else if (pub_durability_unknown &&  // NOLINT
      subscription_qos.durability == RMW_QOS_POLICY_DURABILITY_TRANSIENT_LOCAL)
    {
      // Durability for publisher is unknown and subscription is transient local
      *compatibility = RMW_QOS_COMPATIBILITY_WARNING;
      rmw_ret_t ret = _append_to_buffer(
        reason,
        reason_size,
        "WARNING: Transient local subscription, but publisher is %s;",
        pub_durability_str);
      if (RMW_RET_OK != ret) {
        return ret;
      }
    } else if (publisher_qos.durability == RMW_QOS_POLICY_DURABILITY_VOLATILE &&  // NOLINT
      sub_durability_unknown)
    {
      // Durability for publisher is volatile and subscription is unknown
      *compatibility = RMW_QOS_COMPATIBILITY_WARNING;
      rmw_ret_t ret = _append_to_buffer(
        reason,
        reason_size,
        "WARNING: Volatile publisher, but subscription is %s;",
        sub_durability_str);
      if (RMW_RET_OK != ret) {
        return ret;
      }
    }

    // Liveliness warnings
    if (pub_liveliness_unknown && sub_liveliness_unknown) {
      // Liveliness for publisher and subscription is unknown
      *compatibility = RMW_QOS_COMPATIBILITY_WARNING;
      rmw_ret_t append_ret = _append_to_buffer(
        reason,
        reason_size,
        "WARNING: Publisher liveliness is %s and subscription liveliness is %s;",
        pub_liveliness_str,
        sub_liveliness_str);
      if (RMW_RET_OK != append_ret) {
        return append_ret;
      }
    } else if (pub_liveliness_unknown &&  // NOLINT
      subscription_qos.liveliness == RMW_QOS_POLICY_LIVELINESS_MANUAL_BY_TOPIC)
    {
      // Unknown liveliness for publisher and manual by topic for subscription
      *compatibility = RMW_QOS_COMPATIBILITY_WARNING;
      rmw_ret_t ret = _append_to_buffer(
        reason,
        reason_size,
        "WARNING: Subscription's liveliness is manual by topic, but publisher's is %s;",
        pub_liveliness_str);
      if (RMW_RET_OK != ret) {
        return ret;
      }
    } else if (publisher_qos.liveliness == RMW_QOS_POLICY_LIVELINESS_AUTOMATIC &&  // NOLINT
      sub_liveliness_unknown)
    {
      // Automatic liveliness for publisher and unknown for subscription
      *compatibility = RMW_QOS_COMPATIBILITY_WARNING;
      rmw_ret_t ret = _append_to_buffer(
        reason,
        reason_size,
        "WARNING: Publisher's liveliness is automatic, but subscription's is %s;",
        sub_liveliness_str);
      if (RMW_RET_OK != ret) {
        return ret;
      }
    }
  }

  return RMW_RET_OK;
}

}  // namespace rmw_dds_common
