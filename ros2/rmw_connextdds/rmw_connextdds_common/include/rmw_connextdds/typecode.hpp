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

#ifndef RMW_CONNEXTDDS__TYPECODE_HPP_
#define RMW_CONNEXTDDS__TYPECODE_HPP_

#include "rmw_connextdds/type_support.hpp"

DDS_SEQUENCE(RMW_Connext_TypeCodePtrSeq, DDS_TypeCode *);

DDS_TypeCode *
rmw_connextdds_create_typecode(
  RMW_Connext_MessageTypeSupport * const type_support,
  const rosidl_message_type_support_t * const type_supports,
  const char * const type_name,
  const void * const intro_members = nullptr,
  const bool intro_members_cpp = false,
  RMW_Connext_TypeCodePtrSeq * const tc_cache = nullptr);

void
rmw_connextdds_delete_typecode(DDS_TypeCode * const tc);

void
rmw_connextdds_release_typecode_cache(
  RMW_Connext_TypeCodePtrSeq * const tc_cache);

#endif  // RMW_CONNEXTDDS__TYPECODE_HPP_
