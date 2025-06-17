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

#ifndef RMW_CONNEXTDDS__DDS_API_NDDS_HPP_
#define RMW_CONNEXTDDS__DDS_API_NDDS_HPP_

#include "ndds/ndds_c.h"

#include "rcutils/types.h"

#if RMW_CONNEXT_DDS_API_PRO_LEGACY
#ifndef RTIXCdrLong_MAX
#define RTIXCdrLong_MAX 2147483647
#endif /* RTIXCdrLong_MAX */
#endif /* RMW_CONNEXT_DDS_API_PRO_LEGACY */

// Define a common initializer for DDS_SampleIdentity_t, which initializes
// the guid with 0's.
#define DDS_SampleIdentity_UNKNOWN      DDS_SAMPLEIDENTITY_DEFAULT

// Convenience function to compare the first 12 bytes of the handle
#define DDS_InstanceHandle_compare_prefix(ih_a_, ih_b_) \
  memcmp((ih_a_)->keyHash.value, (ih_b_)->keyHash.value, 12)

#endif  // RMW_CONNEXTDDS__DDS_API_NDDS_HPP_
