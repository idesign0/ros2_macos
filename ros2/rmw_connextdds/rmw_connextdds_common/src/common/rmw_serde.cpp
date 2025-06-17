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
 * Serialization functions
 ******************************************************************************/


rmw_ret_t
rmw_api_connextdds_get_serialized_message_size(
  const rosidl_message_type_support_t * type_supports,
  const rosidl_runtime_c__Sequence__bound * message_bounds,
  size_t * size)
{
  UNUSED_ARG(type_supports);
  UNUSED_ARG(message_bounds);
  UNUSED_ARG(size);
  RMW_CONNEXT_LOG_NOT_IMPLEMENTED
  return RMW_RET_UNSUPPORTED;
}


rmw_ret_t
rmw_api_connextdds_serialize(
  const void * ros_message,
  const rosidl_message_type_support_t * type_supports,
  rmw_serialized_message_t * serialized_message)
{
  try {
    // declare a mock context struct to build a temporary type support
    rmw_context_t ctx_base;
    ctx_base.options.localhost_only = RMW_LOCALHOST_ONLY_DISABLED;
    rmw_context_impl_t ctx(&ctx_base);
    ctx.request_reply_mapping = RMW_Connext_RequestReplyMapping::Extended;
    RMW_Connext_MessageTypeSupport type_support(
      RMW_CONNEXT_MESSAGE_USERDATA, type_supports, nullptr, &ctx);

    const uint32_t ser_size = type_support.serialized_size_max(ros_message);
    rmw_ret_t ret =
      rmw_serialized_message_resize(serialized_message, ser_size);
    if (RMW_RET_OK != ret) {
      RMW_CONNEXT_LOG_ERROR_A_SET(
        "failed to allocate serialized message buffer: type=%s, size=%u",
        type_support.type_name(), ser_size)
      return ret;
    }

    return type_support.serialize(ros_message, serialized_message);
  } catch (const std::exception & exc) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to serialize message to buffer: error=%s", exc.what())
  } catch (...) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to serialize message to buffer")
  }

  return RMW_RET_ERROR;
}


rmw_ret_t
rmw_api_connextdds_deserialize(
  const rmw_serialized_message_t * serialized_message,
  const rosidl_message_type_support_t * type_supports,
  void * ros_message)
{
  try {
    // declare a mock context struct to build a temporary type support
    rmw_context_t ctx_base;
    ctx_base.options.localhost_only = RMW_LOCALHOST_ONLY_DISABLED;
    rmw_context_impl_t ctx(&ctx_base);
    ctx.request_reply_mapping = RMW_Connext_RequestReplyMapping::Extended;
    RMW_Connext_MessageTypeSupport type_support(
      RMW_CONNEXT_MESSAGE_USERDATA, type_supports, nullptr, &ctx);
    size_t deserialized_size = 0;
    rmw_ret_t ret =
      type_support.deserialize(
      ros_message, serialized_message, deserialized_size);
    UNUSED_ARG(deserialized_size);
    return ret;
  } catch (const std::exception & exc) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to deserialize message from buffer: error=%s", exc.what())
  } catch (...) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to deserialize message from buffer")
  }

  return RMW_RET_ERROR;
}
