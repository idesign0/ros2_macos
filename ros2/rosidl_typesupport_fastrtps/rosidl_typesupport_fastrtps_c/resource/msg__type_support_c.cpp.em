@# Included from rosidl_typesupport_fastrtps_c/resource/idl__type_support_c.cpp.em
@{
from rosidl_cmake import convert_camel_case_to_lower_case_underscore
from rosidl_parser.definition import AbstractGenericString
from rosidl_parser.definition import AbstractNestedType
from rosidl_parser.definition import AbstractSequence
from rosidl_parser.definition import AbstractString
from rosidl_parser.definition import AbstractWString
from rosidl_parser.definition import ACTION_FEEDBACK_SUFFIX
from rosidl_parser.definition import ACTION_GOAL_SUFFIX
from rosidl_parser.definition import ACTION_RESULT_SUFFIX
from rosidl_parser.definition import Array
from rosidl_parser.definition import BasicType
from rosidl_parser.definition import BoundedSequence
from rosidl_parser.definition import NamespacedType

include_parts = [package_name] + list(interface_path.parents[0].parts) + [
    'detail', convert_camel_case_to_lower_case_underscore(interface_path.stem)]
include_base = '/'.join(include_parts)

header_files = [
    'cassert',
    'limits',
    'string',
    # Provides the rosidl_typesupport_fastrtps_c__identifier symbol declaration.
    'rosidl_typesupport_fastrtps_c/identifier.h',
    'rosidl_typesupport_fastrtps_c/wstring_conversion.hpp',
    # Provides the definition of the message_type_support_callbacks_t struct.
    'rosidl_typesupport_fastrtps_cpp/message_type_support.h',
    package_name + '/msg/rosidl_typesupport_fastrtps_c__visibility_control.h',
    include_base + '__struct.h',
    include_base + '__functions.h',
    'fastcdr/Cdr.h',
]
}@
@[for header_file in header_files]@
@[    if header_file in include_directives]@
// already included above
// @
@[    else]@
@{include_directives.add(header_file)}@
@[    end if]@
@[    if '/' not in header_file]@
#include <@(header_file)>
@[    else]@
#include "@(header_file)"
@[    end if]@
@[end for]@

#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# ifdef __clang__
#  pragma clang diagnostic ignored "-Wdeprecated-register"
#  pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
# endif
#endif
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif

// includes and forward declarations of message dependencies and their conversion functions

@# // Include the message header for each non-primitive field.
#if defined(__cplusplus)
extern "C"
{
#endif

@{
includes = {}
for member in message.structure.members:
    keys = set([])
    if isinstance(member.type, AbstractSequence) and isinstance(member.type.value_type, BasicType):
        keys.add('rosidl_runtime_c/primitives_sequence.h')
        keys.add('rosidl_runtime_c/primitives_sequence_functions.h')
    type_ = member.type
    if isinstance(type_, AbstractNestedType):
        type_ = type_.value_type
    if isinstance(type_, AbstractString):
        keys.add('rosidl_runtime_c/string.h')
        keys.add('rosidl_runtime_c/string_functions.h')
    elif isinstance(type_, AbstractWString):
        keys.add('rosidl_runtime_c/u16string.h')
        keys.add('rosidl_runtime_c/u16string_functions.h')
    elif isinstance(type_, NamespacedType):
        if (
            type_.name.endswith(ACTION_GOAL_SUFFIX) or
            type_.name.endswith(ACTION_RESULT_SUFFIX) or
            type_.name.endswith(ACTION_FEEDBACK_SUFFIX)
        ):
            typename = type_.name.rsplit('_', 1)[0]
        else:
            typename = type_.name
        keys.add('/'.join(type_.namespaces + ['detail', convert_camel_case_to_lower_case_underscore(typename)]) + '__functions.h')
    for key in keys:
        if key not in includes:
            includes[key] = set([])
        includes[key].add(member.name)
}@
@[for header_file in sorted(includes.keys())]@
@[    if header_file in include_directives]@
// already included above
// @
@[    else]@
@{include_directives.add(header_file)}@
@[    end if]@
#include "@(header_file)"  // @(', '.join(sorted(includes[header_file])))
@[end for]@

// forward declare type support functions
@{
forward_declares = {}
for member in message.structure.members:
    type_ = member.type
    if isinstance(type_, AbstractNestedType):
        type_ = type_.value_type
    if isinstance(type_, NamespacedType):
        key = (*type_.namespaces, type_.name)
        if key not in includes:
            forward_declares[key] = set([])
        forward_declares[key].add(member.name)
}@
@[for key in sorted(forward_declares.keys())]@
@[  if key[0] != package_name]@
ROSIDL_TYPESUPPORT_FASTRTPS_C_IMPORT_@(package_name)
@[  end if]@
size_t get_serialized_size_@('__'.join(key))(
  const void * untyped_ros_message,
  size_t current_alignment);

@[  if key[0] != package_name]@
ROSIDL_TYPESUPPORT_FASTRTPS_C_IMPORT_@(package_name)
@[  end if]@
size_t max_serialized_size_@('__'.join(key))(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

@[  if key[0] != package_name]@
ROSIDL_TYPESUPPORT_FASTRTPS_C_IMPORT_@(package_name)
@[  end if]@
const rosidl_message_type_support_t *
  ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, @(', '.join(key)))();
@[end for]@

@# // Make callback functions specific to this message type.

using _@(message.structure.namespaced_type.name)__ros_msg_type = @('__'.join(message.structure.namespaced_type.namespaced_name()));

static bool _@(message.structure.namespaced_type.name)__cdr_serialize(
  const void * untyped_ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  const _@(message.structure.namespaced_type.name)__ros_msg_type * ros_message = static_cast<const _@(message.structure.namespaced_type.name)__ros_msg_type *>(untyped_ros_message);
@[for member in message.structure.members]@
  // Field name: @(member.name)
  {
@{
type_ = member.type
if isinstance(type_, AbstractNestedType):
    type_ = type_.value_type
}@
@[  if isinstance(type_, NamespacedType)]@
    const message_type_support_callbacks_t * callbacks =
      static_cast<const message_type_support_callbacks_t *>(
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(
        rosidl_typesupport_fastrtps_c, @(', '.join(type_.namespaced_name()))
      )()->data);
@[  end if]@
@[  if isinstance(member.type, AbstractNestedType)]@
@[    if isinstance(member.type, Array)]@
    size_t size = @(member.type.size);
    auto array_ptr = ros_message->@(member.name);
@[    else]@
    size_t size = ros_message->@(member.name).size;
    auto array_ptr = ros_message->@(member.name).data;
@[      if isinstance(member.type, BoundedSequence)]@
    if (size > @(member.type.maximum_size)) {
      fprintf(stderr, "array size exceeds upper bound\n");
      return false;
    }
@[      end if]@
    cdr << static_cast<uint32_t>(size);
@[    end if]@
@[    if isinstance(member.type.value_type, AbstractString)]@
    for (size_t i = 0; i < size; ++i) {
      const rosidl_runtime_c__String * str = &array_ptr[i];
      if (str->capacity == 0 || str->capacity <= str->size) {
        fprintf(stderr, "string capacity not greater than size\n");
        return false;
      }
      if (str->data[str->size] != '\0') {
        fprintf(stderr, "string not null-terminated\n");
        return false;
      }
      cdr << str->data;
    }
@[    elif isinstance(member.type.value_type, AbstractWString)]@
    std::wstring wstr;
    for (size_t i = 0; i < size; ++i) {
      const rosidl_runtime_c__U16String * str = &array_ptr[i];
      if (str->capacity == 0 || str->capacity <= str->size) {
        fprintf(stderr, "string capacity not greater than size\n");
        return false;
      }
      if (str->data[str->size] != u'\0') {
        fprintf(stderr, "string not null-terminated\n");
        return false;
      }
      rosidl_typesupport_fastrtps_c::u16string_to_wstring(*str, wstr);
      cdr << wstr;
    }
@[    elif isinstance(member.type.value_type, BasicType) and member.type.value_type.typename == 'wchar']@
    for (size_t i = 0; i < size; ++i) {
      if (!callbacks->cdr_serialize(
          static_cast<wchar_t *>(&array_ptr[i]), cdr))
      {
        return false;
      }
    }
@[    elif isinstance(member.type.value_type, BasicType)]@
    cdr.serializeArray(array_ptr, size);
@[    else]@
    for (size_t i = 0; i < size; ++i) {
      if (!callbacks->cdr_serialize(
          &array_ptr[i], cdr))
      {
        return false;
      }
    }
@[    end if]@
@[  elif isinstance(member.type, AbstractString)]@
    const rosidl_runtime_c__String * str = &ros_message->@(member.name);
    if (str->capacity == 0 || str->capacity <= str->size) {
      fprintf(stderr, "string capacity not greater than size\n");
      return false;
    }
    if (str->data[str->size] != '\0') {
      fprintf(stderr, "string not null-terminated\n");
      return false;
    }
    cdr << str->data;
@[  elif isinstance(member.type, AbstractWString)]@
    std::wstring wstr;
    rosidl_typesupport_fastrtps_c::u16string_to_wstring(ros_message->@(member.name), wstr);
    cdr << wstr;
@[  elif isinstance(member.type, BasicType) and member.type.typename == 'boolean']@
    cdr << (ros_message->@(member.name) ? true : false);
@[  elif isinstance(member.type, BasicType) and member.type.typename == 'wchar']@
    cdr << static_cast<wchar_t>(ros_message->@(member.name));
@[  elif isinstance(member.type, BasicType)]@
    cdr << ros_message->@(member.name);
@[  else]@
    if (!callbacks->cdr_serialize(
        &ros_message->@(member.name), cdr))
    {
      return false;
    }
@[  end if]@
  }

@[end for]@
  return true;
}

static bool _@(message.structure.namespaced_type.name)__cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  void * untyped_ros_message)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  _@(message.structure.namespaced_type.name)__ros_msg_type * ros_message = static_cast<_@(message.structure.namespaced_type.name)__ros_msg_type *>(untyped_ros_message);
@[for member in message.structure.members]@
  // Field name: @(member.name)
  {
@{
type_ = member.type
if isinstance(type_, AbstractNestedType):
    type_ = type_.value_type
}@
@[  if isinstance(type_, NamespacedType)]@
    const message_type_support_callbacks_t * callbacks =
      static_cast<const message_type_support_callbacks_t *>(
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(
        rosidl_typesupport_fastrtps_c, @(', '.join(type_.namespaced_name()))
      )()->data);
@[  end if]@
@[  if isinstance(member.type, AbstractNestedType)]@
@[    if isinstance(member.type, Array)]@
    size_t size = @(member.type.size);
    auto array_ptr = ros_message->@(member.name);
@[    else]@
@{
if isinstance(member.type.value_type, AbstractString):
    array_init = 'rosidl_runtime_c__String__Sequence__init'
    array_fini = 'rosidl_runtime_c__String__Sequence__fini'
elif isinstance(member.type.value_type, AbstractWString):
    array_init = 'rosidl_runtime_c__U16String__Sequence__init'
    array_fini = 'rosidl_runtime_c__U16String__Sequence__fini'
elif isinstance(member.type.value_type, BasicType):
    type_ = member.type.value_type.typename
    type_ = type_.replace(' ', '_')
    array_init = 'rosidl_runtime_c__{type_}__Sequence__init'.format(**locals())
    array_fini = 'rosidl_runtime_c__{type_}__Sequence__fini'.format(**locals())
else:
    array_init = '__'.join(type_.namespaced_name()) + '__Sequence__init'
    array_fini = '__'.join(type_.namespaced_name()) + '__Sequence__fini'
}@
    uint32_t cdrSize;
    cdr >> cdrSize;
    size_t size = static_cast<size_t>(cdrSize);
    if (ros_message->@(member.name).data) {
      @(array_fini)(&ros_message->@(member.name));
    }
    if (!@(array_init)(&ros_message->@(member.name), size)) {
      fprintf(stderr, "failed to create array for field '@(member.name)'");
      return false;
    }
    auto array_ptr = ros_message->@(member.name).data;
@[    end if]@
@[    if isinstance(member.type.value_type, AbstractString)]@
    for (size_t i = 0; i < size; ++i) {
      std::string tmp;
      cdr >> tmp;
      auto & ros_i = array_ptr[i];
      if (!ros_i.data) {
        rosidl_runtime_c__String__init(&ros_i);
      }
      bool succeeded = rosidl_runtime_c__String__assign(
        &ros_i,
        tmp.c_str());
      if (!succeeded) {
        fprintf(stderr, "failed to assign string into field '@(member.name)'\n");
        return false;
      }
    }
@[    elif isinstance(member.type.value_type, AbstractWString)]@
    std::wstring wstr;
    for (size_t i = 0; i < size; ++i) {
      auto & ros_i = array_ptr[i];
      if (!ros_i.data) {
        rosidl_runtime_c__U16String__init(&ros_i);
      }
      cdr >> wstr;
      bool succeeded = rosidl_typesupport_fastrtps_c::wstring_to_u16string(wstr, ros_i);
      if (!succeeded) {
        fprintf(stderr, "failed to create wstring from u16string\n");
        rosidl_runtime_c__U16String__fini(&ros_i);
        return false;
      }
    }
@[    elif isinstance(member.type.value_type, BasicType) and member.type.value_type.typename == 'boolean']@
    for (size_t i = 0; i < size; ++i) {
      uint8_t tmp;
      cdr >> tmp;
      array_ptr[i] = tmp ? true : false;
    }
@[    elif isinstance(member.type.value_type, BasicType) and member.type.value_type.typename == 'wchar']@
    for (size_t i = 0; i < size; ++i) {
      wchar_t tmp;
      cdr >> tmp;
      array_ptr[i] = static_cast<char16_t>(tmp);
    }
@[    elif isinstance(member.type.value_type, BasicType)]@
    cdr.deserializeArray(array_ptr, size);
@[    else]@
    for (size_t i = 0; i < size; ++i) {
      if (!callbacks->cdr_deserialize(
          cdr, &array_ptr[i]))
      {
        return false;
      }
    }
@[    end if]@
@[   elif isinstance(member.type, AbstractString)]@
    std::string tmp;
    cdr >> tmp;
    if (!ros_message->@(member.name).data) {
      rosidl_runtime_c__String__init(&ros_message->@(member.name));
    }
    bool succeeded = rosidl_runtime_c__String__assign(
      &ros_message->@(member.name),
      tmp.c_str());
    if (!succeeded) {
      fprintf(stderr, "failed to assign string into field '@(member.name)'\n");
      return false;
    }
@[   elif isinstance(member.type, AbstractWString)]@
    if (!ros_message->@(member.name).data) {
      rosidl_runtime_c__U16String__init(&ros_message->@(member.name));
    }
    std::wstring wstr;
    cdr >> wstr;
    bool succeeded = rosidl_typesupport_fastrtps_c::wstring_to_u16string(wstr, ros_message->@(member.name));
    if (!succeeded) {
      fprintf(stderr, "failed to create wstring from u16string\n");
      rosidl_runtime_c__U16String__fini(&ros_message->@(member.name));
      return false;
    }
@[ elif isinstance(member.type, BasicType) and member.type.typename == 'boolean']@
    uint8_t tmp;
    cdr >> tmp;
    ros_message->@(member.name) = tmp ? true : false;
@[ elif isinstance(member.type, BasicType) and member.type.typename == 'wchar']@
    wchar_t tmp;
    cdr >> tmp;
    ros_message->@(member.name) = static_cast<char16_t>(tmp);
@[  elif isinstance(member.type, BasicType)]@
    cdr >> ros_message->@(member.name);
@[  else]@
    if (!callbacks->cdr_deserialize(
        cdr, &ros_message->@(member.name)))
    {
      return false;
    }
@[  end if]@
  }

@[end for]@
  return true;
}  // NOLINT(readability/fn_size)

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_@(package_name)
size_t get_serialized_size_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(
  const void * untyped_ros_message,
  size_t current_alignment)
{
  const _@(message.structure.namespaced_type.name)__ros_msg_type * ros_message = static_cast<const _@(message.structure.namespaced_type.name)__ros_msg_type *>(untyped_ros_message);
  (void)ros_message;
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

@[for member in message.structure.members]@
  // field.name @(member.name)
@[  if isinstance(member.type, AbstractNestedType)]@
  {
@[    if isinstance(member.type, Array)]@
    size_t array_size = @(member.type.size);
    auto array_ptr = ros_message->@(member.name);
@[    else]@
    size_t array_size = ros_message->@(member.name).size;
    auto array_ptr = ros_message->@(member.name).data;
    current_alignment += padding +
      eprosima::fastcdr::Cdr::alignment(current_alignment, padding);
@[    end if]@
@[    if isinstance(member.type.value_type, AbstractGenericString)]@
    for (size_t index = 0; index < array_size; ++index) {
      current_alignment += padding +
        eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
@[      if isinstance(member.type.value_type, AbstractWString)]@
        wchar_size *
@[      end if]@
        (array_ptr[index].size + 1);
    }
@[    elif isinstance(member.type.value_type, BasicType)]@
    (void)array_ptr;
    size_t item_size = sizeof(array_ptr[0]);
    current_alignment += array_size * item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
@[    else]
    for (size_t index = 0; index < array_size; ++index) {
      current_alignment += get_serialized_size_@('__'.join(member.type.value_type.namespaced_name()))(
        &array_ptr[index], current_alignment);
    }
@[    end if]@
  }
@[  else]@
@[    if isinstance(member.type, AbstractGenericString)]@
  current_alignment += padding +
    eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
@[      if isinstance(member.type, AbstractWString)]@
    wchar_size *
@[      end if]@
    (ros_message->@(member.name).size + 1);
@[    elif isinstance(member.type, BasicType)]@
  {
    size_t item_size = sizeof(ros_message->@(member.name));
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
@[    else]
  current_alignment += get_serialized_size_@('__'.join(member.type.namespaced_name()))(
    &(ros_message->@(member.name)), current_alignment);
@[    end if]@
@[  end if]@
@[end for]@

  return current_alignment - initial_alignment;
}

static uint32_t _@(message.structure.namespaced_type.name)__get_serialized_size(const void * untyped_ros_message)
{
  return static_cast<uint32_t>(
    get_serialized_size_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(
      untyped_ros_message, 0));
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_@(package_name)
size_t max_serialized_size_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  size_t last_member_size = 0;
  (void)last_member_size;
  (void)padding;
  (void)wchar_size;

  full_bounded = true;
  is_plain = true;

@{
last_member_name_ = None
}@
@[for member in message.structure.members]@
@{
last_member_name_ = member.name
}@
  // member: @(member.name)
  {
@[  if isinstance(member.type, AbstractNestedType)]@
@[    if isinstance(member.type, Array)]@
    size_t array_size = @(member.type.size);
@[    elif isinstance(member.type, BoundedSequence)]@
    size_t array_size = @(member.type.maximum_size);
@[    else]@
    size_t array_size = 0;
    full_bounded = false;
@[    end if]@
@[    if isinstance(member.type, AbstractSequence)]@
    is_plain = false;
    current_alignment += padding +
      eprosima::fastcdr::Cdr::alignment(current_alignment, padding);
@[    end if]@
@[  else]@
    size_t array_size = 1;
@[  end if]@

@{
type_ = member.type
if isinstance(type_, AbstractNestedType):
    type_ = type_.value_type
}@
@[  if isinstance(type_, AbstractGenericString)]@
    full_bounded = false;
    is_plain = false;
    for (size_t index = 0; index < array_size; ++index) {
      current_alignment += padding +
        eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
@[    if type_.has_maximum_size()]@
@[      if isinstance(type_, AbstractWString)]@
        wchar_size *
@[      end if]@
        @(type_.maximum_size) +
@[    end if]@
@[    if isinstance(type_, AbstractWString)]@
        wchar_size *
@[    end if]@
        1;
    }
@[  elif isinstance(type_, BasicType)]@
@[    if type_.typename in ('boolean', 'octet', 'char', 'uint8', 'int8')]@
    last_member_size = array_size * sizeof(uint8_t);
    current_alignment += array_size * sizeof(uint8_t);
@[    elif type_.typename in ('wchar', 'int16', 'uint16')]@
    last_member_size = array_size * sizeof(uint16_t);
    current_alignment += array_size * sizeof(uint16_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint16_t));
@[    elif type_.typename in ('int32', 'uint32', 'float')]@
    last_member_size = array_size * sizeof(uint32_t);
    current_alignment += array_size * sizeof(uint32_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint32_t));
@[    elif type_.typename in ('int64', 'uint64', 'double')]@
    last_member_size = array_size * sizeof(uint64_t);
    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
@[    elif type_.typename == 'long double']@
    last_member_size = array_size * sizeof(long double);
    current_alignment += array_size * sizeof(long double) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(long double));
@[    end if]@
@[  else]
    last_member_size = 0;
    for (size_t index = 0; index < array_size; ++index) {
      bool inner_full_bounded;
      bool inner_is_plain;
      size_t inner_size;
      inner_size =
        max_serialized_size_@('__'.join(type_.namespaced_name()))(
        inner_full_bounded, inner_is_plain, current_alignment);
      last_member_size += inner_size;
      current_alignment += inner_size;
      full_bounded &= inner_full_bounded;
      is_plain &= inner_is_plain;
    }
@[  end if]@
  }
@[end for]@

  size_t ret_val = current_alignment - initial_alignment;
@[if last_member_name_ is not None]@
  if (is_plain) {
    // All members are plain, and type is not empty.
    // We still need to check that the in-memory alignment
    // is the same as the CDR mandated alignment.
    using DataType = @('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]));
    is_plain =
      (
      offsetof(DataType, @(last_member_name_)) +
      last_member_size
      ) == ret_val;
  }

@[end if]@
  return ret_val;
}

static size_t _@(message.structure.namespaced_type.name)__max_serialized_size(char & bounds_info)
{
  bool full_bounded;
  bool is_plain;
  size_t ret_val;

  ret_val = max_serialized_size_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(
    full_bounded, is_plain, 0);

  bounds_info =
    is_plain ? ROSIDL_TYPESUPPORT_FASTRTPS_PLAIN_TYPE :
    full_bounded ? ROSIDL_TYPESUPPORT_FASTRTPS_BOUNDED_TYPE : ROSIDL_TYPESUPPORT_FASTRTPS_UNBOUNDED_TYPE;
  return ret_val;
}

@
@# // Collect the callback functions and provide a function to get the type support struct.

static message_type_support_callbacks_t __callbacks_@(message.structure.namespaced_type.name) = {
  "@('::'.join([package_name] + list(interface_path.parents[0].parts)))",
  "@(message.structure.namespaced_type.name)",
  _@(message.structure.namespaced_type.name)__cdr_serialize,
  _@(message.structure.namespaced_type.name)__cdr_deserialize,
  _@(message.structure.namespaced_type.name)__get_serialized_size,
  _@(message.structure.namespaced_type.name)__max_serialized_size
};

static rosidl_message_type_support_t _@(message.structure.namespaced_type.name)__type_support = {
  rosidl_typesupport_fastrtps_c__identifier,
  &__callbacks_@(message.structure.namespaced_type.name),
  get_message_typesupport_handle_function,
};

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, @(', '.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name])))() {
  return &_@(message.structure.namespaced_type.name)__type_support;
}

#if defined(__cplusplus)
}
#endif
