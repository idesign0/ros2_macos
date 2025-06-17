// Copyright 2020 Canonical Ltd
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

#include "rcutils/env.h"

// Connext 5.3.1 uses numeric levels (although note that v6 moved to using
// strings). These levels are:
// EMERGENCY: 0
// ALERT: 1
// CRITICAL: 2
// ERROR: 3
// WARNING: 4
// NOTICE: 5
// INFORMATIONAL: 6
// DEBUG: 7
//
// ROS has less logging levels, but it makes sense to use them here for
// consistency, so we have the following mapping.
const struct
{
  RCUTILS_LOG_SEVERITY ros_severity;
  const char * const dds_level;
} RMW_Connext_fv_VerbosityMapping[] =
{
  {RCUTILS_LOG_SEVERITY_FATAL, "0"},
  {RCUTILS_LOG_SEVERITY_ERROR, "3"},
  {RCUTILS_LOG_SEVERITY_WARN, "4"},
  {RCUTILS_LOG_SEVERITY_INFO, "6"},
  {RCUTILS_LOG_SEVERITY_DEBUG, "7"},
};

bool rmw_connextdds_severity_names_str(char buffer[], size_t buffer_size)
{
  size_t offset = 0;

  size_t severity_count = sizeof(RMW_Connext_fv_VerbosityMapping) /
    sizeof(RMW_Connext_fv_VerbosityMapping[0]);
  for (size_t i = 0; i < (severity_count - 1); ++i) {
    RCUTILS_LOG_SEVERITY severity =
      RMW_Connext_fv_VerbosityMapping[i].ros_severity;

    int length = rcutils_snprintf(
      buffer + offset, buffer_size - offset, "%s, ",
      g_rcutils_log_severity_names[severity]);

    if (length < 0) {
      return false;
    }

    offset += length;
    if (offset >= buffer_size) {
      return false;
    }
  }

  RCUTILS_LOG_SEVERITY severity =
    RMW_Connext_fv_VerbosityMapping[severity_count - 1].ros_severity;
  int length = rcutils_snprintf(
    buffer + offset, buffer_size - offset, "or %s",
    g_rcutils_log_severity_names[severity]);

  if (length < 0) {
    return false;
  }

  return (offset + length) <= buffer_size;
}

bool
rmw_connextdds_string_to_verbosity_level(
  const char * const str, const char ** level)
{
  int ros_severity;
  if (rcutils_logging_severity_level_from_string(
      str,
      rcutils_get_default_allocator(), &ros_severity) == RCUTILS_RET_OK)
  {
    for (const auto & item : RMW_Connext_fv_VerbosityMapping) {
      if (ros_severity == item.ros_severity) {
        *level = item.dds_level;
        return true;
      }
    }
  }

  return false;
}

bool
rmw_connextdds_validate_boolean(const char * const val)
{
  return nullptr != val &&
         (strcmp(val, "true") == 0 || strcmp(val, "false") == 0);
}

rmw_ret_t
rmw_connextdds_apply_security_logging_configuration(
  DDS_PropertyQosPolicy * const properties)
{
  // Check if secure logs should be saved to a local file.
  const char * env_val = nullptr;
  const char * lookup_rc =
    rcutils_get_env(RMW_CONNEXT_ENV_SECURITY_LOG_FILE, &env_val);

  if (nullptr != lookup_rc || nullptr == env_val) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to lookup from environment: "
      "var=%s, "
      "rc=%s ",
      RMW_CONNEXT_ENV_SECURITY_LOG_FILE,
      lookup_rc)
    return RMW_RET_ERROR;
  }

  if ('\0' != env_val[0]) {
    if (DDS_RETCODE_OK !=
      DDS_PropertyQosPolicyHelper_assert_property(
        properties,
        DDS_SECURITY_LOGGING_FILE_PROPERTY,
        env_val,
        RTI_FALSE))
    {
      RMW_CONNEXT_LOG_ERROR_A_SET(
        "failed to assert DDS property: '%s' = '%s'",
        DDS_SECURITY_LOGGING_FILE_PROPERTY, env_val)
      return RMW_RET_ERROR;
    }
  }

  // Check if secure logs should be distributed over DDS
  env_val = nullptr;
  lookup_rc =
    rcutils_get_env(RMW_CONNEXT_ENV_SECURITY_LOG_PUBLISH, &env_val);

  if (nullptr != lookup_rc || nullptr == env_val) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to lookup from environment: "
      "var=%s, "
      "rc=%s ",
      RMW_CONNEXT_ENV_SECURITY_LOG_PUBLISH,
      lookup_rc)
    return RMW_RET_ERROR;
  }

  if ('\0' != env_val[0]) {
    if (!rmw_connextdds_validate_boolean(env_val)) {
      RMW_CONNEXT_LOG_ERROR_A_SET(
        "invalid value for %s: '%s' (use 'true' or 'false')",
        RMW_CONNEXT_ENV_SECURITY_LOG_PUBLISH,
        env_val)
      return RMW_RET_ERROR;
    }

    if (DDS_RETCODE_OK !=
      DDS_PropertyQosPolicyHelper_assert_property(
        properties,
        DDS_SECURITY_LOGGING_DISTRIBUTE_PROPERTY,
        env_val,
        RTI_FALSE))
    {
      RMW_CONNEXT_LOG_ERROR_A_SET(
        "failed to assert DDS property: '%s' = '%s'",
        DDS_SECURITY_LOGGING_DISTRIBUTE_PROPERTY, env_val)
      return RMW_RET_ERROR;
    }
  }

  // Check verbosity level for secure logs.
  env_val = nullptr;
  lookup_rc =
    rcutils_get_env(RMW_CONNEXT_ENV_SECURITY_LOG_VERBOSITY, &env_val);

  if (nullptr != lookup_rc || nullptr == env_val) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to lookup from environment: "
      "var=%s, "
      "rc=%s ",
      RMW_CONNEXT_ENV_SECURITY_LOG_VERBOSITY,
      lookup_rc)
    return RMW_RET_ERROR;
  }

  if ('\0' != env_val[0]) {
    const char * level = nullptr;
    if (!rmw_connextdds_string_to_verbosity_level(env_val, &level)) {
      char humanized_severity_list[RCUTILS_ERROR_MESSAGE_MAX_LENGTH];
      if (!rmw_connextdds_severity_names_str(
          humanized_severity_list,
          RCUTILS_ERROR_MESSAGE_MAX_LENGTH))
      {
        RMW_CONNEXT_LOG_ERROR_SET("unable to create severity string")
        humanized_severity_list[0] = '\0';
      }
      RMW_CONNEXT_LOG_ERROR_A_SET(
        "invalid value for %s: '%s' (use one of: %s)",
        RMW_CONNEXT_ENV_SECURITY_LOG_VERBOSITY,
        env_val,
        humanized_severity_list);
      return RMW_RET_ERROR;
    }
    if (nullptr == level) {
      RMW_CONNEXT_LOG_ERROR_A_SET(
        "failed to convert value for %s: '%s'",
        RMW_CONNEXT_ENV_SECURITY_LOG_VERBOSITY,
        env_val)
      return RMW_RET_ERROR;
    }

    if (DDS_RETCODE_OK !=
      DDS_PropertyQosPolicyHelper_assert_property(
        properties,
        DDS_SECURITY_LOGGING_LEVEL_PROPERTY,
        level,
        RTI_FALSE))
    {
      RMW_CONNEXT_LOG_ERROR_A_SET(
        "failed to assert DDS property: '%s' = '%s'",
        DDS_SECURITY_LOGGING_LEVEL_PROPERTY, level)
      return RMW_RET_ERROR;
    }
  }

  return RMW_RET_OK;
}
