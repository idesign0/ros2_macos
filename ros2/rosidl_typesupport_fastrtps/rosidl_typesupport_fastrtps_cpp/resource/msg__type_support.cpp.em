@# Included from rosidl_typesupport_fastrtps_cpp/resource/idl__type_support.cpp.em
@{
from rosidl_parser.definition import AbstractGenericString
from rosidl_parser.definition import AbstractNestedType
from rosidl_parser.definition import AbstractSequence
from rosidl_parser.definition import AbstractWString
from rosidl_parser.definition import Array
from rosidl_parser.definition import BasicType
from rosidl_parser.definition import BoundedSequence
from rosidl_parser.definition import NamespacedType

header_files = [
    'limits',
    'stdexcept',
    'string',
    'rosidl_typesupport_cpp/message_type_support.hpp',
    'rosidl_typesupport_fastrtps_cpp/identifier.hpp',
    'rosidl_typesupport_fastrtps_cpp/message_type_support.h',
    'rosidl_typesupport_fastrtps_cpp/message_type_support_decl.hpp',
    'rosidl_typesupport_fastrtps_cpp/wstring_conversion.hpp',
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


// forward declaration of message dependencies and their conversion functions
@[for member in message.structure.members]@
@{
type_ = member.type
if isinstance(type_, AbstractNestedType):
    type_ = type_.value_type
}@
@[  if isinstance(type_, NamespacedType)]@
@[    if type_.namespaced_name() in forward_declared_types]@
// functions for @('::'.join(type_.namespaced_name())) already declared above
@[    else]@
@{forward_declared_types.add(type_.namespaced_name())}@
@[      for ns in type_.namespaces]@
namespace @(ns)
{
@[      end for]@
namespace typesupport_fastrtps_cpp
{
bool cdr_serialize(
  const @('::'.join(type_.namespaced_name())) &,
  eprosima::fastcdr::Cdr &);
bool cdr_deserialize(
  eprosima::fastcdr::Cdr &,
  @('::'.join(type_.namespaced_name())) &);
size_t get_serialized_size(
  const @('::'.join(type_.namespaced_name())) &,
  size_t current_alignment);
size_t
max_serialized_size_@(type_.name)(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);
}  // namespace typesupport_fastrtps_cpp
@[      for ns in reversed(type_.namespaces)]@
}  // namespace @(ns)
@[      end for]@
@[    end if]@

@[  end if]@
@[end for]@
@
@[  for ns in message.structure.namespaced_type.namespaces]@

namespace @(ns)
{
@[  end for]@

@{forward_declared_types.add(message.structure.namespaced_type.namespaced_name())}@
namespace typesupport_fastrtps_cpp
{

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_@(package_name)
cdr_serialize(
  const @('::'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name])) & ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
@[for member in message.structure.members]@
  // Member: @(member.name)
@[  if isinstance(member.type, AbstractNestedType)]@
  {
@[    if isinstance(member.type, Array)]@
@[      if not isinstance(member.type.value_type, (NamespacedType, AbstractWString))]@
    cdr << ros_message.@(member.name);
@[      else]@
@[        if isinstance(member.type.value_type, AbstractWString)]@
    std::wstring wstr;
@[        end if]@
    for (size_t i = 0; i < @(member.type.size); i++) {
@[        if isinstance(member.type.value_type, NamespacedType)]@
      @('::'.join(member.type.value_type.namespaces))::typesupport_fastrtps_cpp::cdr_serialize(
        ros_message.@(member.name)[i],
        cdr);
@[        else]@
      rosidl_typesupport_fastrtps_cpp::u16string_to_wstring(ros_message.@(member.name)[i], wstr);
      cdr << wstr;
@[        end if]@
    }
@[      end if]@
@[    else]@
@[      if isinstance(member.type, BoundedSequence) or isinstance(member.type.value_type, (NamespacedType, AbstractWString))]@
    size_t size = ros_message.@(member.name).size();
@[        if isinstance(member.type, BoundedSequence)]@
    if (size > @(member.type.maximum_size)) {
      throw std::runtime_error("array size exceeds upper bound");
    }
@[        end if]@
@[      end if]@
@[      if not isinstance(member.type.value_type, (NamespacedType, AbstractWString)) and not isinstance(member.type, BoundedSequence)]@
    cdr << ros_message.@(member.name);
@[      else]@
    cdr << static_cast<uint32_t>(size);
@[        if isinstance(member.type.value_type, BasicType) and member.type.value_type.typename not in ('boolean', 'wchar')]@
    if (size > 0) {
      cdr.serializeArray(&(ros_message.@(member.name)[0]), size);
    }
@[        else]@
@[            if isinstance(member.type.value_type, AbstractWString)]@
    std::wstring wstr;
@[            end if]@
    for (size_t i = 0; i < size; i++) {
@[            if isinstance(member.type.value_type, BasicType) and member.type.value_type.typename == 'boolean']@
      cdr << (ros_message.@(member.name)[i] ? true : false);
@[            elif isinstance(member.type.value_type, BasicType) and member.type.value_type.typename == 'wchar']@
      cdr << static_cast<wchar_t>(ros_message.@(member.name)[i]);
@[            elif isinstance(member.type.value_type, AbstractWString)]@
      rosidl_typesupport_fastrtps_cpp::u16string_to_wstring(ros_message.@(member.name)[i], wstr);
      cdr << wstr;
@[            elif not isinstance(member.type.value_type, NamespacedType)]@
      cdr << ros_message.@(member.name)[i];
@[            else]@
      @('::'.join(member.type.value_type.namespaces))::typesupport_fastrtps_cpp::cdr_serialize(
        ros_message.@(member.name)[i],
        cdr);
@[            end if]@
    }
@[        end if]@
@[      end if]@
@[    end if]@
  }
@[  elif isinstance(member.type, BasicType) and member.type.typename == 'boolean']@
  cdr << (ros_message.@(member.name) ? true : false);
@[  elif isinstance(member.type, BasicType) and member.type.typename == 'wchar']@
  cdr << static_cast<wchar_t>(ros_message.@(member.name));
@[  elif isinstance(member.type, AbstractWString)]@
  {
    std::wstring wstr;
    rosidl_typesupport_fastrtps_cpp::u16string_to_wstring(ros_message.@(member.name), wstr);
    cdr << wstr;
  }
@[  elif not isinstance(member.type, NamespacedType)]@
  cdr << ros_message.@(member.name);
@[  else]@
  @('::'.join(member.type.namespaces))::typesupport_fastrtps_cpp::cdr_serialize(
    ros_message.@(member.name),
    cdr);
@[  end if]@
@[end for]@
  return true;
}

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_@(package_name)
cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  @('::'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name])) & ros_message)
{
@[for member in message.structure.members]@
  // Member: @(member.name)
@[  if isinstance(member.type, AbstractNestedType)]@
  {
@[    if isinstance(member.type, Array)]@
@[      if not isinstance(member.type.value_type, (NamespacedType, AbstractWString))]@
    cdr >> ros_message.@(member.name);
@[      else]@
@[        if isinstance(member.type.value_type, AbstractWString)]@
    std::wstring wstr;
@[        end if]@
    for (size_t i = 0; i < @(member.type.size); i++) {
@[        if isinstance(member.type.value_type, NamespacedType)]@
      @('::'.join(member.type.value_type.namespaces))::typesupport_fastrtps_cpp::cdr_deserialize(
        cdr,
        ros_message.@(member.name)[i]);
@[        else]@
      cdr >> wstr;
      bool succeeded = rosidl_typesupport_fastrtps_cpp::wstring_to_u16string(wstr, ros_message.@(member.name)[i]);
      if (!succeeded) {
        fprintf(stderr, "failed to create wstring from u16string\n");
        return false;
      }
@[        end if]@
    }
@[      end if]@
@[    else]@
@[      if not isinstance(member.type.value_type, (NamespacedType, AbstractWString)) and not isinstance(member.type, BoundedSequence)]@
    cdr >> ros_message.@(member.name);
@[      else]@
    uint32_t cdrSize;
    cdr >> cdrSize;
    size_t size = static_cast<size_t>(cdrSize);
    ros_message.@(member.name).resize(size);
@[        if isinstance(member.type.value_type, BasicType) and member.type.value_type.typename not in ('boolean', 'wchar')]@
    if (size > 0) {
      cdr.deserializeArray(&(ros_message.@(member.name)[0]), size);
    }
@[        else]@
@[            if isinstance(member.type.value_type, AbstractWString)]@
    std::wstring wstr;
@[            end if]@
    for (size_t i = 0; i < size; i++) {
@[            if isinstance(member.type.value_type, BasicType) and member.type.value_type.typename == 'boolean']@
      uint8_t tmp;
      cdr >> tmp;
      ros_message.@(member.name)[i] = tmp ? true : false;
@[            elif isinstance(member.type.value_type, BasicType) and member.type.value_type.typename == 'wchar']@
      wchar_t tmp;
      cdr >> tmp;
      ros_message.@(member.name)[i] = static_cast<char16_t>(tmp);
@[            elif isinstance(member.type.value_type, AbstractWString)]@
      cdr >> wstr;
      bool succeeded = rosidl_typesupport_fastrtps_cpp::wstring_to_u16string(wstr, ros_message.@(member.name)[i]);
      if (!succeeded) {
        fprintf(stderr, "failed to create wstring from u16string\n");
        return false;
      }
@[            elif not isinstance(member.type.value_type, NamespacedType)]@
      cdr >> ros_message.@(member.name)[i];
@[            else]@
      @('::'.join(member.type.value_type.namespaces))::typesupport_fastrtps_cpp::cdr_deserialize(
        cdr, ros_message.@(member.name)[i]);
@[            end if]@
    }
@[          end if]@
@[      end if]@
@[    end if]@
  }
@[  elif isinstance(member.type, BasicType) and member.type.typename == 'boolean']@
  {
    uint8_t tmp;
    cdr >> tmp;
    ros_message.@(member.name) = tmp ? true : false;
  }
@[  elif isinstance(member.type, BasicType) and member.type.typename == 'wchar']@
  {
    wchar_t tmp;
    cdr >> tmp;
    ros_message.@(member.name) = static_cast<char16_t>(tmp);
  }
@[  elif isinstance(member.type, AbstractWString)]@
  {
    std::wstring wstr;
    cdr >> wstr;
    bool succeeded = rosidl_typesupport_fastrtps_cpp::wstring_to_u16string(wstr, ros_message.@(member.name));
    if (!succeeded) {
      fprintf(stderr, "failed to create wstring from u16string\n");
      return false;
    }
  }
@[  elif not isinstance(member.type, NamespacedType)]@
  cdr >> ros_message.@(member.name);
@[  else]@
  @('::'.join(member.type.namespaces))::typesupport_fastrtps_cpp::cdr_deserialize(
    cdr, ros_message.@(member.name));
@[  end if]@

@[end for]@
  return true;
}

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_@(package_name)
get_serialized_size(
  const @('::'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name])) & ros_message,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

@[for member in message.structure.members]@
  // Member: @(member.name)
@[  if isinstance(member.type, AbstractNestedType)]@
  {
@[    if isinstance(member.type, Array)]@
    size_t array_size = @(member.type.size);
@[    else]@
    size_t array_size = ros_message.@(member.name).size();
@[      if isinstance(member.type, BoundedSequence)]@
    if (array_size > @(member.type.maximum_size)) {
      throw std::runtime_error("array size exceeds upper bound");
    }
@[      end if]@

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
        (ros_message.@(member.name)[index].size() + 1);
    }
@[    elif isinstance(member.type.value_type, BasicType)]@
    size_t item_size = sizeof(ros_message.@(member.name)[0]);
    current_alignment += array_size * item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
@[    else]
    for (size_t index = 0; index < array_size; ++index) {
      current_alignment +=
        @('::'.join(member.type.value_type.namespaces))::typesupport_fastrtps_cpp::get_serialized_size(
        ros_message.@(member.name)[index], current_alignment);
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
    (ros_message.@(member.name).size() + 1);
@[    elif isinstance(member.type, BasicType)]@
  {
    size_t item_size = sizeof(ros_message.@(member.name));
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
@[    else]
  current_alignment +=
    @('::'.join(member.type.namespaces))::typesupport_fastrtps_cpp::get_serialized_size(
    ros_message.@(member.name), current_alignment);
@[    end if]@
@[  end if]@
@[end for]@

  return current_alignment - initial_alignment;
}

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_@(package_name)
max_serialized_size_@(message.structure.namespaced_type.name)(
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

  // Member: @(member.name)
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
      size_t inner_size =
        @('::'.join(type_.namespaces))::typesupport_fastrtps_cpp::max_serialized_size_@(type_.name)(
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
    using DataType = @('::'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]));
    is_plain =
      (
      offsetof(DataType, @(last_member_name_)) +
      last_member_size
      ) == ret_val;
  }

@[end if]@
  return ret_val;
}

static bool _@(message.structure.namespaced_type.name)__cdr_serialize(
  const void * untyped_ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  auto typed_message =
    static_cast<const @('::'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name])) *>(
    untyped_ros_message);
  return cdr_serialize(*typed_message, cdr);
}

static bool _@(message.structure.namespaced_type.name)__cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  void * untyped_ros_message)
{
  auto typed_message =
    static_cast<@('::'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name])) *>(
    untyped_ros_message);
  return cdr_deserialize(cdr, *typed_message);
}

static uint32_t _@(message.structure.namespaced_type.name)__get_serialized_size(
  const void * untyped_ros_message)
{
  auto typed_message =
    static_cast<const @('::'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name])) *>(
    untyped_ros_message);
  return static_cast<uint32_t>(get_serialized_size(*typed_message, 0));
}

static size_t _@(message.structure.namespaced_type.name)__max_serialized_size(char & bounds_info)
{
  bool full_bounded;
  bool is_plain;
  size_t ret_val;

  ret_val = max_serialized_size_@(message.structure.namespaced_type.name)(full_bounded, is_plain, 0);

  bounds_info =
    is_plain ? ROSIDL_TYPESUPPORT_FASTRTPS_PLAIN_TYPE :
    full_bounded ? ROSIDL_TYPESUPPORT_FASTRTPS_BOUNDED_TYPE : ROSIDL_TYPESUPPORT_FASTRTPS_UNBOUNDED_TYPE;
  return ret_val;
}

static message_type_support_callbacks_t _@(message.structure.namespaced_type.name)__callbacks = {
  "@('::'.join([package_name] + list(interface_path.parents[0].parts)))",
  "@(message.structure.namespaced_type.name)",
  _@(message.structure.namespaced_type.name)__cdr_serialize,
  _@(message.structure.namespaced_type.name)__cdr_deserialize,
  _@(message.structure.namespaced_type.name)__get_serialized_size,
  _@(message.structure.namespaced_type.name)__max_serialized_size
};

static rosidl_message_type_support_t _@(message.structure.namespaced_type.name)__handle = {
  rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
  &_@(message.structure.namespaced_type.name)__callbacks,
  get_message_typesupport_handle_function,
};

}  // namespace typesupport_fastrtps_cpp
@[  for ns in reversed(message.structure.namespaced_type.namespaces)]@

}  // namespace @(ns)
@[  end for]@

namespace rosidl_typesupport_fastrtps_cpp
{

template<>
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_EXPORT_@(package_name)
const rosidl_message_type_support_t *
get_message_type_support_handle<@('::'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))>()
{
  return &@('::'.join([package_name] + list(interface_path.parents[0].parts)))::typesupport_fastrtps_cpp::_@(message.structure.namespaced_type.name)__handle;
}

}  // namespace rosidl_typesupport_fastrtps_cpp

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, @(', '.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name])))() {
  return &@('::'.join([package_name] + list(interface_path.parents[0].parts)))::typesupport_fastrtps_cpp::_@(message.structure.namespaced_type.name)__handle;
}

#ifdef __cplusplus
}
#endif
