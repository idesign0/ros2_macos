// Copyright 2021 Ericsson AB
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

rmw_ret_t
rmw_api_connextdds_publisher_get_network_flow_endpoints(
  const rmw_publisher_t * publisher,
  rcutils_allocator_t * allocator,
  rmw_network_flow_endpoint_array_t * network_flow_endpoint_array)
{
  UNUSED_ARG(publisher);
  UNUSED_ARG(allocator);
  UNUSED_ARG(network_flow_endpoint_array);
  RMW_CONNEXT_LOG_NOT_IMPLEMENTED
  return RMW_RET_UNSUPPORTED;
}

rmw_ret_t
rmw_api_connextdds_subscription_get_network_flow_endpoints(
  const rmw_subscription_t * subscription,
  rcutils_allocator_t * allocator,
  rmw_network_flow_endpoint_array_t * network_flow_endpoint_array)
{
  UNUSED_ARG(subscription);
  UNUSED_ARG(allocator);
  UNUSED_ARG(network_flow_endpoint_array);
  RMW_CONNEXT_LOG_NOT_IMPLEMENTED
  return RMW_RET_UNSUPPORTED;
}
