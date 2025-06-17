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

#ifndef RMW_CONNEXTDDS__LOG_HPP_
#define RMW_CONNEXTDDS__LOG_HPP_

#include "rmw_connextdds/static_config.hpp"

#include "rcutils/logging_macros.h"


#if RMW_CONNEXT_LOG_MODE == RMW_CONNEXT_LOG_MODE_ALL

#define RMW_CONNEXT_LOG_ERROR(msg_) \
  RCUTILS_LOG_ERROR_NAMED( \
    RMW_CONNEXTDDS_ID, \
    (msg_));

#define RMW_CONNEXT_LOG_ERROR_A(fmt_, ...) \
  RCUTILS_LOG_ERROR_NAMED( \
    RMW_CONNEXTDDS_ID, \
    (fmt_), \
    __VA_ARGS__);

#define RMW_CONNEXT_LOG_WARNING(msg_) \
  RCUTILS_LOG_WARN_NAMED( \
    RMW_CONNEXTDDS_ID, \
    (msg_));

#define RMW_CONNEXT_LOG_WARNING_A(fmt_, ...) \
  RCUTILS_LOG_WARN_NAMED( \
    RMW_CONNEXTDDS_ID, \
    (fmt_), \
    __VA_ARGS__);

#define RMW_CONNEXT_LOG_INFO(msg_) \
  RCUTILS_LOG_DEBUG_NAMED( \
    RMW_CONNEXTDDS_ID, \
    (msg_));

#define RMW_CONNEXT_LOG_INFO_A(fmt_, ...) \
  RCUTILS_LOG_INFO_NAMED( \
    RMW_CONNEXTDDS_ID, \
    (fmt_), \
    __VA_ARGS__);

#define RMW_CONNEXT_LOG_DEBUG(msg_) \
  RCUTILS_LOG_DEBUG_NAMED( \
    RMW_CONNEXTDDS_ID, \
    (msg_));

#define RMW_CONNEXT_LOG_DEBUG_A(fmt_, ...) \
  RCUTILS_LOG_DEBUG_NAMED( \
    RMW_CONNEXTDDS_ID, \
    (fmt_), \
    __VA_ARGS__);

#define RMW_CONNEXT_LOG_TRACE(msg_)

#define RMW_CONNEXT_LOG_TRACE_A(fmt_, ...)

#elif RMW_CONNEXT_LOG_MODE == RMW_CONNEXT_LOG_MODE_PRINTF

#include <stdio.h>

#define RMW_CONNEXT_LOG_ERROR_A(fmt_, ...) \
  printf("[ERROR][%s] " fmt_ "\n", RMW_CONNEXTDDS_ID, __VA_ARGS__);

#define RMW_CONNEXT_LOG_ERROR(msg_) \
  printf("[ERROR][%s] %s\n", RMW_CONNEXTDDS_ID, (msg_));

#define RMW_CONNEXT_LOG_WARNING_A(fmt_, ...) \
  printf("[WARNING][%s] " fmt_ "\n", RMW_CONNEXTDDS_ID, __VA_ARGS__);

#define RMW_CONNEXT_LOG_WARNING(msg_) \
  printf("[WARNING][%s] %s\n", RMW_CONNEXTDDS_ID, (msg_));

#define RMW_CONNEXT_LOG_DEBUG_A(fmt_, ...) \
  printf("[DEBUG][%s] " fmt_ "\n", RMW_CONNEXTDDS_ID, __VA_ARGS__);

#define RMW_CONNEXT_LOG_DEBUG(msg_) \
  printf("[DEBUG][%s] %s\n", RMW_CONNEXTDDS_ID, (msg_));

#define RMW_CONNEXT_LOG_INFO_A(fmt_, ...) \
  printf("[INFO][%s] " fmt_ "\n", RMW_CONNEXTDDS_ID, __VA_ARGS__);

#define RMW_CONNEXT_LOG_INFO(msg_) \
  printf("[INFO][%s] %s\n", RMW_CONNEXTDDS_ID, (msg_));

#define RMW_CONNEXT_LOG_TRACE(msg_)

#define RMW_CONNEXT_LOG_TRACE_A(fmt_, ...)

#elif RMW_CONNEXT_LOG_MODE == RMW_CONNEXT_LOG_MODE_DEFAULT

#define RMW_CONNEXT_LOG_ERROR(msg_) \
  RCUTILS_LOG_ERROR_NAMED( \
    RMW_CONNEXTDDS_ID, \
    (msg_));

#define RMW_CONNEXT_LOG_ERROR_A(fmt_, ...) \
  RCUTILS_LOG_ERROR_NAMED( \
    RMW_CONNEXTDDS_ID, \
    (fmt_), \
    __VA_ARGS__);

#define RMW_CONNEXT_LOG_WARNING(msg_) \
  RCUTILS_LOG_WARN_NAMED( \
    RMW_CONNEXTDDS_ID, \
    (msg_));

#define RMW_CONNEXT_LOG_WARNING_A(fmt_, ...) \
  RCUTILS_LOG_WARN_NAMED( \
    RMW_CONNEXTDDS_ID, \
    (fmt_), \
    __VA_ARGS__);

#define RMW_CONNEXT_LOG_INFO(msg_) \
  RCUTILS_LOG_DEBUG_NAMED( \
    RMW_CONNEXTDDS_ID, \
    (msg_));

#define RMW_CONNEXT_LOG_INFO_A(fmt_, ...) \
  RCUTILS_LOG_INFO_NAMED( \
    RMW_CONNEXTDDS_ID, \
    (fmt_), \
    __VA_ARGS__);

#define RMW_CONNEXT_LOG_DEBUG(msg_)

#define RMW_CONNEXT_LOG_DEBUG_A(fmt_, ...)

#define RMW_CONNEXT_LOG_TRACE(msg_)

#define RMW_CONNEXT_LOG_TRACE_A(fmt_, ...)

#else

#define RMW_CONNEXT_LOG_ERROR(msg_)

#define RMW_CONNEXT_LOG_ERROR_A(fmt_, ...)

#define RMW_CONNEXT_LOG_WARNING(msg_)

#define RMW_CONNEXT_LOG_WARNING_A(fmt_, ...)

#define RMW_CONNEXT_LOG_DEBUG(msg_)

#define RMW_CONNEXT_LOG_DEBUG_A(fmt_, ...)

#define RMW_CONNEXT_LOG_INFO(msg_)

#define RMW_CONNEXT_LOG_INFO_A(fmt_, ...)

#define RMW_CONNEXT_LOG_TRACE(msg_)

#define RMW_CONNEXT_LOG_TRACE_A(fmt_, ...)

#endif

#define RMW_CONNEXT_LOG_NOT_IMPLEMENTED \
  RMW_CONNEXT_LOG_ERROR_A("%s: not implemented", __func__)


#if RMW_CONNEXT_ASSERT_ENABLE
#include <cassert>

#define RMW_CONNEXT_ASSERT(cond) \
  assert(cond);

#else

#define RMW_CONNEXT_ASSERT(cond)

#endif /* RMW_CONNEXT_ASSERT_ENABLE */

#define RMW_CONNEXT_LOG_ERROR_SET(msg_) \
  rmw_reset_error(); \
  RMW_CONNEXT_LOG_ERROR(msg_) \
  RMW_SET_ERROR_MSG(msg_);

#define RMW_CONNEXT_LOG_ERROR_A_SET(fmt_, ...) \
  rmw_reset_error(); \
  RMW_CONNEXT_LOG_ERROR_A(fmt_, __VA_ARGS__) \
  RMW_SET_ERROR_MSG_WITH_FORMAT_STRING(fmt_, __VA_ARGS__);

#endif  // RMW_CONNEXTDDS__LOG_HPP_
