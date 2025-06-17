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

#include <string>

#include "rmw_connextdds/typecode.hpp"
#include "rmw_connextdds/rmw_impl.hpp"


static RTIBool
RMW_Connext_TypeCodePtr_initialize_w_params(
  DDS_TypeCode ** self,
  const struct DDS_TypeAllocationParams_t * allocParams)
{
  UNUSED_ARG(allocParams);
  *self = NULL;
  return RTI_TRUE;
}

static RTIBool
RMW_Connext_TypeCodePtr_finalize_w_params(
  DDS_TypeCode ** self,
  const struct DDS_TypeDeallocationParams_t * deallocParams)
{
  UNUSED_ARG(deallocParams);
  *self = NULL;
  return RTI_TRUE;
}

static RTIBool
RMW_Connext_TypeCodePtr_copy(
  DDS_TypeCode ** dst,
  const DDS_TypeCode ** src)
{
  *dst = const_cast<DDS_TypeCode *>(*src);
  return RTI_TRUE;
}

#define T                       DDS_TypeCode *
#define TSeq                    RMW_Connext_TypeCodePtrSeq
#define T_initialize_w_params   RMW_Connext_TypeCodePtr_initialize_w_params
#define T_finalize_w_params     RMW_Connext_TypeCodePtr_finalize_w_params
#define T_copy                  RMW_Connext_TypeCodePtr_copy
#include "dds_c/generic/dds_c_sequence_TSeq.gen"

static
DDS_TypeCode *
RMW_Connext_TypeCodePtrSeq_lookup_by_name(
  RMW_Connext_TypeCodePtrSeq * const self,
  const char * const name)
{
  const DDS_Long seq_len = RMW_Connext_TypeCodePtrSeq_get_length(self);
  for (DDS_Long i = 0; i < seq_len; i++) {
    DDS_TypeCode * const tc =
      *RMW_Connext_TypeCodePtrSeq_get_reference(self, i);
    if (nullptr == tc) {
      continue;
    }
    DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;
    const char * const tc_name = DDS_TypeCode_name(tc, &ex);
    if (nullptr == tc_name || DDS_NO_EXCEPTION_CODE != ex) {
      // ignore element
      continue;
    }
    if (strcmp(name, tc_name) == 0) {
      return tc;
    }
  }

  return nullptr;
}

static
RTIBool
RMW_Connext_TypeCodePtrSeq_assert_empty_slot(
  RMW_Connext_TypeCodePtrSeq * const self,
  DDS_Long * const slot_i)
{
  const DDS_Long seq_len = RMW_Connext_TypeCodePtrSeq_get_length(self);
  for (DDS_Long i = 0; i < seq_len; i++) {
    DDS_TypeCode * const tc =
      *RMW_Connext_TypeCodePtrSeq_get_reference(self, i);
    if (nullptr == tc) {
      *slot_i = i;
      return RTI_TRUE;
    }
  }
  if (!RMW_Connext_TypeCodePtrSeq_ensure_length(
      self, seq_len + 1, seq_len + 1))
  {
    return RTI_FALSE;
  }
  *slot_i = seq_len;
  return RTI_TRUE;
}
static
RTIBool
RMW_Connext_TypeCodePtrSeq_append(
  RMW_Connext_TypeCodePtrSeq * const self,
  DDS_TypeCode * const tc)
{
  DDS_Long slot_i = 0;

  if (!RMW_Connext_TypeCodePtrSeq_assert_empty_slot(self, &slot_i)) {
    return RTI_FALSE;
  }

  *RMW_Connext_TypeCodePtrSeq_get_reference(self, slot_i) = tc;

  return RTI_TRUE;
}

static
DDS_TypeCode *
RMW_Connext_TypeCodePtrSeq_assert_from_ros(
  RMW_Connext_MessageTypeSupport * const type_support,
  struct RMW_Connext_TypeCodePtrSeq * const self,
  const rosidl_message_type_support_t * const type_supports,
  const char * const type_name,
  DDS_TypeCode * const dds_tc = nullptr,
  bool * const added = nullptr)
{
  if (nullptr != added) {
    *added = false;
  }

  DDS_TypeCode * tc = nullptr;

  if (nullptr != type_name) {
    tc = RMW_Connext_TypeCodePtrSeq_lookup_by_name(self, type_name);

    if (nullptr != tc) {
      return tc;
    }
  }

  if (nullptr != dds_tc) {
    tc = dds_tc;
  } else {
    tc = rmw_connextdds_create_typecode(
      type_support,
      type_supports,
      type_name,
      nullptr /* intro_members */,
      false /* intro_members_cpp */,
      self);
    if (nullptr == tc) {
      return nullptr;
    }
  }

  if (!RMW_Connext_TypeCodePtrSeq_append(self, tc)) {
    rmw_connextdds_delete_typecode(tc);
    return nullptr;
  }

  if (nullptr != added) {
    *added = true;
  }

  return tc;
}

static
void
RMW_Connext_TypeCodePtrSeq_finalize_elements(
  struct RMW_Connext_TypeCodePtrSeq * const self)
{
  const DDS_Long seq_len = RMW_Connext_TypeCodePtrSeq_get_length(self);
  for (DDS_Long i = 0; i < seq_len; i++) {
    DDS_TypeCode ** const tc =
      RMW_Connext_TypeCodePtrSeq_get_reference(self, i);
    if (nullptr == *tc) {
      continue;
    }
    rmw_connextdds_delete_typecode(*tc);
    *tc = nullptr;
  }
}

/******************************************************************************
 * Static TypeCodes for the Request/Reply header with "basic" mapping.
 ******************************************************************************/
static DDS_TypeCode *
GUID_get_typecode(
  DDS_TypeCodeFactory * const tc_factory,
  struct RMW_Connext_TypeCodePtrSeq * const tc_cache)
{
  DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;

  struct DDS_UnsignedLongSeq tc_octet_array_dimensions = DDS_SEQUENCE_INITIALIZER;
  if (!DDS_UnsignedLongSeq_ensure_length(&tc_octet_array_dimensions, 1, 1)) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to resize dimensions sequence")
    return nullptr;
  }
  struct DDS_UnsignedLongSeq * const tc_octet_array_dimensions_ptr =
    &tc_octet_array_dimensions;
  *DDS_UnsignedLongSeq_get_reference(&tc_octet_array_dimensions, 0) = 16;
  auto scope_exit_tc_octet_array_dim = rcpputils::make_scope_exit(
    [tc_octet_array_dimensions_ptr]()
    {
      if (!DDS_UnsignedLongSeq_finalize(tc_octet_array_dimensions_ptr)) {
        RMW_CONNEXT_LOG_ERROR("failed to finalize dimensions sequence")
      }
    });

  DDS_TypeCode * const tc_octet_array =
    DDS_TypeCodeFactory_create_array_tc(
    tc_factory, &tc_octet_array_dimensions, &DDS_g_tc_octet, &ex);
  if (nullptr == tc_octet_array || DDS_NO_EXCEPTION_CODE != ex) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to create TypeCode for octet array: %d", ex)
    return nullptr;
  }
  auto scope_exit_tc_octet_array = rcpputils::make_scope_exit(
    [tc_factory, tc_octet_array]()
    {
      DDS_ExceptionCode_t fex = DDS_NO_EXCEPTION_CODE;
      DDS_TypeCodeFactory_delete_tc(tc_factory, tc_octet_array, &fex);
      if (DDS_NO_EXCEPTION_CODE != fex) {
        RMW_CONNEXT_LOG_ERROR_A(
          "failed to delete TypeCode for octet array: %d", fex)
      }
    });

  struct DDS_StructMemberSeq tc_guid_members = DDS_SEQUENCE_INITIALIZER;
  if (!DDS_StructMemberSeq_ensure_length(&tc_guid_members, 1, 1)) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to DDS_GUID_t members sequence")
    return nullptr;
  }
  struct DDS_StructMemberSeq * const tc_guid_members_ptr = &tc_guid_members;
  auto scope_exit_tc_guid_members = rcpputils::make_scope_exit(
    [tc_guid_members_ptr]()
    {
      if (!DDS_StructMemberSeq_finalize(tc_guid_members_ptr)) {
        RMW_CONNEXT_LOG_ERROR("failed to finalize DDS_GUID_t members sequence")
      }
    });
  DDS_StructMember * tc_member =
    DDS_StructMemberSeq_get_reference(&tc_guid_members, 0);
  tc_member->name = DDS_String_dup("value");
  if (nullptr == tc_member->name) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to allocate string")
    return nullptr;
  }
  tc_member->type = tc_octet_array;

  DDS_TypeCode * const tc_guid =
    DDS_TypeCodeFactory_create_struct_tc(
    tc_factory, "DDS_GUID_t", &tc_guid_members, &ex);
  if (nullptr == tc_guid || DDS_NO_EXCEPTION_CODE != ex) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to create TypeCode for DDS_GUID_t: %d", ex)
    return nullptr;
  }
  auto scope_exit_tc_guid = rcpputils::make_scope_exit(
    [tc_factory, tc_guid]()
    {
      DDS_ExceptionCode_t fex = DDS_NO_EXCEPTION_CODE;
      DDS_TypeCodeFactory_delete_tc(tc_factory, tc_guid, &fex);
      if (DDS_NO_EXCEPTION_CODE != fex) {
        RMW_CONNEXT_LOG_ERROR_A(
          "failed to delete TypeCode for DDS_GUID_t: %d", fex)
      }
    });

  if (!RMW_Connext_TypeCodePtrSeq_append(tc_cache, tc_octet_array)) {
    RMW_CONNEXT_LOG_ERROR("failed to cache tc_octet_array")
    return nullptr;
  }
  scope_exit_tc_octet_array.cancel();

  if (!RMW_Connext_TypeCodePtrSeq_append(tc_cache, tc_guid)) {
    RMW_CONNEXT_LOG_ERROR("failed to cache tc_guid")
    return nullptr;
  }
  scope_exit_tc_guid.cancel();

  return tc_guid;
}

static DDS_TypeCode *
SequenceNumber_get_typecode(
  DDS_TypeCodeFactory * const tc_factory,
  struct RMW_Connext_TypeCodePtrSeq * const tc_cache)
{
  DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;

  struct DDS_StructMemberSeq tc_sn_members = DDS_SEQUENCE_INITIALIZER;
  if (!DDS_StructMemberSeq_ensure_length(&tc_sn_members, 2, 2)) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to resize DDS_SequenceNumber_t members sequence")
    return nullptr;
  }
  struct DDS_StructMemberSeq * const tc_sn_members_ptr = &tc_sn_members;
  auto scope_exit_tc_sn_members = rcpputils::make_scope_exit(
    [tc_sn_members_ptr]()
    {
      if (!DDS_StructMemberSeq_finalize(tc_sn_members_ptr)) {
        RMW_CONNEXT_LOG_ERROR("failed to finalize DDS_SequenceNumber_t members sequence")
      }
    });

  DDS_StructMember * tc_member =
    DDS_StructMemberSeq_get_reference(&tc_sn_members, 0);
  tc_member->name = DDS_String_dup("high");
  if (nullptr == tc_member->name) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to allocate string")
    return nullptr;
  }
  tc_member->type = &DDS_g_tc_long;

  tc_member =
    DDS_StructMemberSeq_get_reference(&tc_sn_members, 1);
  tc_member->name = DDS_String_dup("low");
  if (nullptr == tc_member->name) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to allocate string")
    return nullptr;
  }
  tc_member->type = &DDS_g_tc_ulong;

  DDS_TypeCode * const tc_sn =
    DDS_TypeCodeFactory_create_struct_tc(
    tc_factory, "DDS_SequenceNumber_t", &tc_sn_members, &ex);
  if (nullptr == tc_sn || DDS_NO_EXCEPTION_CODE != ex) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to create TypeCode for DDS_SequenceNumber_t: %d", ex)
    return nullptr;
  }
  auto scope_exit_tc_sn = rcpputils::make_scope_exit(
    [tc_factory, tc_sn]()
    {
      DDS_ExceptionCode_t fex = DDS_NO_EXCEPTION_CODE;
      DDS_TypeCodeFactory_delete_tc(tc_factory, tc_sn, &fex);
      if (DDS_NO_EXCEPTION_CODE != fex) {
        RMW_CONNEXT_LOG_ERROR_A(
          "failed to delete TypeCode for DDS_SequenceNumber_t: %d", fex)
      }
    });

  if (!RMW_Connext_TypeCodePtrSeq_append(tc_cache, tc_sn)) {
    RMW_CONNEXT_LOG_ERROR("failed to cache tc_sn")
    return nullptr;
  }
  scope_exit_tc_sn.cancel();

  return tc_sn;
}

static DDS_TypeCode *
SampleIdentity_get_typecode(
  DDS_TypeCodeFactory * const tc_factory,
  struct RMW_Connext_TypeCodePtrSeq * const tc_cache)
{
  DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;

  DDS_TypeCode * const tc_guid = GUID_get_typecode(tc_factory, tc_cache);
  if (nullptr == tc_guid) {
    RMW_CONNEXT_LOG_ERROR("failed to get nested typecode")
    return nullptr;
  }
  DDS_TypeCode * const tc_sn = SequenceNumber_get_typecode(tc_factory, tc_cache);
  if (nullptr == tc_sn) {
    RMW_CONNEXT_LOG_ERROR("failed to get nested typecode")
    return nullptr;
  }

  struct DDS_StructMemberSeq tc_sampleid_members = DDS_SEQUENCE_INITIALIZER;
  if (!DDS_StructMemberSeq_ensure_length(&tc_sampleid_members, 2, 2)) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to resize DDS_SampleIdentity_t members sequence")
    return nullptr;
  }
  struct DDS_StructMemberSeq * const tc_sampleid_members_ptr = &tc_sampleid_members;
  auto scope_exit_tc_sampleid_members = rcpputils::make_scope_exit(
    [tc_sampleid_members_ptr]()
    {
      if (!DDS_StructMemberSeq_finalize(tc_sampleid_members_ptr)) {
        RMW_CONNEXT_LOG_ERROR("failed to finalize DDS_SampleIdentity_t members sequence")
      }
    });

  DDS_StructMember * tc_member =
    DDS_StructMemberSeq_get_reference(&tc_sampleid_members, 0);
  tc_member->name = DDS_String_dup("writer_guid");
  if (nullptr == tc_member->name) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to allocate string")
    return nullptr;
  }
  tc_member->type = tc_guid;

  tc_member =
    DDS_StructMemberSeq_get_reference(&tc_sampleid_members, 1);
  tc_member->name = DDS_String_dup("sequence_number");
  if (nullptr == tc_member->name) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to allocate string")
    return nullptr;
  }
  tc_member->type = tc_sn;

  DDS_TypeCode * const tc_sampleid =
    DDS_TypeCodeFactory_create_struct_tc(
    tc_factory, "DDS_SampleIdentity_t", &tc_sampleid_members, &ex);
  if (nullptr == tc_sampleid || DDS_NO_EXCEPTION_CODE != ex) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to create TypeCode for DDS_SampleIdentity_t: %d", ex)
    return nullptr;
  }
  auto scope_exit_tc_sampleid = rcpputils::make_scope_exit(
    [tc_factory, tc_sampleid]()
    {
      DDS_ExceptionCode_t fex = DDS_NO_EXCEPTION_CODE;
      DDS_TypeCodeFactory_delete_tc(tc_factory, tc_sampleid, &fex);
      if (DDS_NO_EXCEPTION_CODE != fex) {
        RMW_CONNEXT_LOG_ERROR_A(
          "failed to delete TypeCode for DDS_SampleIdentity_t: %d", fex)
      }
    });

  if (!RMW_Connext_TypeCodePtrSeq_append(tc_cache, tc_sampleid)) {
    RMW_CONNEXT_LOG_ERROR("failed to cache tc_sampleid")
    return nullptr;
  }
  scope_exit_tc_sampleid.cancel();

  return tc_sampleid;
}

static DDS_TypeCode *
RequestHeader_get_typecode(
  DDS_TypeCodeFactory * const tc_factory,
  struct RMW_Connext_TypeCodePtrSeq * const tc_cache)
{
  DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;

  DDS_TypeCode * const tc_sampleid = SampleIdentity_get_typecode(tc_factory, tc_cache);
  if (nullptr == tc_sampleid) {
    RMW_CONNEXT_LOG_ERROR("failed to get nested typecode")
    return nullptr;
  }

  DDS_TypeCode * const tc_instancename = DDS_TypeCodeFactory_create_string_tc(
    tc_factory, 255, &ex);
  if (nullptr == tc_instancename || DDS_NO_EXCEPTION_CODE != ex) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to create TypeCode for InstanceName: %d", ex)
    return nullptr;
  }
  auto scope_exit_tc_instancename = rcpputils::make_scope_exit(
    [tc_factory, tc_instancename]()
    {
      DDS_ExceptionCode_t fex = DDS_NO_EXCEPTION_CODE;
      DDS_TypeCodeFactory_delete_tc(tc_factory, tc_instancename, &fex);
      if (DDS_NO_EXCEPTION_CODE != fex) {
        RMW_CONNEXT_LOG_ERROR_A(
          "failed to delete TypeCode for InstanceName: %d", fex)
      }
    });

  struct DDS_StructMemberSeq tc_reqheader_members = DDS_SEQUENCE_INITIALIZER;
  if (!DDS_StructMemberSeq_ensure_length(&tc_reqheader_members, 2, 2)) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to resize RequestHeader members sequence")
    return nullptr;
  }
  struct DDS_StructMemberSeq * const tc_reqheader_members_ptr = &tc_reqheader_members;
  auto scope_exit_tc_reqheader_members = rcpputils::make_scope_exit(
    [tc_reqheader_members_ptr]()
    {
      if (!DDS_StructMemberSeq_finalize(tc_reqheader_members_ptr)) {
        RMW_CONNEXT_LOG_ERROR("failed to finalize RequestHeader members sequence")
      }
    });

  DDS_StructMember * tc_member =
    DDS_StructMemberSeq_get_reference(&tc_reqheader_members, 0);
  tc_member->name = DDS_String_dup("requestId");
  if (nullptr == tc_member->name) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to allocate string")
    return nullptr;
  }
  tc_member->type = tc_sampleid;

  tc_member =
    DDS_StructMemberSeq_get_reference(&tc_reqheader_members, 1);
  tc_member->name = DDS_String_dup("instanceName");
  if (nullptr == tc_member->name) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to allocate string")
    return nullptr;
  }
  tc_member->type = tc_instancename;

  DDS_TypeCode * const tc_reqheader =
    DDS_TypeCodeFactory_create_struct_tc(
    tc_factory, "RequestHeader", &tc_reqheader_members, &ex);
  if (nullptr == tc_reqheader || DDS_NO_EXCEPTION_CODE != ex) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to create TypeCode for RequestHeader: %d", ex)
    return nullptr;
  }
  auto scope_exit_tc_reqheader = rcpputils::make_scope_exit(
    [tc_factory, tc_reqheader]()
    {
      DDS_ExceptionCode_t fex = DDS_NO_EXCEPTION_CODE;
      DDS_TypeCodeFactory_delete_tc(tc_factory, tc_reqheader, &fex);
      if (DDS_NO_EXCEPTION_CODE != fex) {
        RMW_CONNEXT_LOG_ERROR_A(
          "failed to delete TypeCode for RequestHeader: %d", fex)
      }
    });

  if (!RMW_Connext_TypeCodePtrSeq_append(tc_cache, tc_instancename)) {
    RMW_CONNEXT_LOG_ERROR("failed to cache tc_instancename")
    return nullptr;
  }
  scope_exit_tc_instancename.cancel();
  if (!RMW_Connext_TypeCodePtrSeq_append(tc_cache, tc_reqheader)) {
    RMW_CONNEXT_LOG_ERROR("failed to cache tc_reqheader")
    return nullptr;
  }
  scope_exit_tc_reqheader.cancel();

  return tc_reqheader;
}

static DDS_TypeCode *
ReplyHeader_get_typecode(
  DDS_TypeCodeFactory * const tc_factory,
  struct RMW_Connext_TypeCodePtrSeq * const tc_cache)
{
  DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;

  DDS_TypeCode * const tc_sampleid = SampleIdentity_get_typecode(tc_factory, tc_cache);
  if (nullptr == tc_sampleid) {
    RMW_CONNEXT_LOG_ERROR("failed to get nested typecode")
    return nullptr;
  }

  struct DDS_StructMemberSeq tc_repheader_members = DDS_SEQUENCE_INITIALIZER;
  if (!DDS_StructMemberSeq_ensure_length(&tc_repheader_members, 2, 2)) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to resize ReplyHeader members sequence")
    return nullptr;
  }
  struct DDS_StructMemberSeq * const tc_repheader_members_ptr = &tc_repheader_members;
  auto scope_exit_tc_repheader_members = rcpputils::make_scope_exit(
    [tc_repheader_members_ptr]()
    {
      if (!DDS_StructMemberSeq_finalize(tc_repheader_members_ptr)) {
        RMW_CONNEXT_LOG_ERROR("failed to finalize ReplyHeader members sequence")
      }
    });

  DDS_StructMember * tc_member =
    DDS_StructMemberSeq_get_reference(&tc_repheader_members, 0);
  tc_member->name = DDS_String_dup("relatedRequest");
  if (nullptr == tc_member->name) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to allocate string")
    return nullptr;
  }
  tc_member->type = tc_sampleid;

  tc_member =
    DDS_StructMemberSeq_get_reference(&tc_repheader_members, 1);
  tc_member->name = DDS_String_dup("remoteEx");
  if (nullptr == tc_member->name) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to allocate string")
    return nullptr;
  }
  tc_member->type = &DDS_g_tc_long;

  DDS_TypeCode * const tc_repheader =
    DDS_TypeCodeFactory_create_struct_tc(
    tc_factory, "ReplyHeader", &tc_repheader_members, &ex);
  if (nullptr == tc_repheader || DDS_NO_EXCEPTION_CODE != ex) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to create TypeCode for ReplyHeader: %d", ex)
    return nullptr;
  }
  auto scope_exit_tc_repheader = rcpputils::make_scope_exit(
    [tc_factory, tc_repheader]()
    {
      DDS_ExceptionCode_t fex = DDS_NO_EXCEPTION_CODE;
      DDS_TypeCodeFactory_delete_tc(tc_factory, tc_repheader, &fex);
      if (DDS_NO_EXCEPTION_CODE != fex) {
        RMW_CONNEXT_LOG_ERROR_A(
          "failed to delete TypeCode for ReplyHeader: %d", fex)
      }
    });

  if (!RMW_Connext_TypeCodePtrSeq_append(tc_cache, tc_repheader)) {
    RMW_CONNEXT_LOG_ERROR("failed to cache tc_repheader")
    return nullptr;
  }
  scope_exit_tc_repheader.cancel();

  return tc_repheader;
}


static DDS_TypeCode *
CycloneRequestHeader_get_typecode(
  DDS_TypeCodeFactory * const tc_factory,
  struct RMW_Connext_TypeCodePtrSeq * const tc_cache)
{
  DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;

  struct DDS_StructMemberSeq tc_cycloneheader_members = DDS_SEQUENCE_INITIALIZER;
  if (!DDS_StructMemberSeq_ensure_length(&tc_cycloneheader_members, 2, 2)) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to resize CycloneRequestHeader members sequence")
    return nullptr;
  }
  struct DDS_StructMemberSeq * const tc_cycloneheader_members_ptr = &tc_cycloneheader_members;
  auto scope_exit_tc_cycloneheader_members = rcpputils::make_scope_exit(
    [tc_cycloneheader_members_ptr]()
    {
      if (!DDS_StructMemberSeq_finalize(tc_cycloneheader_members_ptr)) {
        RMW_CONNEXT_LOG_ERROR("failed to finalize CycloneRequestHeader members sequence")
      }
    });

  const DDS_TypeCode * const tc_longlong =
    DDS_TypeCodeFactory_get_primitive_tc(tc_factory, DDS_TK_LONGLONG);
  if (nullptr == tc_longlong) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to look up longlong typecode")
    return nullptr;
  }
  const DDS_TypeCode * const tc_ulonglong =
    DDS_TypeCodeFactory_get_primitive_tc(tc_factory, DDS_TK_ULONGLONG);
  if (nullptr == tc_ulonglong) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to look up ulonglong typecode")
    return nullptr;
  }

  DDS_StructMember * tc_member =
    DDS_StructMemberSeq_get_reference(&tc_cycloneheader_members, 0);
  tc_member->name = DDS_String_dup("guid");
  if (nullptr == tc_member->name) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to allocate string")
    return nullptr;
  }
  tc_member->type = tc_longlong;

  tc_member =
    DDS_StructMemberSeq_get_reference(&tc_cycloneheader_members, 1);
  tc_member->name = DDS_String_dup("seq");
  if (nullptr == tc_member->name) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to allocate string")
    return nullptr;
  }
  tc_member->type = tc_ulonglong;

  DDS_TypeCode * const tc_cycloneheader =
    DDS_TypeCodeFactory_create_struct_tc(
    tc_factory, "ReplyHeader", &tc_cycloneheader_members, &ex);
  if (nullptr == tc_cycloneheader || DDS_NO_EXCEPTION_CODE != ex) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to create TypeCode for CycloneRequestHeader: %d", ex)
    return nullptr;
  }
  auto scope_exit_tc_cycloneheader = rcpputils::make_scope_exit(
    [tc_factory, tc_cycloneheader]()
    {
      DDS_ExceptionCode_t fex = DDS_NO_EXCEPTION_CODE;
      DDS_TypeCodeFactory_delete_tc(tc_factory, tc_cycloneheader, &fex);
      if (DDS_NO_EXCEPTION_CODE != fex) {
        RMW_CONNEXT_LOG_ERROR_A(
          "failed to delete TypeCode for CycloneRequestHeader: %d", fex)
      }
    });

  if (!RMW_Connext_TypeCodePtrSeq_append(tc_cache, tc_cycloneheader)) {
    RMW_CONNEXT_LOG_ERROR("failed to cache tc_cycloneheader")
    return nullptr;
  }
  scope_exit_tc_cycloneheader.cancel();

  return tc_cycloneheader;
}


static
DDS_TCKind
rmw_connextdds_type_id_ros_to_dds(const uint8_t ros_type_id)
{
  switch (ros_type_id) {
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_BOOL:
      {
        return DDS_TK_BOOLEAN;
      }
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_BYTE:
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT8:
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT8:
      {
        return DDS_TK_OCTET;
      }
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_CHAR:
      {
        return DDS_TK_CHAR;
      }
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT32:
      {
        return DDS_TK_FLOAT;
      }
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT64:
      {
        return DDS_TK_DOUBLE;
      }
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT16:
      {
        return DDS_TK_SHORT;
      }
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT16:
      {
        return DDS_TK_USHORT;
      }
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT32:
      {
        return DDS_TK_LONG;
      }
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT32:
      {
        return DDS_TK_ULONG;
      }
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT64:
      {
        return DDS_TK_LONGLONG;
      }
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT64:
      {
        return DDS_TK_ULONGLONG;
      }
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING:
      {
        return DDS_TK_STRING;
      }
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_WSTRING:
      {
        return DDS_TK_WSTRING;
      }
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE:
      {
        return DDS_TK_STRUCT;
      }
    default:
      {
        RMW_CONNEXT_LOG_ERROR_A_SET("unknown ROS type id: %d", ros_type_id)
        return DDS_TK_NULL;
      }
  }
}

#define length_unbound          RTIXCdrLong_MAX

template<typename MemberType>
DDS_TypeCode *
rmw_connextdds_convert_type_member(
  RMW_Connext_MessageTypeSupport * const type_support,
  DDS_TypeCodeFactory * const tc_factory,
  const MemberType * const member,
  struct RMW_Connext_TypeCodePtrSeq * const tc_cache)
{
  DDS_TypeCode * el_tc = nullptr;

  switch (member->type_id_) {
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_BOOL:
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_BYTE:
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT8:
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_CHAR:
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT8:
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT32:
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT64:
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT16:
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT16:
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT32:
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT32:
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT64:
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT64:
      {
        DDS_TCKind dds_type_id =
          rmw_connextdds_type_id_ros_to_dds(member->type_id_);

        el_tc =
          /* TODO(asorbini): refactor out this cast */
          const_cast<DDS_TypeCode *>(
          DDS_TypeCodeFactory_get_primitive_tc(tc_factory, dds_type_id));
        break;
      }
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING:
      {
        DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;
        DDS_TypeCode * const el_tc_new =
          DDS_TypeCodeFactory_create_string_tc(
          tc_factory,
          (member->string_upper_bound_ > 0) ?
          // TODO(asorbini) checked conversion of member->string_upper_bound_
          static_cast<DDS_UnsignedLong>(member->string_upper_bound_) : length_unbound,
          &ex);

        el_tc = RMW_Connext_TypeCodePtrSeq_assert_from_ros(
          type_support,
          tc_cache,
          nullptr /* members */,
          nullptr /* name */,
          el_tc_new);
        if (nullptr == el_tc) {
          return nullptr;
        }
        break;
      }
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_WSTRING:
      {
        DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;
        DDS_TypeCode * const el_tc_new =
          DDS_TypeCodeFactory_create_wstring_tc(
          tc_factory,
          (member->string_upper_bound_ > 0) ?
          // TODO(asorbini) checked conversion of member->string_upper_bound_
          static_cast<DDS_UnsignedLong>(member->string_upper_bound_) : length_unbound,
          &ex);

        el_tc = RMW_Connext_TypeCodePtrSeq_assert_from_ros(
          type_support,
          tc_cache,
          nullptr /* members */,
          nullptr /* name */,
          el_tc_new);
        if (nullptr == el_tc) {
          return nullptr;
        }
        break;
      }
    case ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE:
      {
        bool cpp_version = false;
        const rosidl_message_type_support_t * const type_support_intro =
          RMW_Connext_MessageTypeSupport::get_type_support_intro(
          member->members_, cpp_version);
        if (nullptr == type_support_intro) {
          RMW_CONNEXT_LOG_ERROR_A_SET(
            "introspection type support not found for member: %s",
            member->name_)
          return nullptr;
        }

        std::string type_name;

        if (cpp_version) {
          type_name = rmw_connextdds_create_type_name(
            reinterpret_cast<const rosidl_typesupport_introspection_cpp::MessageMembers *>(
              type_support_intro->data), true /* mangle_names */);
        } else {
          type_name = rmw_connextdds_create_type_name(
            reinterpret_cast<const rosidl_typesupport_introspection_c__MessageMembers *>(
              type_support_intro->data), true /* mangle_names */);
        }

        el_tc = RMW_Connext_TypeCodePtrSeq_assert_from_ros(
          type_support, tc_cache, member->members_, type_name.c_str());
        break;
      }
    default:
      {
        RMW_CONNEXT_LOG_ERROR_A("unknown ROS type id: %d", member->type_id_)
        el_tc = nullptr;
      }
  }

  if (nullptr == el_tc) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to convert member to type code: %s",
      member->name_)
    return nullptr;
  }

  DDS_TypeCode * tc = el_tc;

  if (member->is_array_) {
    DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;
    if (member->array_size_ > 0 && !member->is_upper_bound_) {
      struct DDS_UnsignedLongSeq dimensions = DDS_SEQUENCE_INITIALIZER;
      if (!DDS_UnsignedLongSeq_ensure_length(&dimensions, 1, 1)) {
        return nullptr;
      }
      // TODO(asorbini) checked conversion of member->array_size_
      *DDS_UnsignedLongSeq_get_reference(&dimensions, 0) =
        static_cast<DDS_UnsignedLong>(member->array_size_);
      DDS_TypeCode * const tc_array =
        DDS_TypeCodeFactory_create_array_tc(
        tc_factory, &dimensions, el_tc, &ex);
      DDS_UnsignedLongSeq_finalize(&dimensions);

      tc = RMW_Connext_TypeCodePtrSeq_assert_from_ros(
        type_support,
        tc_cache,
        nullptr /* members */,
        nullptr /* name */,
        tc_array);
      if (nullptr == tc) {
        return nullptr;
      }
    } else {
      DDS_Long tc_seq_len = length_unbound;
      if (member->is_upper_bound_) {
        tc_seq_len = static_cast<DDS_Long>(member->array_size_);
        if (member->array_size_ > static_cast<size_t>(INT32_MAX)) {
          RMW_CONNEXT_LOG_WARNING_A(
            "array member's length trucated to INT32_MAX (%d): %lu",
            INT32_MAX, member->array_size_)
          tc_seq_len = INT32_MAX;
        }
      }
      DDS_TypeCode * const tc_seq =
        DDS_TypeCodeFactory_create_sequence_tc(
        tc_factory, tc_seq_len, el_tc, &ex);

      tc = RMW_Connext_TypeCodePtrSeq_assert_from_ros(
        type_support,
        tc_cache,
        nullptr /* members */,
        nullptr /* name */,
        tc_seq);
      if (nullptr == tc) {
        return nullptr;
      }
    }
  }

  return tc;
}

template<typename MembersType>
rmw_ret_t
rmw_connextdds_convert_type_members(
  RMW_Connext_MessageTypeSupport * const type_support,
  DDS_TypeCodeFactory * const tc_factory,
  const MembersType * const members,
  struct DDS_StructMemberSeq * const tc_members,
  struct RMW_Connext_TypeCodePtrSeq * const tc_cache)
{
  DDS_TypeCode * tc_header = nullptr;

  if (type_support->type_requestreply()) {
    // TODO(asorbini) cache condition to a variable to avoid an "unformattable"
    // long line on the `else()` (see https://github.com/ament/ament_lint/issues/158)
    const bool basic_mapping =
      RMW_Connext_RequestReplyMapping::Basic == type_support->ctx()->request_reply_mapping;

    if (type_support->ctx()->cyclone_compatible) {
      tc_header = CycloneRequestHeader_get_typecode(tc_factory, tc_cache);
      if (nullptr == tc_header) {
        RMW_CONNEXT_LOG_ERROR("failed to get CycloneRequestHeader typecode")
        return RMW_RET_ERROR;
      }
    } else if (basic_mapping) {
      switch (type_support->message_type()) {
        case RMW_CONNEXT_MESSAGE_REQUEST:
          {
            tc_header = RequestHeader_get_typecode(tc_factory, tc_cache);
            if (nullptr == tc_header) {
              RMW_CONNEXT_LOG_ERROR("failed to get RequestHeader typecode")
              return RMW_RET_ERROR;
            }
            break;
          }
        case RMW_CONNEXT_MESSAGE_REPLY:
          {
            tc_header = ReplyHeader_get_typecode(tc_factory, tc_cache);
            if (nullptr == tc_header) {
              RMW_CONNEXT_LOG_ERROR("failed to get ReplyHeader typecode")
              return RMW_RET_ERROR;
            }
            break;
          }
        default:
          {
            RMW_CONNEXT_LOG_ERROR_A_SET(
              "invalid message type for typecode: %d",
              type_support->message_type())
            return RMW_RET_ERROR;
          }
      }
    }
  }

  RMW_CONNEXT_ASSERT(members->member_count_ >= 0)
  const DDS_Long member_count =
    static_cast<DDS_Long>(members->member_count_) + ((nullptr != tc_header) ? 1 : 0);
  const uint32_t member_i_start = (nullptr != tc_header) ? 1 : 0;

  if (!DDS_StructMemberSeq_ensure_length(tc_members, member_count, member_count)) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to resize tc_members sequence")
    return RMW_RET_ERROR;
  }

  if (nullptr != tc_header) {
    DDS_StructMember * const tc_member =
      DDS_StructMemberSeq_get_reference(tc_members, 0);
    tc_member->name = DDS_String_dup("_header");
    if (nullptr == tc_member->name) {
      return RMW_RET_BAD_ALLOC;
    }
    tc_member->type = tc_header;
  }

  for (uint32_t i = 0, j = member_i_start; i < members->member_count_; ++i, ++j) {
    DDS_StructMember * const tc_member =
      DDS_StructMemberSeq_get_reference(tc_members, j);
    const auto * member = members->members_ + i;

    /* Check that member has a non-empty name */
    size_t member_name_len = strlen(member->name_);
    if (nullptr == member->name_ || member_name_len == 0 ||
      (member_name_len == 1 &&
      member->name_[member_name_len - 1] == '_'))
    {
      return RMW_RET_ERROR;
    }

    /* Names in the introspection plugin don't actually end with "_" */
#if RMW_CONNEXT_LEGACY_RMW_COMPATIBILITY_MODE
    if (type_support->ctx()->legacy_rmw_compatible) {
      const DDS_UnsignedLong member_name_len_u =
        static_cast<DDS_UnsignedLong>(member_name_len) + 1;
      tc_member->name = DDS_String_alloc(member_name_len_u);
      if (nullptr == tc_member->name) {
        return RMW_RET_BAD_ALLOC;
      }
      const int rc_written =
        snprintf(tc_member->name, member_name_len_u + 1, "%s_", member->name_);
      if (rc_written < 0 ||
        static_cast<DDS_UnsignedLong>(rc_written) != member_name_len_u)
      {
        return RMW_RET_ERROR;
      }
    } else {
#endif /* RMW_CONNEXT_LEGACY_RMW_COMPATIBILITY_MODE */
    tc_member->name = DDS_String_dup(member->name_);
    if (nullptr == tc_member->name) {
      return RMW_RET_BAD_ALLOC;
    }
#if RMW_CONNEXT_LEGACY_RMW_COMPATIBILITY_MODE
  }
#endif /* RMW_CONNEXT_LEGACY_RMW_COMPATIBILITY_MODE */

    tc_member->type =
      rmw_connextdds_convert_type_member(type_support, tc_factory, member, tc_cache);
    if (nullptr == tc_member->type) {
      return RMW_RET_ERROR;
    }
  }

  return RMW_RET_OK;
}


DDS_TypeCode *
rmw_connextdds_create_typecode(
  RMW_Connext_MessageTypeSupport * const type_support,
  const rosidl_message_type_support_t * const type_supports,
  const char * const type_name,
  const void * const intro_members_in,
  const bool intro_members_cpp,
  RMW_Connext_TypeCodePtrSeq * const tc_cache)
{
  bool cpp_version = intro_members_cpp;
  const rosidl_message_type_support_t * intro_ts = nullptr;
  const void * intro_members = intro_members_in;
  struct RMW_Connext_TypeCodePtrSeq tc_cache_new = DDS_SEQUENCE_INITIALIZER;
  struct RMW_Connext_TypeCodePtrSeq * const tc_cache_new_ptr = &tc_cache_new,
    * tc_cache_ptr = tc_cache;
  if (nullptr == tc_cache_ptr) {
    tc_cache_ptr = tc_cache_new_ptr;
  }
  auto scope_exit_tc_cache_delete =
    rcpputils::make_scope_exit(
    [tc_cache_new_ptr]()
    {
      RMW_Connext_TypeCodePtrSeq_finalize_elements(tc_cache_new_ptr);
      RMW_Connext_TypeCodePtrSeq_finalize(tc_cache_new_ptr);
    });

  if (nullptr == intro_members_in) {
    intro_ts =
      RMW_Connext_MessageTypeSupport::get_type_support_intro(
      type_supports, cpp_version);

    if (nullptr == intro_ts) {
      RMW_CONNEXT_LOG_ERROR_A_SET(
        "introspection type support not found for %s",
        type_name)
      return nullptr;
    }
    intro_members = intro_ts->data;
  }

  DDS_TypeCodeFactory * const tc_factory = DDS_TypeCodeFactory_get_instance();
  if (nullptr == tc_factory) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to get DDS_TypeCodeFactory")
    return nullptr;
  }

  struct DDS_StructMemberSeq tc_members = DDS_SEQUENCE_INITIALIZER;
  struct DDS_StructMemberSeq * const tc_members_ptr = &tc_members;
  auto scope_exit_tc_members_delete =
    rcpputils::make_scope_exit(
    [tc_members_ptr]()
    {
      const DDS_Long seq_len =
      DDS_StructMemberSeq_get_length(tc_members_ptr);
      for (DDS_Long i = 0; i < seq_len; i++) {
        DDS_StructMember * const tc_member =
        DDS_StructMemberSeq_get_reference(tc_members_ptr, i);
        DDS_String_free(tc_member->name);
        tc_member->name = nullptr;
      }

      DDS_StructMemberSeq_finalize(tc_members_ptr);
    });

  if (cpp_version) {
    const rosidl_typesupport_introspection_cpp::MessageMembers * const members =
      reinterpret_cast<const rosidl_typesupport_introspection_cpp::MessageMembers *>(
      intro_members);

    if (RMW_RET_OK !=
      rmw_connextdds_convert_type_members(
        type_support, tc_factory, members, &tc_members, tc_cache_ptr))
    {
      RMW_CONNEXT_LOG_ERROR_A_SET(
        "failed to convert members for %s",
        type_name)
      return nullptr;
    }
  } else {
    const rosidl_typesupport_introspection_c__MessageMembers * const members =
      reinterpret_cast<const rosidl_typesupport_introspection_c__MessageMembers *>(
      intro_members);

    if (RMW_RET_OK !=
      rmw_connextdds_convert_type_members(
        type_support, tc_factory, members, &tc_members, tc_cache_ptr))
    {
      RMW_CONNEXT_LOG_ERROR_A_SET(
        "failed to convert members for %s",
        type_name)
      return nullptr;
    }
  }

  DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;
  DDS_TypeCode * tc =
    DDS_TypeCodeFactory_create_struct_tc(
    tc_factory, type_name, &tc_members, &ex);
  if (nullptr == tc) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to create type code for %s",
      type_name)
    return nullptr;
  }

  return tc;
}

void
rmw_connextdds_delete_typecode(DDS_TypeCode * const tc)
{
  DDS_TypeCodeFactory * const tc_factory = DDS_TypeCodeFactory_get_instance();
  if (nullptr == tc_factory) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to get DDS_TypeCodeFactory")
    return;
  }
  DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;
  DDS_TypeCodeFactory_delete_tc(tc_factory, tc, &ex);
  if (DDS_NO_EXCEPTION_CODE != ex) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to delete type code")
  }
}

void
rmw_connextdds_release_typecode_cache(
  RMW_Connext_TypeCodePtrSeq * const tc_cache)
{
  RMW_Connext_TypeCodePtrSeq_finalize_elements(tc_cache);
  RMW_Connext_TypeCodePtrSeq_finalize(tc_cache);
}
