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

/******************************************************************************
 * Connext Micro TypePlugin
 ******************************************************************************/

extern struct DDS_TypePluginI RMW_Connext_fv_TypePluginI;

struct RMW_Connext_RtimeTypePluginI
{
  struct DDS_TypePluginI base;
  RMW_Connext_MessageTypeSupport * _type_support;

  RMW_Connext_RtimeTypePluginI(
    RMW_Connext_MessageTypeSupport * const type_support)
  {
    this->base = RMW_Connext_fv_TypePluginI;
    this->_type_support = type_support;
  }

  static
  RMW_Connext_MessageTypeSupport *
  type_support(void * const self)
  {
    DDS_TypePluginDefault * const plugin =
      reinterpret_cast<DDS_TypePluginDefault *>(self);

    const RMW_Connext_RtimeTypePluginI * const intf =
      reinterpret_cast<const RMW_Connext_RtimeTypePluginI *>(plugin->_parent._intf);

    return intf->_type_support;
  }
};


// Typed samples management

static
RTI_BOOL
RMW_Connext_MemoryPlugin_create_sample(
  struct DDS_TypePlugin * tp,
  void ** sample)
{
  auto type_support = RMW_Connext_RtimeTypePluginI::type_support(tp);

  size_t buffer_size = 0;
  if (type_support->unbounded()) {
    buffer_size = 0;
  } else {
    buffer_size = type_support->type_serialized_size_max();
  }

  RMW_Connext_Message * const msg = new (std::nothrow) RMW_Connext_Message();
  if (nullptr == msg) {
    return RTI_FALSE;
  }
  if (RMW_RET_OK != RMW_Connext_Message_initialize(msg, type_support, buffer_size)) {
    delete msg;
    return RTI_FALSE;
  }

  *sample = msg;

  return RTI_TRUE;
}

static
RTI_BOOL
RMW_Connext_MemoryPlugin_delete_sample(
  struct DDS_TypePlugin * plugin,
  void * sample)
{
  UNUSED_ARG(plugin);

  RMW_Connext_Message * const msg =
    reinterpret_cast<RMW_Connext_Message *>(sample);
  RMW_Connext_Message_finalize(msg);
  delete msg;

  return RTI_TRUE;
}

static
RTI_BOOL
RMW_Connext_MemoryPlugin_copy_sample(
  struct DDS_TypePlugin * plugin,
  void * dst,
  const void * src)
{
  const RMW_Connext_Message * src_msg =
    reinterpret_cast<const RMW_Connext_Message *>(src);
  RMW_Connext_Message * dst_msg =
    reinterpret_cast<RMW_Connext_Message *>(dst);

  UNUSED_ARG(plugin);

  if (RCUTILS_RET_OK !=
    rcutils_uint8_array_copy(&dst_msg->data_buffer, &src_msg->data_buffer))
  {
    return RTI_FALSE;
  }

  return RTI_TRUE;
}

static
RTI_BOOL
RMW_Connext_EncapsulationPlugin_serialize(
  struct DDS_TypePlugin * plugin,
  struct CDR_Stream_t * stream,
  const void * void_sample,
  DDS_InstanceHandle_t * destination)
{
  const RMW_Connext_Message * const msg =
    reinterpret_cast<const RMW_Connext_Message *>(void_sample);
  auto type_support = msg->type_support;
  UNUSED_ARG(plugin);
  UNUSED_ARG(destination);

  if (nullptr == msg->user_data) {
    // Samples written by the application layer should always have a non-null
    // user_data pointer. The only samples which do not use that pointers are
    // the ones created internally in the DataReader queue, and it would be
    // unexpected for them to be passed to this function.
    return RTI_FALSE;
  }

  DDS_TypePluginBuffer * const tbuf =
    reinterpret_cast<DDS_TypePluginBuffer *>(stream->real_buff);

  rcutils_uint8_array_t data_buffer;

  /* the following pointers are cached here for convenience,
     but they are only accessed when appropriate */
  const rcutils_uint8_array_t * const user_buffer =
    reinterpret_cast<const rcutils_uint8_array_t *>(msg->user_data);
  rcutils_uint8_array_t * const msg_buffer_unbound =
    reinterpret_cast<rcutils_uint8_array_t *>(&tbuf[1]);
  uint8_t * const msg_buffer_bound = reinterpret_cast<uint8_t *>(&tbuf[1]);

  size_t serialized_size = 0;

  if (!msg->serialized) {
    if (type_support->unbounded()) {
      serialized_size =
        type_support->serialized_size_max(msg->user_data);
    } else {
      serialized_size =
        type_support->type_serialized_size_max();
    }
  } else {
    serialized_size = user_buffer->buffer_length;

    if (!type_support->unbounded() &&
      serialized_size >
      type_support->type_serialized_size_max())
    {
      return RTI_FALSE;
    }
  }

  if (type_support->unbounded()) {
    if (msg_buffer_unbound->buffer_capacity < serialized_size) {
      if (RCUTILS_RET_OK !=
        rcutils_uint8_array_resize(msg_buffer_unbound, serialized_size))
      {
        return RTI_FALSE;
      }
      memset(msg_buffer_unbound->buffer, 0, msg_buffer_unbound->buffer_capacity);
    }
    msg_buffer_unbound->buffer_length = 0;

    data_buffer = *msg_buffer_unbound;
  } else {
    data_buffer.buffer = msg_buffer_bound;
    data_buffer.buffer_capacity = type_support->type_serialized_size_max();
    data_buffer.buffer_length = 0;
  }

  if (!msg->serialized) {
    rmw_ret_t rc = type_support->serialize(msg->user_data, &data_buffer);
    if (RMW_RET_OK != rc) {
      return RTI_FALSE;
    }
  } else {
    if (RCUTILS_RET_OK !=
      rcutils_uint8_array_copy(&data_buffer, user_buffer))
    {
      return RTI_FALSE;
    }
  }

  tbuf->data_pbuf.buffer = reinterpret_cast<char *>(data_buffer.buffer);
  tbuf->data_pbuf.max_length = data_buffer.buffer_capacity;

  if (!CDR_Stream_set_buffer(
      stream,
      reinterpret_cast<char *>(data_buffer.buffer),
      data_buffer.buffer_capacity))
  {
    return RTI_FALSE;
  }

  if (!CDR_Stream_set_current_position_offset(
      stream,
      data_buffer.buffer_length))
  {
    return RTI_FALSE;
  }

  return RTI_TRUE;
}

static
RTI_BOOL
RMW_Connext_EncapsulationPlugin_deserialize(
  struct DDS_TypePlugin * plugin,
  void * void_sample,
  struct CDR_Stream_t * stream,
  DDS_InstanceHandle_t * source)
{
  UNUSED_ARG(source);
  UNUSED_ARG(plugin);

  RMW_Connext_Message * const msg =
    reinterpret_cast<RMW_Connext_Message *>(void_sample);
  const size_t deserialize_size =
    stream->length - CDR_Stream_get_current_position_offset(stream) +
    RMW_Connext_MessageTypeSupport::ENCAPSULATION_HEADER_SIZE;

  if (msg->data_buffer.buffer_capacity < deserialize_size) {
    if (RCUTILS_RET_OK !=
      rcutils_uint8_array_resize(&msg->data_buffer, deserialize_size))
    {
      return RTI_FALSE;
    }
  }

  void * src_ptr =
    CDR_Stream_get_current_position_ptr(stream) -
    RMW_Connext_MessageTypeSupport::ENCAPSULATION_HEADER_SIZE;

  memcpy(
    msg->data_buffer.buffer,
    src_ptr,
    deserialize_size);

  msg->data_buffer.buffer_length = deserialize_size;

  return RTI_TRUE;
}

static
RTI_BOOL
RMW_Connext_MemoryPlugin_initialize_sample(
  void * init_config,
  void * buffer)
{
  struct DDS_TypePluginDefault * plugin =
    (struct DDS_TypePluginDefault *)init_config;
  void * sample = NULL;
  struct DDS_TypePluginSampleHolder * sh =
    (struct DDS_TypePluginSampleHolder *)buffer;

  if (!RMW_Connext_MemoryPlugin_create_sample(
      &plugin->_parent, &sample))
  {
    return RTI_FALSE;
  }

  sh->sample = sample;

  return RTI_TRUE;
}

static
RTI_BOOL
RMW_Connext_MemoryPlugin_finalize_sample(
  void * finalize_config, void * buffer)
{
  struct DDS_TypePluginDefault * plugin =
    (struct DDS_TypePluginDefault *)finalize_config;
  struct DDS_TypePluginSampleHolder * sh =
    (struct DDS_TypePluginSampleHolder *)buffer;

  return RMW_Connext_MemoryPlugin_delete_sample(
    &plugin->_parent, sh->sample);
}

static
struct DDS_TypeMemoryPlugin *
RMW_Connext_MemoryPlugin_create(
  struct DDS_TypePlugin * tp,
  DDS_DomainParticipant * participant,
  struct DDS_DomainParticipantQos * dp_qos,
  DDS_TypePluginMode_T endpoint_mode,
  DDS_TypePluginEndpoint * endpoint,
  DDS_TypePluginEndpointQos * qos)
{
  struct DDS_TypePluginDefault * plugin = (struct DDS_TypePluginDefault *)tp;
  struct REDA_BufferPoolProperty bufp = REDA_BufferPoolProperty_INITIALIZER;
  struct DDS_DataReaderQos * dr_qos = NULL;

  UNUSED_ARG(participant);
  UNUSED_ARG(dp_qos);
  UNUSED_ARG(endpoint);

  if (endpoint_mode == DDS_TYPEPLUGIN_MODE_READER) {
    dr_qos = (struct DDS_DataReaderQos *)qos;
    if (tp->property.max_buffers == 0) {
      /* max_samples cannot be negative */
      bufp.max_buffers = (RTI_SIZE_T)dr_qos->resource_limits.max_samples;
    } else {
      bufp.max_buffers = (RTI_SIZE_T)tp->property.max_buffers;
    }

    bufp.buffer_size = sizeof(struct DDS_TypePluginSampleHolder);

    plugin->pool = REDA_BufferPool_new(
      "cdr_samples",
      &bufp,
      RMW_Connext_MemoryPlugin_initialize_sample,
      tp,
      RMW_Connext_MemoryPlugin_finalize_sample,
      tp);
  }

  return &plugin->heap_plugin._parent;
}

static
void
RMW_Connext_MemoryPlugin_delete(
  struct DDS_TypePlugin * plugin,
  struct DDS_TypeMemoryPlugin * mem_plugin)
{
  UNUSED_ARG(plugin);
  UNUSED_ARG(mem_plugin);

  /* plugin->pool will be deleted by DDS_TypePluginDefault_delete */
}

static
RTI_UINT32
RMW_Connext_EncapsulationPlugin_get_serialized_sample_size(
  struct DDS_TypePlugin * plugin,
  struct DDS_TypeEncapsulationPlugin * ep,
  RTI_UINT32 current_alignment)
{
  UNUSED_ARG(ep);
  auto type_support = RMW_Connext_RtimeTypePluginI::type_support(plugin);

  RTI_UINT32 tot_alignment = current_alignment;

  // For unbounded types this call will only report the size of the
  // "bounded" part of the type.
  tot_alignment += type_support->type_serialized_size_max();

  return tot_alignment - current_alignment;
}

static
RTI_BOOL
RMW_Connext_EncapsulationPlugin_initialize_buffer(
  void * initialize_param, void * buffer)
{
  struct DDS_TypePluginDefault * plugin =
    reinterpret_cast<struct DDS_TypePluginDefault *>(initialize_param);
  auto type_support = RMW_Connext_RtimeTypePluginI::type_support(initialize_param);
  DDS_TypePluginBuffer * const tbuf = reinterpret_cast<DDS_TypePluginBuffer *>(buffer);

  UNUSED_ARG(plugin);

  if (type_support->unbounded()) {
    rcutils_uint8_array_t * const data_buffer =
      reinterpret_cast<rcutils_uint8_array_t *>(&tbuf[1]);
    const rcutils_allocator_t allocator = rcutils_get_default_allocator();

    if (RCUTILS_RET_OK !=
      rcutils_uint8_array_init(data_buffer, 0, &allocator))
    {
      return RTI_FALSE;
    }
  }

  return RTI_TRUE;
}

static
RTI_BOOL
RMW_Connext_EncapsulationPlugin_finalize_buffer(
  void * finalize_param, void * buffer)
{
  struct DDS_TypePluginDefault * plugin =
    reinterpret_cast<struct DDS_TypePluginDefault *>(finalize_param);
  auto type_support = RMW_Connext_RtimeTypePluginI::type_support(finalize_param);
  DDS_TypePluginBuffer * const tbuf = reinterpret_cast<DDS_TypePluginBuffer *>(buffer);

  UNUSED_ARG(plugin);

  if (type_support->unbounded()) {
    rcutils_uint8_array_t * const data_buffer =
      reinterpret_cast<rcutils_uint8_array_t *>(&tbuf[1]);

    if (RCUTILS_RET_OK != rcutils_uint8_array_fini(data_buffer)) {
      return RTI_FALSE;
    }
  }

  return RTI_TRUE;
}

static
struct DDS_TypeEncapsulationPlugin *
RMW_Connext_EncapsulationPlugin_create(
  struct DDS_TypePlugin * tp,
  DDS_DomainParticipant * participant,
  struct DDS_DomainParticipantQos * dp_qos,
  DDS_TypePluginMode_T endpoint_mode,
  DDS_TypePluginEndpoint * endpoint,
  DDS_TypePluginEndpointQos * qos,
  struct DDS_TypeMemoryPlugin * mp)
{
  struct DDS_TypePluginDefault * plugin = (struct DDS_TypePluginDefault *)tp;
  auto type_support = RMW_Connext_RtimeTypePluginI::type_support(tp);

  UNUSED_ARG(participant);
  UNUSED_ARG(dp_qos);
  UNUSED_ARG(endpoint);

  if (endpoint_mode == DDS_TYPEPLUGIN_MODE_WRITER) {
    struct REDA_BufferPoolProperty bufp =
      REDA_BufferPoolProperty_INITIALIZER;
    struct DDS_DataWriterQos * const dw_qos =
      (struct DDS_DataWriterQos *)qos;
    size_t serialized_size = 0;

    /* If the type is not unbounded, then we can just allocate a pool of
       buffers of size serialized_size_max. Otherwise, we allocate a pool
       of rcutils_uint8_array_t which will be dynamically allocated by
       the serialized function as needed. */
    if (type_support->unbounded()) {
      serialized_size = sizeof(rcutils_uint8_array_t);
    } else {
      serialized_size = type_support->type_serialized_size_max();
    }

    bufp.buffer_size = sizeof(struct DDS_TypePluginBuffer) +
      serialized_size;

    if (tp->property.max_buffers == 0) {
      bufp.max_buffers = dw_qos->resource_limits.max_samples;
    } else {
      bufp.max_buffers = tp->property.max_buffers;
    }

    plugin->pool = REDA_BufferPool_new(
      "cdr_buffers",
      &bufp,
      RMW_Connext_EncapsulationPlugin_initialize_buffer,
      tp,
      RMW_Connext_EncapsulationPlugin_finalize_buffer,
      tp);

    if (nullptr == plugin->pool) {
      return nullptr;
    }

    plugin->cdr_plugin.size = serialized_size;
  }

  plugin->cdr_plugin._parent.memory_plugin = mp;

  return &plugin->cdr_plugin._parent;
}


RTI_PRIVATE void
RMW_Connext_EncapsulationPlugin_delete(
  struct DDS_TypePlugin * p,
  struct DDS_TypeEncapsulationPlugin * ep)
{
  UNUSED_ARG(p);
  UNUSED_ARG(ep);

  /* plugin->pool will be deleted by DDS_TypePluginDefault_delete */
}

static
NDDSCDREncapsulation RMW_Connext_fv_EncapsulationKind[] =
{
  {
    DDS_ENCAPSULATION_ID_CDR_LE,
    DDS_ENCAPSULATION_ID_CDR_BE,
    0
  }
};

static
void *
RMW_Connext_EncapsulationPlugin_get_buffer(struct DDS_TypePlugin * tp)
{
  struct DDS_TypePluginDefault * plugin = (struct DDS_TypePluginDefault *)tp;

  struct DDS_TypePluginBuffer * tbuf =
    (struct DDS_TypePluginBuffer *)REDA_BufferPool_get_buffer(plugin->pool);

  if (tbuf == NULL) {
    return nullptr;
  }

  tbuf->wp = &plugin->cdr_plugin._parent;
  tbuf->data = nullptr;
  tbuf->data_pbuf._next = nullptr;
  tbuf->data_pbuf.buffer = reinterpret_cast<char *>(tbuf);
  tbuf->data_pbuf.head_pos = 0;
  tbuf->data_pbuf.tail_pos = 0;
  tbuf->data_pbuf.max_length = sizeof(struct DDS_TypePluginBuffer) +
    plugin->cdr_plugin.size;

  return tbuf;
}

static
void
RMW_Connext_EncapsulationPlugin_return_buffer(
  struct DDS_TypePlugin * tp,
  void * buffer)
{
  struct DDS_TypePluginDefault * plugin = (struct DDS_TypePluginDefault *)tp;
  REDA_BufferPool_return_buffer(plugin->pool, buffer);
}

static
struct DDS_TypePluginSampleHolder *
RMW_Connext_EncapsulationPlugin_get_sample(
  struct DDS_TypePlugin * tp, struct CDR_Stream_t * stream)
{
  struct DDS_TypePluginDefault * plugin = (struct DDS_TypePluginDefault *)tp;
  struct DDS_TypePluginSampleHolder * sh;

  UNUSED_ARG(stream);

  sh = (struct DDS_TypePluginSampleHolder *)
    REDA_BufferPool_get_buffer(plugin->pool);
  if (sh == NULL) {
    return NULL;
  }

  sh->owner = &plugin->cdr_plugin._parent;

  return sh;
}

static
void
RMW_Connext_EncapsulationPlugin_return_sample(
  struct DDS_TypePlugin * tp, struct DDS_TypePluginSampleHolder * sample)
{
  struct DDS_TypePluginDefault * plugin =
    reinterpret_cast<struct DDS_TypePluginDefault *>(tp);
  REDA_BufferPool_return_buffer(plugin->pool, sample);
}

static
struct DDS_TypeEncapsulationI RMW_Connext_fv_EncapsulationPluginI =
{
  DDS_XCDR_DATA_REPRESENTATION,
  NULL,
  RMW_Connext_fv_EncapsulationKind,
  RTI_MEMORY_TYPE_HEAP,
  RTI_MEMORY_MANAGER_HEAP,
  NULL,
  NULL,
  RMW_Connext_EncapsulationPlugin_get_buffer,
  RMW_Connext_EncapsulationPlugin_return_buffer,
  RMW_Connext_EncapsulationPlugin_get_sample,
  RMW_Connext_EncapsulationPlugin_return_sample,
  RMW_Connext_EncapsulationPlugin_serialize,
  RMW_Connext_EncapsulationPlugin_deserialize,
  RMW_Connext_EncapsulationPlugin_get_serialized_sample_size,
  RMW_Connext_EncapsulationPlugin_create,
  RMW_Connext_EncapsulationPlugin_delete
};

static
struct DDS_TypeEncapsulationI * RMW_Connext_fv_WirePlugins[] =
{
  &RMW_Connext_fv_EncapsulationPluginI,
  NULL
};

static
struct DDS_TypeMemoryI RMW_Connext_fv_MemoryPluginI =
{
  RTI_MEMORY_MANAGER_HEAP,
  RTI_MEMORY_TYPE_HEAP,
  RMW_Connext_MemoryPlugin_create_sample,
  RMW_Connext_MemoryPlugin_delete_sample,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  RMW_Connext_MemoryPlugin_create,
  RMW_Connext_MemoryPlugin_delete
};

static
struct DDS_TypeMemoryI * RMW_Connext_fv_MemoryPlugins[] =
{
  &RMW_Connext_fv_MemoryPluginI,
  NULL
};

static
struct DDS_TypePlugin *
RMW_Connext_TypePlugin_create(
  DDS_DomainParticipant * participant,
  struct DDS_DomainParticipantQos * dp_qos,
  DDS_TypePluginMode_T endpoint_mode,
  DDS_TypePluginEndpoint * endpoint,
  DDS_TypePluginEndpointQos * qos,
  struct DDS_TypePluginProperty * const property);

static
RTI_BOOL
RMW_Connext_TypePlugin_delete(struct DDS_TypePlugin * plugin);

struct DDS_TypePluginI RMW_Connext_fv_TypePluginI =
{
  NULL,                         /* DDS_TypeCode_t* */
  NDDS_TYPEPLUGIN_NO_KEY,       /* NDDS_TypePluginKeyKind */
  NDDS_TYPEPLUGIN_EH_LOCATION_SAMPLE,
  NULL,
  RTI_MEMORY_TYPE_HEAP,
  NULL,   /* instance to keyhash */
  RMW_Connext_MemoryPlugin_copy_sample,
  NULL,   /* initialize_sample */
  NULL,   /* serialize key */
  NULL,   /* deserialize key */
  NULL,   /* get serialized key size */
  NULL,   /* add peer */
  NULL,   /* remove peer */
  NULL,   /* serialize inline qos */
  NULL,   /* deserialize inline qos */
  NULL,   /* is sample consistent */
  RMW_Connext_fv_MemoryPlugins,   /* memory plugins */
  RMW_Connext_fv_WirePlugins,   /* wire plugins */
  NULL,   /* create typed datawriter */
  NULL,   /* delete typed datawriter */
  NULL,   /* create typed datareader */
  NULL,   /* delete typed datareader */
  RMW_Connext_TypePlugin_create,
  RMW_Connext_TypePlugin_delete,
  NULL,   /* on type registered */
  NULL   /* on type unregistered */
};

static
const char *
RMW_Connext_get_endpoint_type_name(
  DDS_TypePluginMode_T endpoint_mode,
  DDS_TypePluginEndpoint * endpoint)
{
  DDS_TopicDescription * topic_d = nullptr;

  switch (endpoint_mode) {
    case DDS_TYPEPLUGIN_MODE_READER:
      {
        DDS_DataReader * const reader =
          reinterpret_cast<DDS_DataReader *>(endpoint);
        topic_d = DDS_DataReader_get_topicdescription(reader);
        break;
      }
    case DDS_TYPEPLUGIN_MODE_WRITER:
      {
        DDS_DataWriter * const writer =
          reinterpret_cast<DDS_DataWriter *>(endpoint);
        topic_d =
          DDS_Topic_as_topicdescription(DDS_DataWriter_get_topic(writer));
        break;
      }
    default:
      /* should never get here */
      break;
  }

  return DDS_TopicDescription_get_type_name(topic_d);
}

static
struct DDS_TypePlugin *
RMW_Connext_TypePlugin_create(
  DDS_DomainParticipant * participant,
  struct DDS_DomainParticipantQos * dp_qos,
  DDS_TypePluginMode_T endpoint_mode,
  DDS_TypePluginEndpoint * endpoint,
  DDS_TypePluginEndpointQos * qos,
  struct DDS_TypePluginProperty * const property)
{
  const char * const type_name =
    RMW_Connext_get_endpoint_type_name(endpoint_mode, endpoint);
  if (nullptr == type_name) {
    return nullptr;
  }
  DDS_TypePluginI * type_plugin_intfI = nullptr;
  DDS_ReturnCode_t rc = DDS_DomainParticipant_lookup_type_pluginI(
    participant, type_name, &type_plugin_intfI);
  if (DDS_RETCODE_OK != rc || nullptr == type_plugin_intfI) {
    return nullptr;
  }

  RMW_Connext_RtimeTypePluginI * type_plugin_intf =
    reinterpret_cast<RMW_Connext_RtimeTypePluginI *>(type_plugin_intfI);

  struct DDS_TypePluginDefault * base_tp =
    reinterpret_cast<struct DDS_TypePluginDefault *>(
    DDS_TypePluginDefault_create(
      &type_plugin_intf->base,
      participant,
      dp_qos,
      endpoint_mode,
      endpoint,
      qos,
      property));

  if (nullptr == base_tp) {
    return nullptr;
  }

  return &base_tp->_parent;
}

static
RTI_BOOL
RMW_Connext_TypePlugin_delete(struct DDS_TypePlugin * plugin)
{
  return DDS_TypePluginDefault_delete(plugin);
}

rmw_ret_t
rmw_connextdds_assert_type(
  DDS_DomainParticipant * const participant,
  const char * const type_name,
  DDS_TypePluginI * const new_intf,
  DDS_TypePluginI ** const reg_intf,
  bool & registered)
{
  DDS_TypePluginI * existing_type = nullptr;

  registered = false;

  if (DDS_RETCODE_OK !=
    DDS_DomainParticipant_lookup_type_pluginI(
      participant, type_name, &existing_type))
  {
    return RMW_RET_ERROR;
  }

  if (nullptr == existing_type) {
    if (DDS_RETCODE_OK !=
      DDS_DomainParticipant_register_type(
        participant, type_name, new_intf))
    {
      return RMW_RET_ERROR;
    }

    registered = true;
    *reg_intf = new_intf;

    RMW_CONNEXT_LOG_DEBUG_A("registered type: name=%s", type_name)
  } else {
    *reg_intf = existing_type;
    RMW_CONNEXT_LOG_DEBUG_A("already registered type: name=%s", type_name)
  }


  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_delete_type_if_unused(
  DDS_DomainParticipant * const participant,
  const char * const type_name,
  DDS_TypePluginI ** const reg_intf_out)
{
  DDS_Boolean in_use = DDS_BOOLEAN_FALSE;

  *reg_intf_out = nullptr;

  RMW_CONNEXT_LOG_DEBUG_A("check if type in use: %s", type_name)

  if (DDS_RETCODE_OK !=
    DDS_DomainParticipant_is_type_in_use(
      participant, type_name, &in_use))
  {
    RMW_CONNEXT_LOG_ERROR_SET("failed to check type status")
    return RMW_RET_ERROR;
  }

  if (!in_use) {
    DDS_TypePluginI * const reg_intf =
      DDS_DomainParticipant_unregister_type(participant, type_name);

    if (nullptr == reg_intf) {
      RMW_CONNEXT_LOG_ERROR_SET("failed to unregister type")
      return RMW_RET_ERROR;
    }

    *reg_intf_out = reg_intf;
    RMW_CONNEXT_LOG_DEBUG_A("unregistered type: name=%s", type_name)
  }

  return RMW_RET_OK;
}


rmw_ret_t
rmw_connextdds_register_type_support(
  rmw_context_impl_t * const ctx,
  const rosidl_message_type_support_t * const type_supports,
  DDS_DomainParticipant * const participant,
  const RMW_Connext_MessageType message_type,
  const void * const intro_members,
  const bool intro_members_cpp,
  const char * const type_name)
{
  UNUSED_ARG(intro_members);
  UNUSED_ARG(intro_members_cpp);

  RMW_Connext_MessageTypeSupport * type_support = nullptr;
  try {
    type_support = new RMW_Connext_MessageTypeSupport(
      message_type, type_supports, type_name, ctx);
  } catch (const std::exception & e) {
    RMW_CONNEXT_LOG_ERROR_A_SET("failed to create type support: %s", e.what())
  }

  if (nullptr == type_support) {
    return RMW_RET_ERROR;
  }

  auto scope_exit_intf_delete_ts =
    rcpputils::make_scope_exit(
    [type_support]()
    {
      delete type_support;
    });

  RMW_Connext_RtimeTypePluginI * type_plugin_intf =
    new (std::nothrow) RMW_Connext_RtimeTypePluginI(type_support);

  if (nullptr == type_plugin_intf) {
    return RMW_RET_ERROR;
  }

  auto scope_exit_intf_delete =
    rcpputils::make_scope_exit(
    [type_plugin_intf]()
    {
      delete type_plugin_intf;
    });

  DDS_TypePluginI * reg_intf = nullptr;
  bool registered = false;

  if (RMW_RET_OK !=
    rmw_connextdds_assert_type(
      participant,
      type_support->type_name(),
      &type_plugin_intf->base,
      &reg_intf,
      registered))
  {
    return RMW_RET_ERROR;
  }

  if (registered) {
    scope_exit_intf_delete.cancel();
    scope_exit_intf_delete_ts.cancel();
    // Cache type support wrapper so that we may delete it later,
    // after deregistration.
    // TODO(asorbini) add assertion for (nullptr == ctx->registered_types[tname])
    std::string tname = type_support->type_name();
    ctx->registered_types[tname] = type_support;
  }

  return RMW_RET_OK;
}

rmw_ret_t
rmw_connextdds_unregister_type_support(
  rmw_context_impl_t * const ctx,
  DDS_DomainParticipant * const participant,
  const char * const type_name)
{
  /* Cache type_name into a string, since may be deallocated */
  std::string tname(type_name);

  DDS_TypePluginI * reg_intf = nullptr;

  if (RMW_RET_OK !=
    rmw_connextdds_delete_type_if_unused(
      participant, type_name, &reg_intf))
  {
    return RMW_RET_ERROR;
  }

  if (nullptr != reg_intf) {
    RMW_Connext_RtimeTypePluginI * type_plugin_intf =
      reinterpret_cast<RMW_Connext_RtimeTypePluginI *>(reg_intf);
    RMW_Connext_MessageTypeSupport * type_support =
      ctx->registered_types[tname];
    ctx->registered_types.erase(tname);
    delete type_plugin_intf;
    delete type_support;
  }

  return RMW_RET_OK;
}
