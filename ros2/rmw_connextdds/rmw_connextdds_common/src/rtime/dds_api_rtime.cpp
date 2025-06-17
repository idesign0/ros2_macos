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

#include <mutex>
#include <vector>
#include <algorithm>

#include "rmw_connextdds/type_support.hpp"
#include "rmw_connextdds/rmw_impl.hpp"
#include "rmw_connextdds/graph_cache.hpp"

#include "rcutils/env.h"

struct RMW_Connext_BuiltinListener;

static const DDS_ParticipantBuiltinTopicData DEFAULT_PARTICIPANT_DATA =
  DDS_ParticipantBuiltinTopicData_INITIALIZER;
static const DDS_SubscriptionBuiltinTopicData DEFAULT_SUBSCRIPTION_DATA =
  DDS_SubscriptionBuiltinTopicData_INITIALIZER;
static const DDS_PublicationBuiltinTopicData DEFAULT_PUBLICATION_DATA =
  DDS_PublicationBuiltinTopicData_INITIALIZER;

template<typename T>
struct RMW_Connext_CachedBuiltinData
{
  T data;
  bool valid_data;
  DDS_InstanceStateKind instance_state;
  DDS_InstanceHandle_t instance_handle;
};

struct RMW_Connext_ParticipantData
{
  static
  rmw_ret_t
  copy(
    DDS_ParticipantBuiltinTopicData * const dst,
    const DDS_ParticipantBuiltinTopicData * const src)
  {
    if (!DDS_ParticipantBuiltinTopicData_copy(dst, src)) {
      return RMW_RET_ERROR;
    }
    return RMW_RET_OK;
  }

  static
  void
  initialize(DDS_ParticipantBuiltinTopicData * const data)
  {
    /* *INDENT-OFF* */
    * data = DEFAULT_PARTICIPANT_DATA;
    /* *INDENT-ON* */
  }

  static
  void
  finalize(DDS_ParticipantBuiltinTopicData * const data)
  {
    DDS_ParticipantBuiltinTopicData_finalize(data);
  }

  static
  const char *
  topic_name()
  {
    return DDS_PARTICIPANT_BUILTIN_TOPIC_NAME;
  }

  static
  void
  process_data_available(
    RMW_Connext_BuiltinListener * const listener,
    DDS_DataReader * const reader);
};

struct RMW_Connext_SubscriptionData
{
  static
  rmw_ret_t
  copy(
    DDS_SubscriptionBuiltinTopicData * const dst,
    const DDS_SubscriptionBuiltinTopicData * const src)
  {
    if (!DDS_SubscriptionBuiltinTopicData_copy(dst, src)) {
      return RMW_RET_ERROR;
    }
    return RMW_RET_OK;
  }

  static
  void
  initialize(DDS_SubscriptionBuiltinTopicData * const data)
  {
    /* *INDENT-OFF* */
    * data = DEFAULT_SUBSCRIPTION_DATA;
    /* *INDENT-ON* */
  }

  static
  void
  finalize(DDS_SubscriptionBuiltinTopicData * const data)
  {
    DDS_SubscriptionBuiltinTopicData_finalize(data);
  }

  static
  const char *
  topic_name()
  {
    return DDS_PUBLICATION_BUILTIN_TOPIC_NAME;
  }

  static
  void
  process_data_available(
    RMW_Connext_BuiltinListener * const listener,
    DDS_DataReader * const reader);
};

struct RMW_Connext_PublicationData
{
  static
  rmw_ret_t
  copy(
    DDS_PublicationBuiltinTopicData * const dst,
    const DDS_PublicationBuiltinTopicData * const src)
  {
    if (!DDS_PublicationBuiltinTopicData_copy(dst, src)) {
      return RMW_RET_ERROR;
    }
    return RMW_RET_OK;
  }

  static
  void
  initialize(DDS_PublicationBuiltinTopicData * const data)
  {
    /* *INDENT-OFF* */
    * data = DEFAULT_PUBLICATION_DATA;
    /* *INDENT-ON* */
  }

  static
  void
  finalize(DDS_PublicationBuiltinTopicData * const data)
  {
    DDS_PublicationBuiltinTopicData_finalize(data);
  }

  static
  const char *
  topic_name()
  {
    return DDS_SUBSCRIPTION_BUILTIN_TOPIC_NAME;
  }

  static
  void
  process_data_available(
    RMW_Connext_BuiltinListener * const listener,
    DDS_DataReader * const reader);
};

class RMW_Connext_BuiltinListener
{
public:
  explicit RMW_Connext_BuiltinListener(rmw_context_impl_t * const ctx)
  : ctx(ctx) {}

  virtual ~RMW_Connext_BuiltinListener() {}

  static
  void
  initialize_listener(
    RMW_Connext_BuiltinListener * const b_listener,
    struct DDS_DataReaderListener * const listener);

  virtual
  void
  received_sample(const void * const data, const DDS_SampleInfo * const info) = 0;

  void
  set_parent_listener(const DDS_DataReaderListener & parent_listener)
  {
    this->parent_listener = parent_listener;
  }

protected:
  virtual
  void
  process_data_available(DDS_DataReader * const reader) = 0;

private:
  rmw_context_impl_t * ctx;
  DDS_DataReaderListener parent_listener;

  static
  void
  on_requested_deadline_missed(
    void * listener_data,
    DDS_DataReader * reader,
    const struct DDS_RequestedDeadlineMissedStatus * status);

  static
  void
  on_liveliness_changed(
    void * listener_data,
    DDS_DataReader * reader,
    const struct DDS_LivelinessChangedStatus * status);

  static
  void
  on_requested_incompatible_qos(
    void * listener_data,
    DDS_DataReader * reader,
    const struct DDS_RequestedIncompatibleQosStatus * status);

  static
  void
  on_sample_rejected(
    void * listener_data,
    DDS_DataReader * reader,
    const struct DDS_SampleRejectedStatus * status);

  static
  DDS_Boolean
  on_before_sample_commit(
    void * listener_data,
    DDS_DataReader * reader,
    const void * const sample,
    const struct DDS_SampleInfo * const sample_info,
    DDS_Boolean * dropped);

  static
  void
  on_data_available(
    void * listener_data,
    DDS_DataReader * reader);

  static void
  on_subscription_matched(
    void * listener_data,
    DDS_DataReader * reader,
    const struct DDS_SubscriptionMatchedStatus * status);

  static
  void
  on_sample_lost(
    void * listener_data,
    DDS_DataReader * reader,
    const struct DDS_SampleLostStatus * status);

  static
  void
  on_instance_replaced(
    void * listener_data,
    DDS_DataReader * reader,
    const struct DDS_DataReaderInstanceReplacedStatus * status);
};

template<class T, class H>
class RMW_Connext_TypedBuiltinListener : public RMW_Connext_BuiltinListener
{
public:
  explicit RMW_Connext_TypedBuiltinListener(rmw_context_impl_t * const ctx)
  : RMW_Connext_BuiltinListener(ctx) {}

  virtual ~RMW_Connext_TypedBuiltinListener()
  {
    for (auto & data : this->data_queue) {
      H::finalize(&data.data);
    }
  }

  bool
  next_data(RMW_Connext_CachedBuiltinData<T> & qdata)
  {
    std::lock_guard<std::mutex> guard(this->data_mutex);
    if (this->data_queue.size() == 0) {
      return false;
    }
    auto first = this->data_queue[0];
    qdata.valid_data = first.valid_data;
    qdata.instance_state = first.instance_state;
    qdata.instance_handle = first.instance_handle;
    rmw_ret_t rc = RMW_RET_OK;
    if (qdata.valid_data) {
      H::initialize(&qdata.data);
      rc = H::copy(&qdata.data, &first.data);
      H::finalize(&first.data);
    }
    this->data_queue.erase(this->data_queue.begin());
    return (RMW_RET_OK == rc) ? true : false;
  }

  virtual
  void
  received_sample(const void * const data, const DDS_SampleInfo * const info)
  {
    const T * const tdata = reinterpret_cast<const T *>(data);
    this->on_data(tdata, info);
  }

protected:
  virtual
  void
  process_data_available(DDS_DataReader * const reader)
  {
    H::process_data_available(this, reader);
  }

private:
  std::mutex data_mutex;
  std::vector<RMW_Connext_CachedBuiltinData<T>> data_queue;

  rmw_ret_t
  on_data(const T * const data, const DDS_SampleInfo * const info)
  {
    std::lock_guard<std::mutex> guard(this->data_mutex);
    RMW_Connext_CachedBuiltinData<T> qdata;
    H::initialize(&qdata.data);
    qdata.valid_data = info->valid_data;
    if (qdata.valid_data) {
      rmw_ret_t rc = H::copy(&qdata.data, data);
      if (RMW_RET_OK != rc) {
        H::finalize(&qdata.data);
        return rc;
      }
    }
    qdata.instance_state = info->instance_state;
    qdata.instance_handle = info->instance_handle;
    this->data_queue.push_back(qdata);
    return RMW_RET_OK;
  }
};

struct rmw_connextdds_api_micro
{
  bool rt_whsm;
  bool rt_rhsm;
  bool rt_udp;
  bool rt_shmem;
  bool rt_dpde;
  bool rt_sec;
  struct UDP_InterfaceFactoryProperty * udp_property;

  RMW_Connext_TypedBuiltinListener<DDS_ParticipantBuiltinTopicData, RMW_Connext_ParticipantData>
  discovery_dp_listener;
  RMW_Connext_TypedBuiltinListener<DDS_SubscriptionBuiltinTopicData, RMW_Connext_SubscriptionData>
  discovery_sub_listener;
  RMW_Connext_TypedBuiltinListener<DDS_PublicationBuiltinTopicData, RMW_Connext_PublicationData>
  discovery_pub_listener;

  explicit rmw_connextdds_api_micro(rmw_context_impl_t * const ctx)
  : rt_whsm(false),
    rt_rhsm(false),
    rt_udp(false),
    rt_shmem(false),
    rt_dpde(false),
    rt_sec(false),
    udp_property(nullptr),
    discovery_dp_listener(ctx),
    discovery_sub_listener(ctx),
    discovery_pub_listener(ctx)
  {}
};

rmw_connextdds_api_micro * RMW_Connext_fv_FactoryContext = nullptr;

const char * const RMW_CONNEXTDDS_ID = "rmw_connextddsmicro";
const char * const RMW_CONNEXTDDS_SERIALIZATION_FORMAT = "cdr";

rmw_ret_t
rmw_connextdds_set_log_verbosity(rmw_log_severity_t severity)
{
  OSAPI_LogVerbosity_T verbosity = OSAPI_LOG_VERBOSITY_ERROR;

  switch (severity) {
    case RMW_LOG_SEVERITY_DEBUG:
    case RMW_LOG_SEVERITY_INFO:
      {
        verbosity = OSAPI_LOG_VERBOSITY_DEBUG;
        break;
      }
    case RMW_LOG_SEVERITY_WARN:
      {
        verbosity = OSAPI_LOG_VERBOSITY_WARNING;
        break;
      }
    case RMW_LOG_SEVERITY_ERROR:
    case RMW_LOG_SEVERITY_FATAL:
      {
        verbosity = OSAPI_LOG_VERBOSITY_ERROR;
        break;
      }
    default:
      {
        RMW_CONNEXT_LOG_ERROR_A_SET("invalid log level: %d", severity)
        return RMW_RET_INVALID_ARGUMENT;
      }
  }

  OSAPI_Log_set_verbosity(verbosity);
  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_initialize_participant_factory_context(
  rmw_context_impl_t * const ctx)
{
  RMW_CONNEXT_ASSERT(RMW_Connext_fv_FactoryContext == nullptr)
  RMW_CONNEXT_ASSERT(RMW_Connext_gv_DomainParticipantFactory == nullptr)

  DDS_DomainParticipantFactory * factory = NULL;
  RT_Registry_T * registry = NULL;

  struct OSAPI_LogProperty log_prop;

  const char * const whsm_name = DDSHST_WRITER_DEFAULT_HISTORY_NAME,
  * const rhsm_name = DDSHST_READER_DEFAULT_HISTORY_NAME,
  * const udp_name = NETIO_DEFAULT_UDP_NAME,
  * const shmem_name = NETIO_DEFAULT_SHMEM_NAME,
  * const dpde_name = "dpde";

  rmw_connextdds_api_micro * ctx_api = nullptr;

  auto scope_exit_api_delete = rcpputils::make_scope_exit(
    [ctx_api, registry, whsm_name, rhsm_name, udp_name, shmem_name, dpde_name]()
    {
      if (ctx_api) {
        if (ctx_api->rt_whsm) {
          if (!RT_Registry_unregister(registry, whsm_name, NULL, NULL)) {
            RMW_CONNEXT_LOG_ERROR_SET("failed to unregister whsm")
          }
        }
        if (ctx_api->rt_rhsm) {
          if (!RT_Registry_unregister(registry, rhsm_name, NULL, NULL)) {
            RMW_CONNEXT_LOG_ERROR_SET("failed to unregister rhsm")
          }
        }
        if (ctx_api->rt_udp) {
          if (!RT_Registry_unregister(registry, udp_name, NULL, NULL)) {
            RMW_CONNEXT_LOG_ERROR_SET("failed to unregister udp")
          }
        }
        if (ctx_api->rt_shmem) {
          if (!RT_Registry_unregister(registry, shmem_name, NULL, NULL)) {
            RMW_CONNEXT_LOG_ERROR_SET("failed to unregister shmem")
          }
        }
        if (ctx_api->rt_dpde) {
          if (!RT_Registry_unregister(registry, dpde_name, NULL, NULL)) {
            RMW_CONNEXT_LOG_ERROR_SET("failed to unregister dpde")
          }
        }
        if (nullptr != ctx_api->udp_property) {
          DDS_StringSeq_finalize(&ctx_api->udp_property->allow_interface);
          delete ctx_api->udp_property;
        }
        delete ctx_api;
      }
      DDS_DomainParticipantFactory_finalize_instance();
    });

  RMW_CONNEXT_LOG_DEBUG("initializing DDS DomainParticipantFactory")

  OSAPI_Log_get_property(&log_prop);
  log_prop.max_buffer_size = OSAPI_LOG_BUFFER_SIZE * 100;
  OSAPI_Log_set_property(&log_prop);

  factory = DDS_DomainParticipantFactory_get_instance();
  if (nullptr == factory) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to get DDS participant factory")
    return RMW_RET_ERROR;
  }

  ctx_api = new (std::nothrow) rmw_connextdds_api_micro(ctx);
  if (nullptr == ctx_api) {
    return RMW_RET_ERROR;
  }

  registry = DDS_DomainParticipantFactory_get_registry(factory);
  if (nullptr == registry) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to get factory registry")
    return RMW_RET_ERROR;
  }

  /* Configure Writer and Reader History plugins */

  if (!RT_Registry_register(
      registry,
      whsm_name, WHSM_HistoryFactory_get_interface(), NULL, NULL))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to register Writer History plugin")
    return RMW_RET_ERROR;
  }

  ctx_api->rt_whsm = true;

  if (!RT_Registry_register(
      registry,
      rhsm_name, RHSM_HistoryFactory_get_interface(), NULL, NULL))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to register Reader History plugin")
    return RMW_RET_ERROR;
  }

  ctx_api->rt_rhsm = true;

  /* Configure UDP Transport plugin */

  if (!RT_Registry_unregister(registry, udp_name, NULL, NULL)) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to unregister udp")
    return RMW_RET_ERROR;
  }

  ctx_api->udp_property = new (std::nothrow) UDP_InterfaceFactoryProperty();

  if (nullptr == ctx_api->udp_property) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to allocate UDP Transport property")
    return RMW_RET_ERROR;
  }

  *ctx_api->udp_property = UDP_INTERFACE_FACTORY_PROPERTY_DEFAULT;

  if (!DDS_StringSeq_set_maximum(&ctx_api->udp_property->allow_interface, 1)) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to set allow_interface maximum")
    return RMW_RET_ERROR;
  }
  if (!DDS_StringSeq_set_length(&ctx_api->udp_property->allow_interface, 1)) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to set allow_interface length")
    return RMW_RET_ERROR;
  }

  /* Lookup name of UDP interface from environment */
  const char * env_udp_intf = nullptr;
  const char * lookup_rc =
    rcutils_get_env(RMW_CONNEXT_ENV_UDP_INTERFACE, &env_udp_intf);

  if (nullptr != lookup_rc || nullptr == env_udp_intf) {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed to lookup from environment: "
      "var=%s, "
      "rc=%s ",
      RMW_CONNEXT_ENV_UDP_INTERFACE,
      lookup_rc)
    return RMW_RET_ERROR;
  }

  if (strlen(env_udp_intf) == 0) {
    env_udp_intf = RMW_CONNEXT_DEFAULT_UDP_INTERFACE;
  }

  char * udp_intf = DDS_String_dup(env_udp_intf);
  if (nullptr == udp_intf) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to allocate udp interface name")
    return RMW_RET_ERROR;
  }

  *DDS_StringSeq_get_reference(&ctx_api->udp_property->allow_interface, 0) =
    udp_intf;

  RMW_CONNEXT_LOG_DEBUG_A("UDP interface: %s", udp_intf)

  if (!RT_Registry_register(
      registry,
      udp_name,
      UDP_InterfaceFactory_get_interface(),
      (struct RT_ComponentFactoryProperty *)ctx_api->udp_property, NULL))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to register UDP Transport plugin")
    return RMW_RET_ERROR;
  }

  ctx_api->rt_udp = true;

#if RMW_CONNEXT_TRANSPORT_SHMEM
  struct NETIO_SHMEMInterfaceFactoryProperty shmem_property =
    NETIO_SHMEMInterfaceFactoryProperty_INITIALIZER;

  if (!RT_Registry_register(
      registry,
      shmem_name,
      NETIO_SHMEMInterfaceFactory_get_interface(),
      (struct RT_ComponentFactoryProperty *)&shmem_property, NULL))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to register SHMEM Transport plugin")
    return RMW_RET_ERROR;
  }

  ctx_api->rt_shmem = true;
#endif /* RMW_CONNEXT_TRANSPORT_SHMEM */

  struct DPDE_DiscoveryPluginProperty discovery_plugin_properties =
    DPDE_DiscoveryPluginProperty_INITIALIZER;

#if defined(CONFIGURE_DPDE_LIVELINESS_SOMEHOW) || 1
  discovery_plugin_properties.participant_liveliness_assert_period.sec =
    10;
  discovery_plugin_properties.participant_liveliness_assert_period.nanosec =
    0;

  discovery_plugin_properties.participant_liveliness_lease_duration.sec =
    60;
  discovery_plugin_properties.participant_liveliness_lease_duration.nanosec =
    0;
#endif

  if (!RT_Registry_register(
      registry,
      dpde_name,
      DPDE_DiscoveryFactory_get_interface(),
      &discovery_plugin_properties._parent,
      NULL))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to register DPDE Discovery plugin")
    return RMW_RET_ERROR;
  }

  ctx_api->rt_dpde = true;

  scope_exit_api_delete.cancel();

  RMW_CONNEXT_LOG_DEBUG("DDS DomainParticipantFactory initialized")

  RMW_Connext_fv_FactoryContext = ctx_api;
  RMW_Connext_gv_DomainParticipantFactory = factory;

  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_finalize_participant_factory_context(
  rmw_context_impl_t * const ctx)
{
  UNUSED_ARG(ctx);

  RMW_CONNEXT_ASSERT(nullptr != RMW_Connext_fv_FactoryContext)
  rmw_connextdds_api_micro * const ctx_api = RMW_Connext_fv_FactoryContext;

  RMW_Connext_fv_FactoryContext = nullptr;

  RT_Registry_T * registry =
    DDS_DomainParticipantFactory_get_registry(
    RMW_Connext_gv_DomainParticipantFactory);
  if (nullptr == registry) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to get factory registry")
    return RMW_RET_ERROR;
  }

  if (ctx_api->rt_whsm) {
    if (!RT_Registry_unregister(
        registry,
        DDSHST_WRITER_DEFAULT_HISTORY_NAME,
        NULL, NULL))
    {
      RMW_CONNEXT_LOG_ERROR_SET(
        "failed to unregister Writer History plugin")
      return RMW_RET_ERROR;
    }
    ctx_api->rt_whsm = false;
  }

  if (ctx_api->rt_rhsm) {
    if (!RT_Registry_unregister(
        registry,
        DDSHST_READER_DEFAULT_HISTORY_NAME,
        NULL, NULL))
    {
      RMW_CONNEXT_LOG_ERROR_SET(
        "failed to unregister Reader History plugin")
      return RMW_RET_ERROR;
    }
    ctx_api->rt_rhsm = false;
  }

  if (ctx_api->rt_udp) {
    if (!RT_Registry_unregister(
        registry,
        NETIO_DEFAULT_UDP_NAME,
        NULL, NULL))
    {
      RMW_CONNEXT_LOG_ERROR_SET(
        "failed to unregister UDP Transport plugin")
      return RMW_RET_ERROR;
    }
    ctx_api->rt_udp = false;
  }

  if (ctx_api->rt_shmem) {
    if (!RT_Registry_unregister(
        registry,
        NETIO_DEFAULT_SHMEM_NAME,
        NULL, NULL))
    {
      RMW_CONNEXT_LOG_ERROR_SET(
        "failed to unregister SHMEM Transport plugin")
      return RMW_RET_ERROR;
    }
    ctx_api->rt_shmem = false;
  }

  if (ctx_api->rt_dpde) {
    if (!RT_Registry_unregister(registry, "dpde", NULL, NULL)) {
      RMW_CONNEXT_LOG_ERROR_SET(
        "failed to unregister DPDE Discovery plugin")
      return RMW_RET_ERROR;
    }
    ctx_api->rt_rhsm = false;
  }

  if (nullptr != ctx_api->udp_property) {
    UDP_InterfaceFactoryProperty_finalize(ctx_api->udp_property);
    delete ctx_api->udp_property;
    ctx_api->udp_property = nullptr;
  }

  delete ctx_api;

  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_initialize_participant_qos_impl(
  rmw_context_impl_t * const ctx,
  DDS_DomainParticipantQos * const dp_qos)
{
  UNUSED_ARG(ctx);

  /* TODO(asorbini:) Store enclave's name in USER_DATA field */

  /*  TODO(asorbini) Configure DDS Security options */

  size_t max_transports = 1;
#if RMW_CONNEXT_TRANSPORT_SHMEM
  max_transports += 1;
#endif /* RMW_CONNEXT_TRANSPORT_SHMEM */

  /* Mark only UDP transport as enabled, and skip INTRA */
  if (!REDA_StringSeq_set_maximum(
      &dp_qos->transports.enabled_transports, max_transports))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to set transports.enabled_transports maximum")
    return RMW_RET_ERROR;
  }
  if (!REDA_StringSeq_set_length(
      &dp_qos->transports.enabled_transports, max_transports))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to set transports.enabled_transports length")
    return RMW_RET_ERROR;
  }
  char ** seq_ref =
    REDA_StringSeq_get_reference(&dp_qos->transports.enabled_transports, 0);
  *seq_ref = REDA_String_dup(NETIO_DEFAULT_UDP_NAME);
  if (nullptr == *seq_ref) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to allocate string for transports.enabled_transports")
    return RMW_RET_ERROR;
  }

#if RMW_CONNEXT_TRANSPORT_SHMEM
  seq_ref =
    REDA_StringSeq_get_reference(&dp_qos->transports.enabled_transports, 1);
  *seq_ref = REDA_String_dup(NETIO_DEFAULT_SHMEM_NAME);
  if (nullptr == *seq_ref) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to get transports.enabled_transports reference")
    return RMW_RET_ERROR;
  }
#endif /* RMW_CONNEXT_TRANSPORT_SHMEM */

  if (!REDA_StringSeq_set_maximum(
      &dp_qos->user_traffic.enabled_transports, max_transports))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to set user_traffic.enabled_transports maximum")
    return RMW_RET_ERROR;
  }
  if (!REDA_StringSeq_set_length(
      &dp_qos->user_traffic.enabled_transports, max_transports))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to set user_traffic.enabled_transports length")
    return RMW_RET_ERROR;
  }

  seq_ref =
    REDA_StringSeq_get_reference(
    &dp_qos->user_traffic.enabled_transports, 0);

  std::ostringstream udp_ss;
  udp_ss << NETIO_DEFAULT_UDP_NAME << "://";

  *seq_ref = REDA_String_dup(udp_ss.str().c_str());
  if (nullptr == *seq_ref) {
    return RMW_RET_ERROR;
  }

#if RMW_CONNEXT_TRANSPORT_SHMEM
  seq_ref =
    REDA_StringSeq_get_reference(
    &dp_qos->user_traffic.enabled_transports, 1);
  std::ostringstream shmem_ss;
  shmem_ss << NETIO_DEFAULT_SHMEM_NAME << "://";

  *seq_ref = REDA_String_dup(shmem_ss.str().c_str());
  if (nullptr == *seq_ref) {
    return RMW_RET_ERROR;
  }
#endif /* RMW_CONNEXT_TRANSPORT_SHMEM */

  /* Increate Participant resource limits */
  dp_qos->resource_limits.local_type_allocation =
    RMW_CONNEXT_LIMIT_TYPES_LOCAL_MAX;

  dp_qos->resource_limits.local_topic_allocation =
    RMW_CONNEXT_LIMIT_TOPICS_LOCAL_MAX;

  dp_qos->resource_limits.local_reader_allocation =
    RMW_CONNEXT_LIMIT_READERS_LOCAL_MAX;

  dp_qos->resource_limits.local_writer_allocation =
    RMW_CONNEXT_LIMIT_WRITERS_LOCAL_MAX;

  dp_qos->resource_limits.remote_participant_allocation =
    RMW_CONNEXT_LIMIT_PARTICIPANTS_REMOTE_MAX;

  dp_qos->resource_limits.remote_writer_allocation =
    RMW_CONNEXT_LIMIT_WRITERS_REMOTE_MAX;

  dp_qos->resource_limits.remote_reader_allocation =
    RMW_CONNEXT_LIMIT_READERS_REMOTE_MAX;

  dp_qos->resource_limits.local_publisher_allocation = 1;

  dp_qos->resource_limits.local_subscriber_allocation = 1;

  dp_qos->resource_limits.matching_reader_writer_pair_allocation =
    dp_qos->resource_limits.local_reader_allocation *
    dp_qos->resource_limits.remote_writer_allocation *
    dp_qos->resource_limits.remote_participant_allocation;

  dp_qos->resource_limits.matching_writer_reader_pair_allocation =
    dp_qos->resource_limits.local_writer_allocation *
    dp_qos->resource_limits.remote_reader_allocation *
    dp_qos->resource_limits.remote_participant_allocation;

  if (!RT_ComponentFactoryId_set_name(
      &dp_qos->discovery.discovery.name, "dpde"))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to set discovery plugin name")
    return RMW_RET_ERROR;
  }

  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_configure_participant(
  rmw_context_impl_t * const ctx,
  DDS_DomainParticipant * const participant)
{
  UNUSED_ARG(participant);
  UNUSED_ARG(ctx);

  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_create_contentfilteredtopic(
  rmw_context_impl_t * const ctx,
  DDS_DomainParticipant * const dp,
  DDS_Topic * const base_topic,
  const char * const cft_name,
  const char * const cft_filter,
  const rcutils_string_array_t * const cft_expression_parameters,
  DDS_TopicDescription ** const cft_out)
{
  UNUSED_ARG(ctx);
  UNUSED_ARG(dp);
  UNUSED_ARG(base_topic);
  UNUSED_ARG(cft_name);
  UNUSED_ARG(cft_filter);
  UNUSED_ARG(cft_expression_parameters);
  UNUSED_ARG(cft_out);
  return RMW_RET_UNSUPPORTED;
}

rmw_ret_t
rmw_connextdds_delete_contentfilteredtopic(
  rmw_context_impl_t * const ctx,
  DDS_DomainParticipant * const dp,
  DDS_TopicDescription * const cft_topic)
{
  UNUSED_ARG(ctx);
  UNUSED_ARG(cft_topic);
  UNUSED_ARG(dp);
  return RMW_RET_UNSUPPORTED;
}

static
rmw_ret_t
rmw_connextdds_get_qos_policies(
  const bool writer_qos,
  RMW_Connext_MessageTypeSupport * const type_support,
  DDS_HistoryQosPolicy * const history,
  DDS_ReliabilityQosPolicy * const reliability,
  DDS_DurabilityQosPolicy * const durability,
  DDS_DeadlineQosPolicy * const deadline,
  DDS_LivelinessQosPolicy * const liveliness,
  DDS_ResourceLimitsQosPolicy * const resource_limits,
  DDS_DataReaderResourceLimitsQosPolicy * const reader_resource_limits,
  DDS_DataWriterResourceLimitsQosPolicy * const writer_resource_limits,
  DDS_DataReaderProtocolQosPolicy * const reader_protocol,
  DDS_DataWriterProtocolQosPolicy * const writer_protocol,
  const rmw_qos_profile_t * const qos_policies,
  const rmw_publisher_options_t * const pub_options,
  const rmw_subscription_options_t * const sub_options)
{
  UNUSED_ARG(type_support);
  UNUSED_ARG(writer_qos);
  UNUSED_ARG(reliability);
  UNUSED_ARG(durability);
  UNUSED_ARG(deadline);
  UNUSED_ARG(liveliness);
  UNUSED_ARG(qos_policies);
  UNUSED_ARG(pub_options);
  UNUSED_ARG(sub_options);

  /* Adjust resource limits according to other QoS policies */
  size_t max_samples = RMW_CONNEXT_LIMIT_SAMPLES_MAX;

  if (DDS_LENGTH_UNLIMITED != history->depth &&
    static_cast<size_t>(history->depth) > max_samples)
  {
    max_samples = history->depth;
  }
  if (max_samples < RMW_CONNEXT_LIMIT_SAMPLES_MAX) {
    max_samples = RMW_CONNEXT_LIMIT_SAMPLES_MAX;
  }

  resource_limits->max_samples_per_instance = max_samples;
  resource_limits->max_samples = max_samples;
  resource_limits->max_instances = 1;   /* ROS doesn't use instances */

  RMW_CONNEXT_LOG_DEBUG_A(
    "endpoint resource limits: "
    "max_samples_per_instance=%d, "
    "max_samples=%d, "
    "max_instances=%d",
    resource_limits->max_samples_per_instance,
    resource_limits->max_samples,
    resource_limits->max_instances);

  if (nullptr != reader_resource_limits) {
    reader_resource_limits->max_remote_writers =
      RMW_CONNEXT_LIMIT_WRITERS_LOCAL_MAX +
      RMW_CONNEXT_LIMIT_WRITERS_REMOTE_MAX;
    reader_resource_limits->max_remote_writers_per_instance =
      RMW_CONNEXT_LIMIT_WRITERS_LOCAL_MAX +
      RMW_CONNEXT_LIMIT_WRITERS_REMOTE_MAX;
    // reader_resource_limits->max_samples_per_remote_writer = 0;
    reader_resource_limits->max_outstanding_reads = 1;
    reader_resource_limits->max_routes_per_writer = 1;
  }

  if (nullptr != writer_resource_limits) {
    writer_resource_limits->max_remote_readers =
      RMW_CONNEXT_LIMIT_READERS_LOCAL_MAX +
      RMW_CONNEXT_LIMIT_READERS_REMOTE_MAX;
    writer_resource_limits->max_routes_per_reader = 1;
  }

  if (nullptr != reader_protocol) {
    reader_protocol->rtps_reliable_reader.nack_period.sec = 0;
    reader_protocol->rtps_reliable_reader.nack_period.nanosec = 10000000;
  }

  if (nullptr != writer_protocol) {
    RMW_CONNEXT_ASSERT(nullptr != writer_resource_limits)
    writer_protocol->rtps_reliable_writer.heartbeats_per_max_samples =
      resource_limits->max_samples;
  }
  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_get_datawriter_qos(
  rmw_context_impl_t * const ctx,
  RMW_Connext_MessageTypeSupport * const type_support,
  DDS_Topic * const topic,
  DDS_DataWriterQos * const qos,
  const rmw_qos_profile_t * const qos_policies,
  const rmw_publisher_options_t * const pub_options)
{
  UNUSED_ARG(ctx);
  UNUSED_ARG(topic);

  if (RMW_RET_OK !=
    rmw_connextdds_get_readerwriter_qos(
      true /* writer_qos */,
      type_support,
      &qos->history,
      &qos->reliability,
      &qos->durability,
      &qos->deadline,
      &qos->liveliness,
      &qos->resource_limits,
      &qos->publish_mode,
      nullptr /* Micro doesn't support DDS_LifespanQosPolicy */,
      qos_policies,
      pub_options,
      nullptr /* sub_options */))
  {
    return RMW_RET_ERROR;
  }

  return rmw_connextdds_get_qos_policies(
    true /* writer_qos */,
    type_support,
    &qos->history,
    &qos->reliability,
    &qos->durability,
    &qos->deadline,
    &qos->liveliness,
    &qos->resource_limits,
    nullptr /* reader_resource_limits */,
    &qos->writer_resource_limits,
    nullptr /* reader protocol */,
    &qos->protocol,
    qos_policies,
    pub_options,
    nullptr /* sub_options */);
}

rmw_ret_t
rmw_connextdds_get_datareader_qos(
  rmw_context_impl_t * const ctx,
  RMW_Connext_MessageTypeSupport * const type_support,
  DDS_TopicDescription * const topic_desc,
  DDS_DataReaderQos * const qos,
  const rmw_qos_profile_t * const qos_policies,
  const rmw_subscription_options_t * const sub_options)
{
  UNUSED_ARG(ctx);
  UNUSED_ARG(topic_desc);

  if (RMW_RET_OK !=
    rmw_connextdds_get_readerwriter_qos(
      false /* writer_qos */,
      type_support,
      &qos->history,
      &qos->reliability,
      &qos->durability,
      &qos->deadline,
      &qos->liveliness,
      &qos->resource_limits,
      nullptr /* publish_mode */,
      nullptr /* Lifespan is a writer-only qos policy */,
      qos_policies,
      nullptr /* pub_options */,
      sub_options))
  {
    return RMW_RET_ERROR;
  }
  return rmw_connextdds_get_qos_policies(
    false /* writer_qos */,
    type_support,
    &qos->history,
    &qos->reliability,
    &qos->durability,
    &qos->deadline,
    &qos->liveliness,
    &qos->resource_limits,
    &qos->reader_resource_limits,
    nullptr /* writer_resource_limits */,
    &qos->protocol,
    nullptr /* writer protocol */,
    qos_policies,
    nullptr /* pub_options */,
    sub_options);
}

DDS_DataWriter *
rmw_connextdds_create_datawriter(
  rmw_context_impl_t * const ctx,
  DDS_DomainParticipant * const participant,
  DDS_Publisher * const pub,
  const rmw_qos_profile_t * const qos_policies,
  const rmw_publisher_options_t * const publisher_options,
  const bool internal,
  RMW_Connext_MessageTypeSupport * const type_support,
  DDS_Topic * const topic,
  DDS_DataWriterQos * const dw_qos)
{
  UNUSED_ARG(ctx);
  UNUSED_ARG(participant);
  UNUSED_ARG(internal);

  if (RMW_RET_OK !=
    rmw_connextdds_get_datawriter_qos(
      ctx, type_support, topic, dw_qos, qos_policies, publisher_options))
  {
    RMW_CONNEXT_LOG_ERROR("failed to convert writer QoS")
    return nullptr;
  }

  DDS_DataWriter * const writer =
    DDS_Publisher_create_datawriter(
    pub, topic, dw_qos, NULL, DDS_STATUS_MASK_NONE);

  return writer;
}

DDS_DataReader *
rmw_connextdds_create_datareader(
  rmw_context_impl_t * const ctx,
  DDS_DomainParticipant * const participant,
  DDS_Subscriber * const sub,
  const rmw_qos_profile_t * const qos_policies,
  const rmw_subscription_options_t * const subscriber_options,
  const bool internal,
  RMW_Connext_MessageTypeSupport * const type_support,
  DDS_TopicDescription * const topic_desc,
  DDS_DataReaderQos * const dr_qos)
{
  UNUSED_ARG(ctx);
  UNUSED_ARG(participant);
  UNUSED_ARG(internal);

  if (RMW_RET_OK !=
    rmw_connextdds_get_datareader_qos(
      ctx, type_support, topic_desc, dr_qos, qos_policies, subscriber_options))
  {
    RMW_CONNEXT_LOG_ERROR("failed to convert reader QoS")
    return nullptr;
  }

  return DDS_Subscriber_create_datareader(
    sub, topic_desc, dr_qos, NULL, DDS_STATUS_MASK_NONE);
}

rmw_ret_t
rmw_connextdds_write_message(
  RMW_Connext_Publisher * const pub,
  RMW_Connext_Message * const message,
  int64_t * const sn_out)
{
  UNUSED_ARG(sn_out);

  if (DDS_RETCODE_OK !=
    DDS_DataWriter_write(pub->writer(), message, &DDS_HANDLE_NIL))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to write message to DDS")
    return RMW_RET_ERROR;
  }

  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_take_samples(
  RMW_Connext_Subscriber * const sub)
{
  DDS_ReturnCode_t rc =
    DDS_DataReader_take(
    sub->reader(),
    sub->data_seq(),
    sub->info_seq(),
    DDS_LENGTH_UNLIMITED,
    DDS_ANY_VIEW_STATE,
    DDS_ANY_SAMPLE_STATE,
    DDS_ANY_INSTANCE_STATE);

  if (DDS_RETCODE_NO_DATA == rc) {
    return RMW_RET_OK;
  } else if (DDS_RETCODE_OK != rc) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to take data from DDS reader")
    return RMW_RET_ERROR;
  }

  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_return_samples(
  RMW_Connext_Subscriber * const sub)
{
  if (DDS_RETCODE_OK !=
    DDS_DataReader_return_loan(
      sub->reader(), sub->data_seq(), sub->info_seq()))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to return data to DDS reader")
    return RMW_RET_ERROR;
  }
  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_count_unread_samples(
  RMW_Connext_Subscriber * const sub,
  size_t & unread_count)
{
  unread_count = 0;
  DDS_SampleInfoSeq info_seq = DDS_SEQUENCE_INITIALIZER;
  DDS_UntypedSampleSeq data_seq = DDS_SEQUENCE_INITIALIZER;
  DDS_ReturnCode_t rc = DDS_RETCODE_ERROR;
  do {
    rc = DDS_DataReader_read(
      sub->reader(),
      &data_seq,
      &info_seq,
      DDS_LENGTH_UNLIMITED,
      DDS_ANY_VIEW_STATE,
      DDS_NOT_READ_SAMPLE_STATE,
      DDS_ANY_INSTANCE_STATE);
    if (DDS_RETCODE_OK != rc && DDS_RETCODE_NO_DATA != rc) {
      RMW_CONNEXT_LOG_ERROR_SET("failed to read data from DDS reader")
      return RMW_RET_ERROR;
    }
    if (DDS_RETCODE_OK == rc) {
      unread_count += DDS_UntypedSampleSeq_get_length(&data_seq);
      rc = DDS_DataReader_return_loan(sub->reader(), &data_seq, &info_seq);
      if (DDS_RETCODE_OK != rc) {
        RMW_CONNEXT_LOG_ERROR_SET("failed to return loan to DDS reader")
        return RMW_RET_ERROR;
      }
    }
  } while (rc == DDS_RETCODE_OK);
  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_filter_sample(
  RMW_Connext_Subscriber * const sub,
  const void * const sample,
  const DDS_SampleInfo * const info,
  const DDS_InstanceHandle_t * const request_writer_handle,
  bool * const accepted)
{
  UNUSED_ARG(info);
  // In this implementation, local samples are dropped by the
  // DataReaderListener::on_before_sample_commit() callback.
  // TODO(asorbini) add filtering because this isn't true anymore
  *accepted = true;

  if (nullptr != request_writer_handle) {
    if (!sub->message_type_support()->type_requestreply()) {
      return RMW_RET_ERROR;
    }

    const RMW_Connext_RequestReplyMessage * const rr_msg =
      reinterpret_cast<const RMW_Connext_RequestReplyMessage *>(sample);
    // Convert instance handle to guid
    DDS_GUID_t writer_guid = DDS_GUID_INITIALIZER,
      related_writer_guid = DDS_GUID_INITIALIZER;
    memcpy(
      writer_guid.value,
      request_writer_handle->octet,
      16);
    RMW_CONNEXT_ASSERT(nullptr != rr_msg)
    rmw_connextdds_gid_to_guid(rr_msg->gid, related_writer_guid);

    if (sub->message_type_support()->ctx()->cyclone_compatible) {
      // Compare only the 8 MSB from writer_guid
      *accepted = (memcmp(
          static_cast<void *>(writer_guid.value + 8),
          static_cast<void *>(related_writer_guid.value + 8), 8) == 0);
    } else {
      *accepted =
        (DDS_GUID_compare(&writer_guid, &related_writer_guid) == 0);
    }
  }

  return RMW_RET_OK;
}

void
RMW_Connext_ParticipantData::process_data_available(
  RMW_Connext_BuiltinListener * const listener,
  DDS_DataReader * const reader)
{
  struct DDS_ParticipantBuiltinTopicDataSeq data_seq =
    DDS_ParticipantBuiltinTopicDataSeq_INITIALIZER;
  struct DDS_SampleInfoSeq info_seq = DDS_SEQUENCE_INITIALIZER;

  DDS_ReturnCode_t retcode = DDS_RETCODE_ERROR;

  do {
    retcode =
      DDS_DataReader_read(
      reader,
      reinterpret_cast<DDS_UntypedSampleSeq *>(&data_seq),
      &info_seq,
      DDS_LENGTH_UNLIMITED,
      DDS_NOT_READ_SAMPLE_STATE,
      DDS_ANY_VIEW_STATE,
      DDS_ANY_INSTANCE_STATE);

    if (DDS_RETCODE_NO_DATA == retcode) {
      break;
    } else if (DDS_RETCODE_OK != retcode) {
      // TODO(asorbini) log without lock
      break;
    }

    const size_t l = DDS_ParticipantBuiltinTopicDataSeq_get_length(&data_seq);
    for (size_t i = 0; i < l; ++i) {
      DDS_ParticipantBuiltinTopicData * const data =
        reinterpret_cast<DDS_ParticipantBuiltinTopicData *>(
        DDS_UntypedSampleSeq_get_reference(
          reinterpret_cast<DDS_UntypedSampleSeq *>(&data_seq), i));
      DDS_SampleInfo * const info = DDS_SampleInfoSeq_get_reference(&info_seq, i);
      listener->received_sample(data, info);
    }

    retcode = DDS_DataReader_return_loan(
      reader,
      reinterpret_cast<DDS_UntypedSampleSeq *>(&data_seq), &info_seq);
  } while (retcode == DDS_RETCODE_OK);
}

void
RMW_Connext_PublicationData::process_data_available(
  RMW_Connext_BuiltinListener * const listener,
  DDS_DataReader * const reader)
{
  struct DDS_PublicationBuiltinTopicDataSeq data_seq =
    DDS_PublicationBuiltinTopicDataSeq_INITIALIZER;
  struct DDS_SampleInfoSeq info_seq = DDS_SEQUENCE_INITIALIZER;

  DDS_ReturnCode_t retcode = DDS_RETCODE_ERROR;

  do {
    retcode =
      DDS_DataReader_read(
      reader,
      reinterpret_cast<DDS_UntypedSampleSeq *>(&data_seq),
      &info_seq,
      DDS_LENGTH_UNLIMITED,
      DDS_NOT_READ_SAMPLE_STATE,
      DDS_ANY_VIEW_STATE,
      DDS_ANY_INSTANCE_STATE);

    if (DDS_RETCODE_NO_DATA == retcode) {
      break;
    } else if (DDS_RETCODE_OK != retcode) {
      // TODO(asorbini) log without lock
      break;
    }

    const size_t l =
      DDS_PublicationBuiltinTopicDataSeq_get_length(&data_seq);
    for (size_t i = 0; i < l; ++i) {
      DDS_PublicationBuiltinTopicData * const data =
        reinterpret_cast<DDS_PublicationBuiltinTopicData *>(
        DDS_UntypedSampleSeq_get_reference(
          reinterpret_cast<DDS_UntypedSampleSeq *>(&data_seq), i));
      DDS_SampleInfo * const info = DDS_SampleInfoSeq_get_reference(&info_seq, i);

      listener->received_sample(data, info);
    }

    retcode = DDS_DataReader_return_loan(
      reader,
      reinterpret_cast<DDS_UntypedSampleSeq *>(&data_seq), &info_seq);
  } while (retcode == DDS_RETCODE_OK);
}


void
RMW_Connext_SubscriptionData::process_data_available(
  RMW_Connext_BuiltinListener * const listener,
  DDS_DataReader * const reader)
{
  struct DDS_SubscriptionBuiltinTopicDataSeq data_seq =
    DDS_SubscriptionBuiltinTopicDataSeq_INITIALIZER;
  struct DDS_SampleInfoSeq info_seq = DDS_SEQUENCE_INITIALIZER;

  DDS_ReturnCode_t retcode = DDS_RETCODE_ERROR;

  do {
    retcode =
      DDS_DataReader_read(
      reader,
      reinterpret_cast<DDS_UntypedSampleSeq *>(&data_seq),
      &info_seq,
      DDS_LENGTH_UNLIMITED,
      DDS_NOT_READ_SAMPLE_STATE,
      DDS_ANY_VIEW_STATE,
      DDS_ANY_INSTANCE_STATE);

    if (DDS_RETCODE_NO_DATA == retcode) {
      break;
    } else if (DDS_RETCODE_OK != retcode) {
      // TODO(asorbini) log without lock
      break;
    }

    const size_t l =
      DDS_SubscriptionBuiltinTopicDataSeq_get_length(&data_seq);
    for (size_t i = 0; i < l; ++i) {
      DDS_SubscriptionBuiltinTopicData * const data =
        reinterpret_cast<DDS_SubscriptionBuiltinTopicData *>(
        DDS_UntypedSampleSeq_get_reference(
          reinterpret_cast<DDS_UntypedSampleSeq *>(&data_seq), i));
      DDS_SampleInfo * const info = DDS_SampleInfoSeq_get_reference(&info_seq, i);

      listener->received_sample(data, info);
    }

    retcode = DDS_DataReader_return_loan(
      reader,
      reinterpret_cast<DDS_UntypedSampleSeq *>(&data_seq), &info_seq);
  } while (retcode == DDS_RETCODE_OK);
}

void
RMW_Connext_BuiltinListener::on_requested_deadline_missed(
  void * listener_data,
  DDS_DataReader * reader,
  const struct DDS_RequestedDeadlineMissedStatus * status)
{
  RMW_Connext_BuiltinListener * const self =
    reinterpret_cast<RMW_Connext_BuiltinListener *>(listener_data);

  if (nullptr != self->parent_listener.on_requested_deadline_missed) {
    self->parent_listener.on_requested_deadline_missed(
      self->parent_listener.as_listener.listener_data, reader, status);
  }
}

void
RMW_Connext_BuiltinListener::on_liveliness_changed(
  void * listener_data,
  DDS_DataReader * reader,
  const struct DDS_LivelinessChangedStatus * status)
{
  RMW_Connext_BuiltinListener * const self =
    reinterpret_cast<RMW_Connext_BuiltinListener *>(listener_data);

  if (nullptr != self->parent_listener.on_liveliness_changed) {
    self->parent_listener.on_liveliness_changed(
      self->parent_listener.as_listener.listener_data, reader, status);
  }
}

void
RMW_Connext_BuiltinListener::on_requested_incompatible_qos(
  void * listener_data,
  DDS_DataReader * reader,
  const struct DDS_RequestedIncompatibleQosStatus * status)
{
  RMW_Connext_BuiltinListener * const self =
    reinterpret_cast<RMW_Connext_BuiltinListener *>(listener_data);

  if (nullptr != self->parent_listener.on_requested_incompatible_qos) {
    self->parent_listener.on_requested_incompatible_qos(
      self->parent_listener.as_listener.listener_data, reader, status);
  }
}

void
RMW_Connext_BuiltinListener::on_sample_rejected(
  void * listener_data,
  DDS_DataReader * reader,
  const struct DDS_SampleRejectedStatus * status)
{
  RMW_Connext_BuiltinListener * const self =
    reinterpret_cast<RMW_Connext_BuiltinListener *>(listener_data);

  if (nullptr != self->parent_listener.on_sample_rejected) {
    self->parent_listener.on_sample_rejected(
      self->parent_listener.as_listener.listener_data, reader, status);
  }
}

DDS_Boolean
RMW_Connext_BuiltinListener::on_before_sample_commit(
  void * listener_data,
  DDS_DataReader * reader,
  const void * const sample,
  const struct DDS_SampleInfo * const sample_info,
  DDS_Boolean * dropped)
{
  RMW_Connext_BuiltinListener * const self =
    reinterpret_cast<RMW_Connext_BuiltinListener *>(listener_data);

  if (nullptr != self->parent_listener.on_before_sample_commit) {
    return self->parent_listener.on_before_sample_commit(
      self->parent_listener.as_listener.listener_data,
      reader, sample, sample_info, dropped);
  }

  return DDS_BOOLEAN_TRUE;
}

void
RMW_Connext_BuiltinListener::on_subscription_matched(
  void * listener_data,
  DDS_DataReader * reader,
  const struct DDS_SubscriptionMatchedStatus * status)
{
  RMW_Connext_BuiltinListener * const self =
    reinterpret_cast<RMW_Connext_BuiltinListener *>(listener_data);

  if (nullptr != self->parent_listener.on_subscription_matched) {
    self->parent_listener.on_subscription_matched(
      self->parent_listener.as_listener.listener_data, reader, status);
  }
}

void
RMW_Connext_BuiltinListener::on_sample_lost(
  void * listener_data,
  DDS_DataReader * reader,
  const struct DDS_SampleLostStatus * status)
{
  RMW_Connext_BuiltinListener * const self =
    reinterpret_cast<RMW_Connext_BuiltinListener *>(listener_data);

  if (nullptr != self->parent_listener.on_sample_lost) {
    self->parent_listener.on_sample_lost(
      self->parent_listener.as_listener.listener_data, reader, status);
  }
}

void
RMW_Connext_BuiltinListener::on_instance_replaced(
  void * listener_data,
  DDS_DataReader * reader,
  const struct DDS_DataReaderInstanceReplacedStatus * status)
{
  RMW_Connext_BuiltinListener * const self =
    reinterpret_cast<RMW_Connext_BuiltinListener *>(listener_data);

  if (nullptr != self->parent_listener.on_instance_replaced) {
    self->parent_listener.on_instance_replaced(
      self->parent_listener.as_listener.listener_data, reader, status);
  }
}

void
RMW_Connext_BuiltinListener::on_data_available(
  void * listener_data, DDS_DataReader * reader)
{
  RMW_Connext_BuiltinListener * const self =
    reinterpret_cast<RMW_Connext_BuiltinListener *>(listener_data);

  self->process_data_available(reader);

  if (nullptr != self->parent_listener.on_data_available) {
    self->parent_listener.on_data_available(
      self->parent_listener.as_listener.listener_data, reader);
  }
}

void
RMW_Connext_BuiltinListener::initialize_listener(
  RMW_Connext_BuiltinListener * const b_listener,
  struct DDS_DataReaderListener * const listener)
{
  struct DDS_DataReaderListener DEFAULT_LISTENER = DDS_DataReaderListener_INITIALIZER;

  *listener = DEFAULT_LISTENER;
  listener->as_listener.listener_data = b_listener;
  listener->on_requested_deadline_missed =
    RMW_Connext_BuiltinListener::on_requested_deadline_missed;
  listener->on_requested_incompatible_qos =
    RMW_Connext_BuiltinListener::on_requested_incompatible_qos;
  listener->on_sample_rejected =
    RMW_Connext_BuiltinListener::on_sample_rejected;
  listener->on_liveliness_changed =
    RMW_Connext_BuiltinListener::on_liveliness_changed;
  listener->on_subscription_matched =
    RMW_Connext_BuiltinListener::on_subscription_matched;
  listener->on_sample_lost =
    RMW_Connext_BuiltinListener::on_sample_lost;
  listener->on_instance_replaced =
    RMW_Connext_BuiltinListener::on_instance_replaced;
  listener->on_before_sample_commit =
    RMW_Connext_BuiltinListener::on_before_sample_commit;
  listener->on_data_available =
    RMW_Connext_BuiltinListener::on_data_available;
}

rmw_ret_t
rmw_connextdds_dcps_participant_get_reader(
  rmw_context_impl_t * const ctx,
  DDS_DataReader ** const reader_out)
{
  rmw_connextdds_api_micro * const ctx_api = RMW_Connext_fv_FactoryContext;

  if (nullptr == ctx_api) {
    return RMW_RET_ERROR;
  }

  DDS_Subscriber * const sub =
    DDS_DomainParticipant_get_builtin_subscriber(ctx->participant);
  if (nullptr == sub) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to lookup builtin subscriber")
    return RMW_RET_ERROR;
  }
  DDS_DataReader * const reader =
    DDS_Subscriber_lookup_datareader(sub, DDS_PARTICIPANT_BUILTIN_TOPIC_NAME);
  if (nullptr == reader) {
    return RMW_RET_ERROR;
  }

  const DDS_DataReaderListener parent_listener =
    DDS_DataReader_get_listener(reader);

  ctx_api->discovery_dp_listener.set_parent_listener(parent_listener);

  DDS_DataReaderListener dr_listener = DDS_DataReaderListener_INITIALIZER;
  RMW_Connext_BuiltinListener::initialize_listener(
    &ctx_api->discovery_dp_listener, &dr_listener);

  if (DDS_RETCODE_OK !=
    DDS_DataReader_set_listener(reader, &dr_listener, DDS_STATUS_MASK_ALL))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to set DataReaderListener")
    return RMW_RET_ERROR;
  }

  *reader_out = reader;

  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_dcps_publication_get_reader(
  rmw_context_impl_t * const ctx,
  DDS_DataReader ** const reader_out)
{
  rmw_connextdds_api_micro * const ctx_api = RMW_Connext_fv_FactoryContext;

  if (nullptr == ctx_api) {
    return RMW_RET_ERROR;
  }

  DDS_Subscriber * const sub =
    DDS_DomainParticipant_get_builtin_subscriber(ctx->participant);
  if (nullptr == sub) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to lookup builtin subscriber")
    return RMW_RET_ERROR;
  }
  DDS_DataReader * const reader =
    DDS_Subscriber_lookup_datareader(sub, DDS_PUBLICATION_BUILTIN_TOPIC_NAME);
  if (nullptr == reader) {
    return RMW_RET_ERROR;
  }

  const DDS_DataReaderListener parent_listener =
    DDS_DataReader_get_listener(reader);

  ctx_api->discovery_pub_listener.set_parent_listener(parent_listener);

  DDS_DataReaderListener dr_listener = DDS_DataReaderListener_INITIALIZER;
  RMW_Connext_BuiltinListener::initialize_listener(
    &ctx_api->discovery_pub_listener, &dr_listener);

  if (DDS_RETCODE_OK !=
    DDS_DataReader_set_listener(reader, &dr_listener, DDS_STATUS_MASK_ALL))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to lookup set DataReaderListener")
    return RMW_RET_ERROR;
  }

  *reader_out = reader;

  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_dcps_subscription_get_reader(
  rmw_context_impl_t * const ctx,
  DDS_DataReader ** const reader_out)
{
  rmw_connextdds_api_micro * const ctx_api = RMW_Connext_fv_FactoryContext;

  if (nullptr == ctx_api) {
    return RMW_RET_ERROR;
  }

  DDS_Subscriber * const sub =
    DDS_DomainParticipant_get_builtin_subscriber(ctx->participant);
  if (nullptr == sub) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to lookup builtin subscriber")
    return RMW_RET_ERROR;
  }
  DDS_DataReader * const reader =
    DDS_Subscriber_lookup_datareader(sub, DDS_SUBSCRIPTION_BUILTIN_TOPIC_NAME);
  if (nullptr == reader) {
    return RMW_RET_ERROR;
  }

  const DDS_DataReaderListener parent_listener =
    DDS_DataReader_get_listener(reader);

  ctx_api->discovery_sub_listener.set_parent_listener(parent_listener);

  DDS_DataReaderListener dr_listener = DDS_DataReaderListener_INITIALIZER;
  RMW_Connext_BuiltinListener::initialize_listener(
    &ctx_api->discovery_sub_listener, &dr_listener);

  if (DDS_RETCODE_OK !=
    DDS_DataReader_set_listener(reader, &dr_listener, DDS_STATUS_MASK_ALL))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to set DataReaderListener")
    return RMW_RET_ERROR;
  }

  *reader_out = reader;

  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_enable_builtin_readers(rmw_context_impl_t * const ctx)
{
  UNUSED_ARG(ctx);
  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_dcps_participant_on_data(rmw_context_impl_t * const ctx)
{
  rmw_connextdds_api_micro * const ctx_api = RMW_Connext_fv_FactoryContext;

  if (nullptr == ctx_api) {
    return RMW_RET_ERROR;
  }

  RMW_Connext_CachedBuiltinData<DDS_ParticipantBuiltinTopicData> qdata;
  while (ctx_api->discovery_dp_listener.next_data(qdata)) {
    rmw_ret_t rc = RMW_RET_OK;

    if (qdata.valid_data) {
      if (RMW_RET_OK !=
        rmw_connextdds_graph_add_participant(ctx, &qdata.data, nullptr))
      {
        rc = RMW_RET_ERROR;
      }
      RMW_Connext_ParticipantData::finalize(&qdata.data);
    } else {
      if (qdata.instance_state == DDS_NOT_ALIVE_DISPOSED_INSTANCE_STATE ||
        qdata.instance_state == DDS_NOT_ALIVE_NO_WRITERS_INSTANCE_STATE)
      {
        if (RMW_RET_OK !=
          rmw_connextdds_graph_remove_participant(
            ctx, &qdata.instance_handle))
        {
          rc = RMW_RET_ERROR;
        }
      }
    }

    if (RMW_RET_OK != rc) {
      return rc;
    }
  }

  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_dcps_publication_on_data(rmw_context_impl_t * const ctx)
{
  rmw_connextdds_api_micro * const ctx_api = RMW_Connext_fv_FactoryContext;

  if (nullptr == ctx_api) {
    return RMW_RET_ERROR;
  }

  RMW_Connext_CachedBuiltinData<DDS_PublicationBuiltinTopicData> qdata;
  while (ctx_api->discovery_pub_listener.next_data(qdata)) {
    rmw_ret_t rc = RMW_RET_OK;

    if (qdata.valid_data) {
      DDS_GUID_t endp_guid;
      DDS_GUID_t dp_guid;

      DDS_GUID_from_rtps(&endp_guid, reinterpret_cast<RTPS_Guid *>(&qdata.data.key));
      DDS_GUID_from_rtps(
        &dp_guid, reinterpret_cast<RTPS_Guid *>(&qdata.data.participant_key));

      // Ignore return code since we can't do anything about a failure
      // in this listener.
      rmw_connextdds_graph_add_remote_entity(
        ctx,
        &endp_guid,
        &dp_guid,
        qdata.data.topic_name,
        qdata.data.type_name,
        &qdata.data.reliability,
        &qdata.data.durability,
        &qdata.data.deadline,
        &qdata.data.liveliness,
        nullptr /* Micro doesn't support DDS_LifespanQosPolicy */,
        false /* is_reader */);

      RMW_Connext_PublicationData::finalize(&qdata.data);
    } else {
      if (qdata.instance_state == DDS_NOT_ALIVE_DISPOSED_INSTANCE_STATE ||
        qdata.instance_state == DDS_NOT_ALIVE_NO_WRITERS_INSTANCE_STATE)
      {
        if (RMW_RET_OK !=
          rmw_connextdds_graph_remove_entity(
            ctx, &qdata.instance_handle, false /* is_reader */))
        {
          rc = RMW_RET_ERROR;
        }
      }
    }

    if (RMW_RET_OK != rc) {
      return rc;
    }
  }

  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_dcps_subscription_on_data(rmw_context_impl_t * const ctx)
{
  rmw_connextdds_api_micro * const ctx_api = RMW_Connext_fv_FactoryContext;

  if (nullptr == ctx_api) {
    return RMW_RET_ERROR;
  }

  RMW_Connext_CachedBuiltinData<DDS_SubscriptionBuiltinTopicData> qdata;
  while (ctx_api->discovery_sub_listener.next_data(qdata)) {
    rmw_ret_t rc = RMW_RET_OK;

    if (qdata.valid_data) {
      DDS_GUID_t endp_guid;
      DDS_GUID_t dp_guid;

      DDS_GUID_from_rtps(&endp_guid, reinterpret_cast<RTPS_Guid *>(&qdata.data.key));
      DDS_GUID_from_rtps(
        &dp_guid, reinterpret_cast<RTPS_Guid *>(&qdata.data.participant_key));

      // Ignore return code since we can't do anything about a failure
      // in this listener.
      rmw_connextdds_graph_add_remote_entity(
        ctx,
        &endp_guid,
        &dp_guid,
        qdata.data.topic_name,
        qdata.data.type_name,
        &qdata.data.reliability,
        &qdata.data.durability,
        &qdata.data.deadline,
        &qdata.data.liveliness,
        nullptr /* Lifespan is a writer-only qos policy */,
        true /* is_reader */);

      RMW_Connext_SubscriptionData::finalize(&qdata.data);
    } else {
      if (qdata.instance_state == DDS_NOT_ALIVE_DISPOSED_INSTANCE_STATE ||
        qdata.instance_state == DDS_NOT_ALIVE_NO_WRITERS_INSTANCE_STATE)
      {
        if (RMW_RET_OK !=
          rmw_connextdds_graph_remove_entity(
            ctx, &qdata.instance_handle, true /* is_reader */))
        {
          rc = RMW_RET_ERROR;
        }
      }
    }

    if (RMW_RET_OK != rc) {
      return rc;
    }
  }
  return RMW_RET_OK;
}

void
rmw_connextdds_ih_to_gid(
  const DDS_InstanceHandle_t & ih, rmw_gid_t & gid)
{
  static_assert(
    RMW_GID_STORAGE_SIZE >= 16,
    "rmw_gid_t type too small for an RTI Connext DDS Micro GUID");

  memset(&gid, 0, sizeof(gid));
  gid.implementation_identifier = RMW_CONNEXTDDS_ID;
  memcpy(gid.data, ih.octet, 16);
}

static
DDS_Boolean
RMW_Connext_DataReaderListener_before_sample_commit_drop_local(
  void * listener_data,
  DDS_DataReader * reader,
  const void * const sample,
  const struct DDS_SampleInfo * const sample_info,
  DDS_Boolean * dropped)
{
  UNUSED_ARG(reader);
  UNUSED_ARG(sample);

  RMW_Connext_SubscriberStatusCondition * const self =
    reinterpret_cast<RMW_Connext_SubscriberStatusCondition *>(listener_data);

  *dropped = memcmp(
    self->participant_handle.octet,
    sample_info->publication_handle.octet,
    12) == 0 ?
    DDS_BOOLEAN_TRUE : DDS_BOOLEAN_FALSE;

  return DDS_BOOLEAN_TRUE;
}

void
rmw_connextdds_configure_subscriber_condition_listener(
  RMW_Connext_SubscriberStatusCondition * cond,
  DDS_DataReaderListener * const listener,
  DDS_StatusMask * const listener_mask)
{
  UNUSED_ARG(cond);
  UNUSED_ARG(listener_mask);
  if (cond->ignore_local) {
    listener->on_before_sample_commit =
      RMW_Connext_DataReaderListener_before_sample_commit_drop_local;
  }
}


void
rmw_connextdds_builtinkey_to_guid(
  const DDS_BuiltinTopicKey_t * const self,
  DDS_GUID_t * const dst)
{
  RTPS_Guid guid = RTPS_GUID_UNKNOWN;

  guid.prefix.host_id = self->value[0];
  guid.prefix.app_id = self->value[1];
  guid.prefix.instance_id = self->value[2];
  guid.object_id = self->value[3];

  DDS_GUID_from_rtps(dst, &guid);
}

rmw_ret_t
rmw_connextdds_enable_security(
  rmw_context_impl_t * const ctx,
  DDS_DomainParticipantQos * const qos)
{
#if RMW_CONNEXT_ENABLE_SECURITY
  // TODO(asorbini) Don't try to initialize security plugins more than once.

  RT_Registry_T * registry =
    DDS_DomainParticipantFactory_get_registry(
    RMW_Connext_gv_DomainParticipantFactory);
  if (nullptr == registry) {
    RMW_CONNEXT_LOG_ERROR_SET("failed to get factory registry")
    return RMW_RET_ERROR;
  }

  // Register security plugin suite with DomainParticipantFactory
  struct SECCORE_SecurePluginFactoryProperty sec_plugin_prop =
    SECCORE_SecurePluginFactoryProperty_INITIALIZER;

  if (!SECCORE_SecurePluginFactory_register(
      registry, SECCORE_DEFAULT_SUITE_NAME, &sec_plugin_prop))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to register DDS Security plugins")
    return RMW_RET_ERROR;
  }

  // In order to enable security, the name used to register the suite of
  // plugins must be set in DomainParticipantQos
  if (!RT_ComponentFactoryId_set_name(
      &qos->trust.suite, SECCORE_DEFAULT_SUITE_NAME))
  {
    RMW_CONNEXT_LOG_ERROR_A_SET(
      "failed component name: %s", SECCORE_DEFAULT_SUITE_NAME)
    return RMW_RET_ERROR;
  }

  return RMW_RET_OK;
#else
  UNUSED_ARG(ctx);
  UNUSED_ARG(qos);
  RMW_CONNEXT_LOG_ERROR_A_SET(
    "DDS Security not available. "
    "Please rebuild %s with -DRMW_CONNEXT_ENABLE_SECURITY",
    RMW_CONNEXTDDS_ID)
  return RMW_RET_ERROR;
#endif /* RMW_CONNEXT_ENABLE_SECURITY */
}

rmw_ret_t
rmw_connextdds_set_cft_filter_expression(
  DDS_TopicDescription * const topic_desc,
  const char * const cft_expression,
  const rcutils_string_array_t * const cft_expression_parameters)
{
  UNUSED_ARG(topic_desc);
  UNUSED_ARG(cft_expression);
  UNUSED_ARG(cft_expression_parameters);
  return RMW_RET_UNSUPPORTED;
}

rmw_ret_t
rmw_connextdds_get_cft_filter_expression(
  DDS_TopicDescription * const topic_desc,
  rcutils_allocator_t * const allocator,
  rmw_subscription_content_filter_options_t * const options)
{
  UNUSED_ARG(topic_desc);
  UNUSED_ARG(allocator);
  UNUSED_ARG(options);
  return RMW_RET_UNSUPPORTED;
}
