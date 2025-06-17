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

#include <string.h>
#include <string>

#include "rmw_connextdds/type_support.hpp"

#include "rmw_connextdds/rmw_impl.hpp"

static uint32_t
RMW_Connext_RequestReplyMapping_Basic_serialized_size_max(
  RMW_Connext_MessageTypeSupport * const type_support)
{
  // This function returns the max serialized size of the request header assuming
  // that the starting byte is "aligned" (for XCDR that means on a 4-byte boundary).
  // In order to support computation of this size from an unaligned starting
  // point, the logic would have to be extended to take into account possible
  // padding occurring before aligned members (e.g. sequence number,
  // string length), and the formal parameters would need to include an
  // "initial alignment" (similar to Connext's TypePlugin::get_serialized_size_max).
  // We return an "aligned size" to make sure we take into account any possible
  // padding that might be required to match the alignment of the first member
  // in the data payload. We align the output to 8-byte boundary.
  uint32_t result = 0;

  if (type_support->ctx()->cyclone_compatible) {
    /**
     * Cyclone uses a custom request header:
     * struct cdds_request_header_t
     * {
     *   uint64_t guid;
     *   int64_t seq;
     * };
     */
    result += 8 /* GUID */ + 8 /* SN */;
  } else {
    /**
     * Add request header to the serialized buffer:
     * struct SampleIdentity {
     *   GUID_t writer_guid;
     *   SequenceNumber_t sequence_number;
     * };
     * enum RemoteExceptionCode_t {
     *   REMOTE_EX_OK,
     *   ...
     * };
     * typedef string<255> InstanceName;
     * struct RequestHeader {
     *   SampleIndentity_t requestId;
     *   InstanceName instanceName;
     * };
     * struct ReplyHeader {
     *   dds::SampleIdentity relatedRequestId;
     *   dds::rpc::RemoteExceptionCode_t remoteEx;
     * };
     */
    static const uint32_t sample_identity_size = 16 /* GUID */ + 8 /* SN */;
    static const uint32_t instance_name_size =
      4 /* str_len */ + 1 /* nul */ + 3 /* possible padding */;
    static const uint32_t header_req_size = sample_identity_size + instance_name_size;
    static const uint32_t header_rep_size =
      sample_identity_size + 4 /* ex */ + 4 /* padding */;

    if (type_support->message_type() == RMW_CONNEXT_MESSAGE_REQUEST) {
      result += header_req_size;
    } else if (type_support->message_type() == RMW_CONNEXT_MESSAGE_REPLY) {
      result += header_rep_size;
    }
  }

  return result;
}

static rmw_ret_t
RMW_Connext_RequestReplyMapping_Basic_serialize(
  RMW_Connext_MessageTypeSupport * const type_support,
  eprosima::fastcdr::Cdr & cdr_stream,
  const RMW_Connext_RequestReplyMessage * const rr_msg)
{
  DDS_SampleIdentity_t sample_identity;

  rmw_ret_t rc = rmw_connextdds_gid_to_guid(
    rr_msg->gid, sample_identity.writer_guid);
  if (RMW_RET_OK != rc) {
    return rc;
  }
  sample_identity.sequence_number.high =
    static_cast<DDS_Long>((rr_msg->sn & 0xFFFFFFFF00000000) >> 8);
  sample_identity.sequence_number.low =
    static_cast<DDS_UnsignedLong>(rr_msg->sn & 0x00000000FFFFFFFF);

  try {
    // Cyclone only serializes 8 bytes of the writer guid.
    // We pick the last 8 to include instance and object ID.
    for (size_t i = (type_support->ctx()->cyclone_compatible) ? 8 : 0; i < 16; i++) {
      cdr_stream << sample_identity.writer_guid.value[i];
    }
    cdr_stream << sample_identity.sequence_number.high;
    cdr_stream << sample_identity.sequence_number.low;

    if (type_support->ctx()->cyclone_compatible) {
      return RMW_RET_OK;
    }

    switch (type_support->message_type()) {
      case RMW_CONNEXT_MESSAGE_REQUEST:
        {
          std::string instance_name = "";
          cdr_stream << instance_name;
          // static const uint32_t instance_name_len = 0;
          // cdr_stream << instance_name_len;
          // cdr_stream << '\0';
          break;
        }
      case RMW_CONNEXT_MESSAGE_REPLY:
        {
          static const int32_t sample_rc = 0;
          cdr_stream << sample_rc;
          break;
        }
      default:
        {
          RMW_CONNEXT_LOG_ERROR_A_SET(
            "invalid mapping type to serialize: %d",
            type_support->message_type())
          return RMW_RET_ERROR;
        }
    }

    return RMW_RET_OK;
  } catch (const std::exception & exc) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "Failed to serialize %s request header: %s",
      type_support->type_name(), exc.what())
    return RMW_RET_ERROR;
  } catch (...) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "Failed to serialize %s request header",
      type_support->type_name())
    return RMW_RET_ERROR;
  }
}

static rmw_ret_t
RMW_Connext_RequestReplyMapping_Basic_deserialize(
  RMW_Connext_MessageTypeSupport * const type_support,
  eprosima::fastcdr::Cdr & cdr_stream,
  RMW_Connext_RequestReplyMessage * const rr_msg)
{
  DDS_SampleIdentity_t sample_identity = DDS_SampleIdentity_UNKNOWN;

  try {
    // Cyclone only serializes 8 bytes of the writer guid.
    // We use these to fill the 8 MSB of the guid, and set the LSB to 0 for now
    // (they will be set to the writer's prefix after take()).
    for (int i = (type_support->ctx()->cyclone_compatible) ? 8 : 0; i < 16; i++) {
      cdr_stream >> sample_identity.writer_guid.value[i];
    }
    cdr_stream >> sample_identity.sequence_number.high;
    cdr_stream >> sample_identity.sequence_number.low;

    rmw_connextdds_guid_to_gid(sample_identity.writer_guid, rr_msg->gid);
    rmw_connextdds_sn_dds_to_ros(sample_identity.sequence_number, rr_msg->sn);
    rr_msg->gid.implementation_identifier = RMW_CONNEXTDDS_ID;

    if (type_support->ctx()->cyclone_compatible) {
      return RMW_RET_OK;
    }

    switch (type_support->message_type()) {
      case RMW_CONNEXT_MESSAGE_REQUEST:
        {
          std::string instance_name;
          cdr_stream >> instance_name;
          UNUSED_ARG(instance_name);
          break;
        }
      case RMW_CONNEXT_MESSAGE_REPLY:
        {
          int32_t sample_rc = 0;
          cdr_stream >> sample_rc;
          UNUSED_ARG(sample_rc);
          break;
        }
      default:
        {
          RMW_CONNEXT_LOG_ERROR_A_SET(
            "invalid mapping type to deserialize: %d",
            type_support->message_type())
          return RMW_RET_ERROR;
        }
    }

    return RMW_RET_OK;
  } catch (const std::exception & exc) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "Failed to deserialize %s request header: %s",
      type_support->type_name(), exc.what())
    return RMW_RET_ERROR;
  } catch (...) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "Failed to deserialize %s request header",
      type_support->type_name())
    return RMW_RET_ERROR;
  }
}
/******************************************************************************
 * RMW_Connext_MessageTypeSupport
 ******************************************************************************/

RMW_Connext_MessageTypeSupport::RMW_Connext_MessageTypeSupport(
  const RMW_Connext_MessageType message_type,
  const rosidl_message_type_support_t * const type_supports,
  const char * const type_name,
  rmw_context_impl_t * const ctx)
: _type_support_fastrtps(
    RMW_Connext_MessageTypeSupport::get_type_support_fastrtps(
      type_supports)),
  _unbounded(false),
  _empty(false),
  _serialized_size_max(0),
  _type_name(),
  _message_type(message_type),
  _ctx(ctx)
{
  if (this->_type_support_fastrtps == nullptr) {
    throw std::runtime_error("FastRTPS type support not found");
  }
  auto callbacks = this->callbacks_fastrtps();

  switch (this->_message_type) {
    case RMW_CONNEXT_MESSAGE_USERDATA:
      {
        this->_type_name = rmw_connextdds_create_type_name(callbacks);
        break;
      }
    case RMW_CONNEXT_MESSAGE_REQUEST:
    case RMW_CONNEXT_MESSAGE_REPLY:
      {
        RMW_CONNEXT_ASSERT(type_name != nullptr)
        this->_type_name = type_name;
        break;
      }
    default:
      // should never get here
      break;
  }

  RMW_Connext_MessageTypeSupport::type_info(
    this->_type_support_fastrtps,
    this->_serialized_size_max,
    this->_unbounded,
    this->_empty);

  if (this->type_requestreply() &&
    this->_ctx->request_reply_mapping == RMW_Connext_RequestReplyMapping::Basic)
  {
    this->_serialized_size_max +=
      RMW_Connext_RequestReplyMapping_Basic_serialized_size_max(this);
  }

  RMW_CONNEXT_LOG_DEBUG_A(
    "[type support] new %s: "
    "type.unbounded=%d, "
    "type.empty=%d, "
    "type.serialized_size_max=%u, "
    "type.reqreply=%d",
    this->type_name(),
    this->_unbounded,
    this->_empty,
    this->_serialized_size_max,
    this->type_requestreply())
}

rmw_ret_t RMW_Connext_MessageTypeSupport::serialize(
  const void * const ros_msg,
  rcutils_uint8_array_t * const to_buffer)
{
  auto callbacks =
    static_cast<const message_type_support_callbacks_t *>(
    this->_type_support_fastrtps->data);

  eprosima::fastcdr::FastBuffer cdr_buffer(
    reinterpret_cast<char *>(to_buffer->buffer),
    to_buffer->buffer_capacity);
  eprosima::fastcdr::Cdr cdr_stream(
    cdr_buffer,
    eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
    eprosima::fastcdr::Cdr::DDS_CDR);
  cdr_stream.setDDSCdrPlFlag(
    eprosima::fastcdr::Cdr::DDSCdrPlFlag::DDS_CDR_WITHOUT_PL);

  RMW_CONNEXT_LOG_TRACE_A(
    "[type support] %s serialize: "
    "type.unbounded=%d, "
    "type.empty=%d, "
    "type.serialized_size_max=%u, "
    "buffer.capacity=%lu",
    this->_type_name.c_str(),
    this->_unbounded,
    this->_empty,
    this->_serialized_size_max,
    to_buffer->buffer_capacity)

  const void * payload = ros_msg;

  try {
    cdr_stream.serialize_encapsulation();

    if (this->type_requestreply()) {
      const RMW_Connext_RequestReplyMessage * const rr_msg =
        reinterpret_cast<const RMW_Connext_RequestReplyMessage *>(ros_msg);
      payload = rr_msg->payload;


      if (this->_ctx->request_reply_mapping == RMW_Connext_RequestReplyMapping::Basic) {
        // encode the request's sequence number and originator's guid as a
        // header to the data payload
        rmw_ret_t head_rc = RMW_Connext_RequestReplyMapping_Basic_serialize(
          this, cdr_stream, rr_msg);
        if (RMW_RET_OK != head_rc) {
          return head_rc;
        }
      }
    }

    if (!this->_empty) {
      try {
        if (!callbacks->cdr_serialize(payload, cdr_stream)) {
          return RMW_RET_ERROR;
        }
      } catch (const std::exception & exc) {
        RMW_CONNEXT_LOG_ERROR_A_SET(
          "Failed to serialize %s sample: %s",
          this->type_name(), exc.what())
        return RMW_RET_ERROR;
      } catch (...) {
        RMW_CONNEXT_LOG_ERROR_A_SET(
          "Failed to serialize %s sample",
          this->type_name())
        return RMW_RET_ERROR;
      }
    } else {
      /* Serialize a dummy byte */
      cdr_stream << (uint8_t)0;
    }
  } catch (const std::exception & exc) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "Failed to serialize %s sample: %s", this->type_name(), exc.what())
    return RMW_RET_ERROR;
  } catch (...) {
    RMW_CONNEXT_LOG_ERROR_A_SET("Failed to serialize %s sample", this->type_name())
    return RMW_RET_ERROR;
  }

  to_buffer->buffer_length = cdr_stream.getSerializedDataLength();

  RMW_CONNEXT_LOG_DEBUG_A(
    "[type support] %s serialized: "
    "buffer.length=%lu",
    this->_type_name.c_str(),
    to_buffer->buffer_length)

  return RMW_RET_OK;
}

rmw_ret_t
RMW_Connext_MessageTypeSupport::deserialize(
  void * const ros_msg,
  const rcutils_uint8_array_t * const from_buffer,
  size_t & size_out,
  const bool header_only)
{
  auto callbacks =
    static_cast<const message_type_support_callbacks_t *>(
    this->_type_support_fastrtps->data);

  eprosima::fastcdr::FastBuffer cdr_buffer(
    reinterpret_cast<char *>(from_buffer->buffer),
    from_buffer->buffer_length);
  eprosima::fastcdr::Cdr cdr_stream(
    cdr_buffer,
    eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
    eprosima::fastcdr::Cdr::DDS_CDR);

  RMW_CONNEXT_LOG_TRACE_A(
    "[type support] %s deserialize: "
    "sample=%p, "
    "type.unbounded=%d, "
    "type.empty=%d, "
    "type.serialized_size_max=%u, "
    "buffer.length=%lu, "
    "buffer.capacity=%lu",
    this->type_name(),
    ros_msg,
    this->_unbounded,
    this->_empty,
    this->_serialized_size_max,
    from_buffer->buffer_length,
    from_buffer->buffer_capacity)

  void * payload = ros_msg;

  // This function is only called with (header_only == true) when used by
  // a request/reply endpoint using the "basic" mapping.
  if (header_only &&
    (!this->type_requestreply() ||
    this->_ctx->request_reply_mapping != RMW_Connext_RequestReplyMapping::Basic))
  {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "header_only used on non-request/reply or without basic mapping: %s",
      this->type_name())
    return RMW_RET_ERROR;
  }

  try {
    cdr_stream.read_encapsulation();

    if (this->type_requestreply()) {
      RMW_Connext_RequestReplyMessage * const rr_msg =
        reinterpret_cast<RMW_Connext_RequestReplyMessage *>(ros_msg);

      payload = reinterpret_cast<void *>(rr_msg->payload);

      if (this->_ctx->request_reply_mapping == RMW_Connext_RequestReplyMapping::Basic) {
        rmw_ret_t head_rc = RMW_Connext_RequestReplyMapping_Basic_deserialize(
          this, cdr_stream, rr_msg);
        if (RMW_RET_OK != head_rc) {
          return head_rc;
        }
        if (header_only) {
          return RMW_RET_OK;
        }
      }
    }

    if (!this->_empty) {
      try {
        if (!callbacks->cdr_deserialize(cdr_stream, payload)) {
          return RMW_RET_ERROR;
        }
      } catch (const std::exception & exc) {
        RMW_CONNEXT_LOG_ERROR_A_SET(
          "Failed to deserialize %s sample: %s", this->type_name(), exc.what())
        return RMW_RET_ERROR;
      } catch (...) {
        RMW_CONNEXT_LOG_ERROR_A_SET(
          "Failed to deserialize %s sample",
          this->type_name())
        return RMW_RET_ERROR;
      }
    } else {
      /* Consume dummy byte */
      uint8_t dummy = 0;
      cdr_stream >> dummy;
      UNUSED_ARG(dummy);
    }
  } catch (const std::exception & exc) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "Failed to deserialize %s sample: %s", this->type_name(), exc.what())
    return RMW_RET_ERROR;
  } catch (...) {
    RMW_CONNEXT_LOG_ERROR_A_SET("Failed to deserialize %s sample", this->type_name())
    return RMW_RET_ERROR;
  }

  size_out = cdr_stream.getSerializedDataLength();

  RMW_CONNEXT_LOG_DEBUG_A(
    "[type support] %s deserialized: "
    "consumed=%lu",
    this->_type_name.c_str(),
    size_out)

  return RMW_RET_OK;
}


uint32_t RMW_Connext_MessageTypeSupport::serialized_size_max(
  const void * const ros_msg,
  const bool include_encapsulation)
{
  /* Include size of encapsulation header in message's serialized size */
  uint32_t serialized_size = 0;

  if (include_encapsulation) {
    serialized_size +=
      RMW_Connext_MessageTypeSupport::ENCAPSULATION_HEADER_SIZE;
  }

  auto callbacks =
    static_cast<const message_type_support_callbacks_t *>(
    this->_type_support_fastrtps->data);


  if (!this->_unbounded) {
    serialized_size += this->_serialized_size_max;
  } else {
    const void * payload = ros_msg;
    if (this->type_requestreply()) {
      const RMW_Connext_RequestReplyMessage * rr_msg =
        reinterpret_cast<const RMW_Connext_RequestReplyMessage *>(ros_msg);
      payload = rr_msg->payload;
    }
    serialized_size += callbacks->get_serialized_size(payload);
    if (this->type_requestreply() &&
      this->_ctx->request_reply_mapping == RMW_Connext_RequestReplyMapping::Basic)
    {
      /* Add request header to serialized payload */
      serialized_size +=
        RMW_Connext_RequestReplyMapping_Basic_serialized_size_max(this);
    }
    RMW_CONNEXT_LOG_TRACE_A(
      "[type support] %s serialized size_MAX: %u",
      this->type_name(), serialized_size)
  }

  return serialized_size;
}

const rosidl_message_type_support_t *
RMW_Connext_MessageTypeSupport::get_type_support_fastrtps(
  const rosidl_message_type_support_t * const type_supports)
{
  const rosidl_message_type_support_t * type_support =
    get_message_typesupport_handle(
    type_supports, RMW_FASTRTPS_CPP_TYPESUPPORT_C);
  if (nullptr == type_support) {
    // Reset error string since this is not (yet) an error
    // (see https://github.com/ros2/rosidl_typesupport/pull/102)
    rcutils_error_string_t prev_error_string = rcutils_get_error_string();
    rcutils_reset_error();

    type_support = get_message_typesupport_handle(
      type_supports, RMW_FASTRTPS_CPP_TYPESUPPORT_CPP);
    if (nullptr == type_support) {
      rcutils_error_string_t error_string = rcutils_get_error_string();
      rcutils_reset_error();

      RMW_CONNEXT_LOG_ERROR_A_SET(
        "failed to load required fastrtps message type support. \n"
        "Received these errors:\n"
        "C: '%s'\n"
        "CPP: '%s'",
        prev_error_string.str,
        error_string.str);
    }
  }
  return type_support;
}

const rosidl_message_type_support_t *
RMW_Connext_MessageTypeSupport::get_type_support_intro(
  const rosidl_message_type_support_t * const type_supports,
  bool & cpp_version)
{
  const rosidl_message_type_support_t * type_support =
    get_message_typesupport_handle(
    type_supports, rosidl_typesupport_introspection_c__identifier);
  if (nullptr == type_support) {
    // Reset error string since this is not (yet) an error
    // (see https://github.com/ros2/rosidl_typesupport/pull/102)
    rcutils_error_string_t prev_error_string = rcutils_get_error_string();
    rcutils_reset_error();

    type_support =
      get_message_typesupport_handle(
      type_supports,
      rosidl_typesupport_introspection_cpp::typesupport_identifier);
    if (nullptr != type_support) {
      cpp_version = true;
    } else {
      rcutils_error_string_t error_string = rcutils_get_error_string();
      rcutils_reset_error();

      RMW_CONNEXT_LOG_ERROR_A_SET(
        "failed to load required introspection message type support. \n"
        "Received these errors:\n"
        "C: '%s'\n"
        "CPP: '%s'",
        prev_error_string.str,
        error_string.str);
    }
  } else {
    cpp_version = false;
  }
  return type_support;
}

RMW_Connext_MessageTypeSupport *
RMW_Connext_MessageTypeSupport::register_type_support(
  rmw_context_impl_t * const ctx,
  const rosidl_message_type_support_t * const type_supports,
  DDS_DomainParticipant * const participant,
  const RMW_Connext_MessageType message_type,
  const void * const intro_members,
  const bool intro_members_cpp,
  std::string * const type_name)
{
  RMW_Connext_MessageTypeSupport * type_support = nullptr;
  try {
    type_support = new RMW_Connext_MessageTypeSupport(
      message_type,
      type_supports,
      (nullptr != type_name) ? type_name->c_str() : nullptr,
      ctx);
  } catch (const std::exception & e) {
    RMW_CONNEXT_LOG_ERROR_A_SET("failed to create type support: %s", e.what())
  }

  if (nullptr == type_support) {
    return nullptr;
  }

  auto scope_exit_support_delete =
    rcpputils::make_scope_exit(
    [type_support]()
    {
      delete type_support;
    });

  rmw_ret_t rc = rmw_connextdds_register_type_support(
    ctx,
    type_supports,
    participant,
    message_type,
    intro_members,
    intro_members_cpp,
    type_support->type_name());
  if (RMW_RET_OK != rc) {
    RMW_CONNEXT_LOG_ERROR_A(
      "failed to register type support: %s",
      type_support->type_name())
    return nullptr;
  }

  scope_exit_support_delete.cancel();

  // This type support object must be freed by the caller (via delete)
  return type_support;
}

rmw_ret_t
RMW_Connext_MessageTypeSupport::unregister_type_support(
  rmw_context_impl_t * const ctx,
  DDS_DomainParticipant * const participant,
  const char * const type_name)
{
  return rmw_connextdds_unregister_type_support(ctx, participant, type_name);
}

void RMW_Connext_MessageTypeSupport::type_info(
  const rosidl_message_type_support_t * const type_support,
  uint32_t & serialized_size_max,
  bool & unbounded,
  bool & empty)
{
  serialized_size_max = 0;
  unbounded = false;
  empty = false;

  auto callbacks =
    static_cast<const message_type_support_callbacks_t *>(type_support->data);

  /* The fastrtps type support sets full_bounded to false if unbounded,
     but assumes full_bounded == true by default */
  bool full_bounded = true;

#ifdef ROSIDL_TYPESUPPORT_FASTRTPS_HAS_PLAIN_TYPES
  char bounds_info;
  serialized_size_max =
    static_cast<uint32_t>(callbacks->max_serialized_size(bounds_info));
  full_bounded = 0 != (bounds_info & ROSIDL_TYPESUPPORT_FASTRTPS_BOUNDED_TYPE);
#else
  serialized_size_max =
    static_cast<uint32_t>(callbacks->max_serialized_size(full_bounded));
#endif

  unbounded = !full_bounded;

  if (full_bounded && serialized_size_max == 0) {
    /* Empty message */
    empty = true;
    serialized_size_max = 1;
  }

  /* add encapsulation size to static serialized_size_max */
  serialized_size_max +=
    RMW_Connext_MessageTypeSupport::ENCAPSULATION_HEADER_SIZE;
}


/******************************************************************************
 * Service Type Support
 ******************************************************************************/
const rosidl_service_type_support_t *
RMW_Connext_ServiceTypeSupportWrapper::get_type_support_intro(
  const rosidl_service_type_support_t * const type_supports,
  bool & cpp_version)
{
  cpp_version = false;
  const rosidl_service_type_support_t * type_support =
    get_service_typesupport_handle(
    type_supports, rosidl_typesupport_introspection_c__identifier);

  if (nullptr == type_support) {
    // Reset error string since this is not (yet) an error
    // (see https://github.com/ros2/rosidl_typesupport/pull/102)
    rcutils_error_string_t prev_error_string = rcutils_get_error_string();
    rcutils_reset_error();

    type_support =
      get_service_typesupport_handle(
      type_supports,
      rosidl_typesupport_introspection_cpp::typesupport_identifier);
    if (nullptr != type_support) {
      cpp_version = true;
    } else {
      rcutils_error_string_t error_string = rcutils_get_error_string();
      rcutils_reset_error();

      RMW_CONNEXT_LOG_ERROR_A_SET(
        "failed to load required introspection service type support. \n"
        "Received these errors:\n"
        "C: '%s'\n"
        "CPP: '%s'",
        prev_error_string.str,
        error_string.str);
    }
  }

  return type_support;
}

const rosidl_service_type_support_t *
RMW_Connext_ServiceTypeSupportWrapper::get_type_support_fastrtps(
  const rosidl_service_type_support_t * const type_supports)
{
  const rosidl_service_type_support_t * type_support =
    get_service_typesupport_handle(
    type_supports, RMW_FASTRTPS_CPP_TYPESUPPORT_C);

  if (nullptr == type_support) {
    // Reset error string since this is not (yet) an error
    // (see https://github.com/ros2/rosidl_typesupport/pull/102)
    rcutils_error_string_t prev_error_string = rcutils_get_error_string();
    rcutils_reset_error();

    type_support = get_service_typesupport_handle(
      type_supports, RMW_FASTRTPS_CPP_TYPESUPPORT_CPP);

    if (nullptr == type_support) {
      rcutils_error_string_t error_string = rcutils_get_error_string();
      rcutils_reset_error();

      RMW_CONNEXT_LOG_ERROR_A_SET(
        "failed to load required fastrtps service type support. \n"
        "Received these errors:\n"
        "C: '%s'\n"
        "CPP: '%s'",
        prev_error_string.str,
        error_string.str);
    }
  }

  return type_support;
}

std::string
RMW_Connext_ServiceTypeSupportWrapper::get_request_type_name(
  const rosidl_service_type_support_t * const type_supports)
{
  const rosidl_service_type_support_t * const svc_type_support_fastrtps =
    RMW_Connext_ServiceTypeSupportWrapper::get_type_support_fastrtps(
    type_supports);
  if (nullptr == svc_type_support_fastrtps) {
    std::string empty_name;
    return empty_name;
  }
  auto svc_callbacks =
    static_cast<const service_type_support_callbacks_t *>(
    svc_type_support_fastrtps->data);

  return rmw_connextdds_create_type_name_request(svc_callbacks);
}

std::string
RMW_Connext_ServiceTypeSupportWrapper::get_response_type_name(
  const rosidl_service_type_support_t * const type_supports)
{
  const rosidl_service_type_support_t * const svc_type_support_fastrtps =
    RMW_Connext_ServiceTypeSupportWrapper::get_type_support_fastrtps(
    type_supports);
  if (nullptr == svc_type_support_fastrtps) {
    std::string empty_name;
    return empty_name;
  }
  auto svc_callbacks =
    static_cast<const service_type_support_callbacks_t *>(
    svc_type_support_fastrtps->data);

  return rmw_connextdds_create_type_name_response(svc_callbacks);
}


rmw_ret_t
RMW_Connext_Message_initialize(
  RMW_Connext_Message * const self,
  RMW_Connext_MessageTypeSupport * const type_support,
  const size_t data_buffer_size)
{
  const rcutils_allocator_t allocator = rcutils_get_default_allocator();

  self->user_data = nullptr;
  self->serialized = false;
  self->type_support = type_support;

  if (RCUTILS_RET_OK !=
    rcutils_uint8_array_init(&self->data_buffer, data_buffer_size, &allocator))
  {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to initialize message buffer: size=%lu",
      data_buffer_size)
    return RMW_RET_ERROR;
  }

  return RMW_RET_OK;
}

void
RMW_Connext_Message_finalize(RMW_Connext_Message * const self)
{
  if (RCUTILS_RET_OK != rcutils_uint8_array_fini(&self->data_buffer)) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to finalize uint8 array")
  }
}
