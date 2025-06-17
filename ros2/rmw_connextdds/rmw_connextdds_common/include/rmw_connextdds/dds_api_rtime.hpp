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

#ifndef RMW_CONNEXTDDS__DDS_API_RTIME_HPP_
#define RMW_CONNEXTDDS__DDS_API_RTIME_HPP_

#include "rti_me_c.h"  // NOLINT(build/include_subdir)
#include "disc_dpde/disc_dpde_discovery_plugin.h"
#include "wh_sm/wh_sm_history.h"
#include "rh_sm/rh_sm_history.h"
#include "netio/netio_udp.h"
#include "netio_shmem/netio_shmem.h"
#if RMW_CONNEXT_ENABLE_SECURITY
#include "sec_core/sec_core_c.h"
#endif /* RMW_CONNEXT_ENABLE_SECURITY */
#include "rmw_connextdds/rtime_ext.h"
#include "REDASequence.h"

typedef DDS_UntypedSampleSeq RMW_Connext_UntypedSampleSeq;

#define RMW_Connext_UntypedSampleSeq_INITIALIZER    DDS_SEQUENCE_INITIALIZER

#define DDS_PropertyQosPolicyHelper_assert_property(p_, n_, v_, pr_) \
  (DDS_PropertySeq_assert_property(&(p_)->value, (n_), (v_), (pr_)) ? \
  DDS_RETCODE_OK : DDS_RETCODE_ERROR)

#define DDS_DomainParticipantFactory_get_qos_profiles(dpf, profiles, library_name) DDS_RETCODE_OK

#define DDS_DomainParticipantFactory_get_qos_profile_libraries(dpf, libraries) DDS_RETCODE_OK

#define DDS_Publisher_get_default_datawriter_qos_w_topic_name(pub_, dw_qos_, topic_) \
  DDS_Publisher_get_default_datawriter_qos(pub_, dw_qos_)

#define DDS_Subscriber_get_default_datareader_qos_w_topic_name(sub_, dr_qos_, topic_) \
  DDS_Subscriber_get_default_datareader_qos(sub_, dr_qos_)

// Forward declaration for DDS_LifespanQosPolicy, since it is not supported by Micro.
struct DDS_LifespanQosPolicy;

// These function are needed by code implementing the "extended" request/reply
// mapping, but they are not available in Micro. They are defined as no-op's,
// since the "extended" mapping cannot be used with Micro.
#define DDS_SampleInfo_get_sample_identity(info_, identity_) \
  { \
    UNUSED_ARG((info_)); \
    UNUSED_ARG((identity_)); \
  }
#define DDS_SampleInfo_get_related_sample_identity(info_, identity_) \
  { \
    UNUSED_ARG((info_)); \
    UNUSED_ARG((identity_)); \
  }

// Define a common initializer for DDS_SampleIdentity_t, which initializes
// the guid with 0's.
#define DDS_SampleIdentity_UNKNOWN      DDS_SAMPLE_IDENTITY_UNKNOWN

// Convenience function to compare the first 12 bytes of the handle
#define DDS_InstanceHandle_compare_prefix(ih_a_, ih_b_) \
  memcmp((ih_a_)->octet, (ih_b_)->octet, 12)

// Not available function
#define DDS_DataWriter_wait_for_acknowledgments(writer_, timeout_)    DDS_RETCODE_UNSUPPORTED

#endif  // RMW_CONNEXTDDS__DDS_API_RTIME_HPP_
