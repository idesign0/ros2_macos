// Copyright 2021 Real-Time Innovations, Inc. (RTI)
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
#ifndef RMW_CONNEXTDDS__CUSTOM_SQL_FILTER_HPP_
#define RMW_CONNEXTDDS__CUSTOM_SQL_FILTER_HPP_

#include "rmw_connextdds/dds_api.hpp"

#if RMW_CONNEXT_DDS_API == RMW_CONNEXT_DDS_API_PRO

namespace rti_connext_dds_custom_sql_filter
{

struct CustomSqlFilterData
{
  DDS_SqlFilterGeneratorQos base;

  CustomSqlFilterData();

  DDS_ReturnCode_t
  set_memory_management_property(
    const DDS_DomainParticipantQos & dp_qos);
};

RMW_CONNEXTDDS_PUBLIC
DDS_ReturnCode_t
register_content_filter(
  DDS_DomainParticipant * const participant,
  CustomSqlFilterData * const filter_data);

RMW_CONNEXTDDS_PUBLIC
extern const char * const PLUGIN_NAME;

}  // namespace rti_connext_dds_custom_sql_filter

#if !RMW_CONNEXT_BUILTIN_CFT_COMPATIBILITY_MODE
extern "C" {
// This is an internal function from RTI Connext DDS which allows a filter to
// be registered as "built-in". We need this because we want this custom filter
// to be a replacement for the built-in SQL-like filter.
RMW_CONNEXTDDS_PUBLIC
DDS_ReturnCode_t
DDS_ContentFilter_register_filter(
  DDS_DomainParticipant * participant,
  const char * name,
  const struct DDS_ContentFilter * filter,
  const DDS_ContentFilterEvaluateFunction evaluateOnSerialized,
  const DDS_ContentFilterWriterEvaluateFunction writerEvaluateOnSerialized,
  const DDS_ContentFilterQueryFunction query,
  DDS_Boolean isBuiltin);
}
#endif  // RMW_CONNEXT_BUILTIN_CFT_COMPATIBILITY_MODE

#endif  // RMW_CONNEXT_DDS_API == RMW_CONNEXT_DDS_API_PRO

#endif  // RMW_CONNEXTDDS__CUSTOM_SQL_FILTER_HPP_
