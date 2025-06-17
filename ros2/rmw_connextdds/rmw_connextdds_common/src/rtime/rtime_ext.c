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

#include "dds_c/dds_c_config.h"
#ifndef netio_address_h
#include "netio/netio_address.h"
#endif
#ifndef netio_route_h
#include "netio/netio_route.h"
#endif
#ifndef netio_loopback_h
#include "netio/netio_loopback.h"
#endif
#ifndef netio_udp_h
#include "netio/netio_udp.h"
#endif
#ifndef netio_rtps_h
#include "netio/netio_rtps.h"
#endif
#ifndef reda_sequence_h
#include "reda/reda_sequence.h"
#endif
#ifndef reda_string_h
#include "reda/reda_string.h"
#endif
#ifndef reda_bufferpool_h
#include "reda/reda_bufferpool.h"
#endif
#ifndef dds_c_common_impl_h
#include "dds_c/dds_c_common_impl.h"
#endif
#ifndef dds_c_discovery_plugin_h
#include "dds_c/dds_c_discovery_plugin.h"
#endif
#ifndef dds_c_string_manager_h
#include "dds_c/dds_c_string_manager.h"
#endif

#include "DomainParticipant.h"

#include "rmw_connextdds/rtime_ext.h"

DDS_ReturnCode_t
DDS_DomainParticipant_lookup_type_pluginI(
  DDS_DomainParticipant * const participant,
  const char * const type_name,
  struct DDS_TypePluginI ** const plugin_out)
{
  struct DDS_DomainParticipantImpl * self =
    (struct DDS_DomainParticipantImpl *)participant;
  DDS_ReturnCode_t retcode = DDS_RETCODE_ERROR;
  DB_ReturnCode_T dbrc = DB_RETCODE_ERROR;
  RTI_SIZE_T type_name_len = 0;
  struct DDS_TypeImpl * a_type = NULL;

  OSAPI_PRECONDITION_ALWAYS(
    (self == NULL) || (type_name == NULL) || (plugin_out == NULL),
    return DDS_RETCODE_BAD_PARAMETER,
    OSAPI_Log_entry_add_pointer("self", self, RTI_FALSE);
    OSAPI_Log_entry_add_pointer("type_name", type_name, RTI_FALSE);
    OSAPI_Log_entry_add_pointer("plugin_out", plugin_out, RTI_TRUE);)

  type_name_len = DDS_String_length(type_name);

  OSAPI_PRECONDITION_ALWAYS(
    type_name_len > RTPS_PATHNAME_LEN_MAX,
    return DDS_RETCODE_BAD_PARAMETER,
    OSAPI_Log_entry_add_uint("len(type_name)", type_name_len, RTI_TRUE);)

  * plugin_out = NULL;

  if (DB_RETCODE_OK != DB_Database_lock(self->database)) {
    return DDS_RETCODE_ERROR;
  }

  dbrc = DB_Table_select_match(
    self->type_table,
    DB_TABLE_DEFAULT_INDEX,
    (DB_Record_T *)&a_type,
    (DB_Key_T)type_name);

  if (dbrc == DB_RETCODE_OK) {
    *plugin_out = a_type->plugin;
  } else if (dbrc != DB_RETCODE_NO_DATA) {
    goto done;
  }

  retcode = DDS_RETCODE_OK;

done:
  if (DB_RETCODE_OK != DB_Database_unlock(self->database)) {
    retcode = DDS_RETCODE_ERROR;
  }

  return retcode;
}


DDS_ReturnCode_t
DDS_DomainParticipant_is_type_in_use(
  DDS_DomainParticipant * const participant,
  const char * const type_name,
  DDS_Boolean * const in_use_out)
{
  struct DDS_DomainParticipantImpl * self =
    (struct DDS_DomainParticipantImpl *)participant;
  DDS_ReturnCode_t retcode = DDS_RETCODE_ERROR;
  DB_ReturnCode_T dbrc = DB_RETCODE_ERROR;
  RTI_SIZE_T type_name_len = 0;
  struct DDS_TypeImpl * a_type = NULL;

  OSAPI_PRECONDITION_ALWAYS(
    (self == NULL) || (type_name == NULL) || (in_use_out == NULL),
    return DDS_RETCODE_BAD_PARAMETER,
    OSAPI_Log_entry_add_pointer("self", self, RTI_FALSE);
    OSAPI_Log_entry_add_pointer("type_name", type_name, RTI_FALSE);
    OSAPI_Log_entry_add_pointer("in_use_out", in_use_out, RTI_TRUE);)

  type_name_len = DDS_String_length(type_name);

  OSAPI_PRECONDITION_ALWAYS(
    type_name_len > RTPS_PATHNAME_LEN_MAX,
    return DDS_RETCODE_BAD_PARAMETER,
    OSAPI_Log_entry_add_uint("len(type_name)", type_name_len, RTI_TRUE);)

  * in_use_out = DDS_BOOLEAN_FALSE;

  if (DB_RETCODE_OK != DB_Database_lock(self->database)) {
    return DDS_RETCODE_ERROR;
  }

  dbrc = DB_Table_select_match(
    self->type_table,
    DB_TABLE_DEFAULT_INDEX,
    (DB_Record_T *)&a_type,
    (DB_Key_T)type_name);

  /* This function expects to be called for a registered type,
     if no type was found, consider it an error */
  if (dbrc != DB_RETCODE_OK) {
    goto done;
  }

  *in_use_out = (a_type->topic_count > 0) ?
    DDS_BOOLEAN_TRUE : DDS_BOOLEAN_FALSE;

  retcode = DDS_RETCODE_OK;

done:
  if (DB_RETCODE_OK != DB_Database_unlock(self->database)) {
    retcode = DDS_RETCODE_ERROR;
  }

  return retcode;
}
