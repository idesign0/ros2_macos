@# Included from rosidl_generator_py/resource/_idl.py.em
@{

from rosidl_cmake import convert_camel_case_to_lower_case_underscore
from rosidl_generator_py.generate_py_impl import constant_value_to_py
from rosidl_generator_py.generate_py_impl import get_python_type
from rosidl_generator_py.generate_py_impl import SPECIAL_NESTED_BASIC_TYPES
from rosidl_generator_py.generate_py_impl import value_to_py
from rosidl_parser.definition import AbstractGenericString
from rosidl_parser.definition import AbstractNestedType
from rosidl_parser.definition import AbstractSequence
from rosidl_parser.definition import AbstractWString
from rosidl_parser.definition import ACTION_FEEDBACK_SUFFIX
from rosidl_parser.definition import ACTION_GOAL_SUFFIX
from rosidl_parser.definition import ACTION_RESULT_SUFFIX
from rosidl_parser.definition import Array
from rosidl_parser.definition import BasicType
from rosidl_parser.definition import BOOLEAN_TYPE
from rosidl_parser.definition import BoundedSequence
from rosidl_parser.definition import CHARACTER_TYPES
from rosidl_parser.definition import EMPTY_STRUCTURE_REQUIRED_MEMBER_NAME
from rosidl_parser.definition import FLOATING_POINT_TYPES
from rosidl_parser.definition import INTEGER_TYPES
from rosidl_parser.definition import NamespacedType
from rosidl_parser.definition import SIGNED_INTEGER_TYPES
from rosidl_parser.definition import UnboundedSequence
from rosidl_parser.definition import UNSIGNED_INTEGER_TYPES
}@
@#<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
@# Collect necessary import statements for all members
@{
from collections import OrderedDict
imports = OrderedDict()
if message.structure.members:
    imports.setdefault(
        'import rosidl_parser.definition', [])  # used for SLOT_TYPES
for member in message.structure.members:
    type_ = member.type
    if isinstance(type_, AbstractNestedType):
        type_ = type_.value_type
    if member.name != EMPTY_STRUCTURE_REQUIRED_MEMBER_NAME:
        imports.setdefault(
            'import builtins', [])  # used for @builtins.property
    if isinstance(type_, BasicType) and type_.typename in FLOATING_POINT_TYPES:
        imports.setdefault(
            'import math', [])  # used for math.isinf
    if (
        isinstance(member.type, AbstractNestedType) and
        isinstance(member.type.value_type, BasicType) and
        member.type.value_type.typename in SPECIAL_NESTED_BASIC_TYPES
    ):
        if isinstance(member.type, Array):
            member_names = imports.setdefault(
                'import numpy', [])
        elif isinstance(member.type, AbstractSequence):
            member_names = imports.setdefault(
                'import array', [])
        else:
            assert False
        member_names.append(member.name)
}@
@#>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
@
@#<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
@[if imports]@


# Import statements for member types
@[    for import_statement, member_names in sorted(imports.items())]@

@[        for member_name in member_names]@
# Member '@(member_name)'
@[        end for]@
@[        if import_statement in import_statements]@
# already imported above
# @
@[        end if]@
@(import_statement)@
@[        if import_statement not in import_statements]@
@{import_statements.add(import_statement)}@
  # noqa: E402, I100@
@[        end if]
@[    end for]@
@[end if]@
@#>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


class Metaclass_@(message.structure.namespaced_type.name)(type):
    """Metaclass of message '@(message.structure.namespaced_type.name)'."""

    _CREATE_ROS_MESSAGE = None
    _CONVERT_FROM_PY = None
    _CONVERT_TO_PY = None
    _DESTROY_ROS_MESSAGE = None
    _TYPE_SUPPORT = None

    __constants = {
@[for constant in message.constants]@
        '@(constant.name)': @constant_value_to_py(constant.type, constant.value),
@[end for]@
    }

    @@classmethod
    def __import_type_support__(cls):
        try:
            from rosidl_generator_py import import_type_support
            module = import_type_support('@(package_name)')
        except ImportError:
            import logging
            import traceback
            logger = logging.getLogger(
                '@('.'.join(message.structure.namespaced_type.namespaced_name()))')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
@{
suffix = '__'.join(message.structure.namespaced_type.namespaces[1:]) + '__' + convert_camel_case_to_lower_case_underscore(message.structure.namespaced_type.name)
}@
            cls._CREATE_ROS_MESSAGE = module.create_ros_message_msg__@(suffix)
            cls._CONVERT_FROM_PY = module.convert_from_py_msg__@(suffix)
            cls._CONVERT_TO_PY = module.convert_to_py_msg__@(suffix)
            cls._TYPE_SUPPORT = module.type_support_msg__@(suffix)
            cls._DESTROY_ROS_MESSAGE = module.destroy_ros_message_msg__@(suffix)
@{
importable_typesupports = set()
for member in message.structure.members:
    type_ = member.type
    if isinstance(type_, AbstractNestedType):
        type_ = type_.value_type
    if isinstance(type_, NamespacedType):
        if (
            type_.name.endswith(ACTION_GOAL_SUFFIX) or
            type_.name.endswith(ACTION_RESULT_SUFFIX) or
            type_.name.endswith(ACTION_FEEDBACK_SUFFIX)
        ):
            action_name, suffix = type_.name.rsplit('_', 1)
            typename = (*type_.namespaces, action_name, action_name + '.' + suffix)
        else:
            typename = (*type_.namespaces, type_.name, type_.name)
        importable_typesupports.add(typename)
}@
@[for typename in sorted(importable_typesupports)]@

            from @('.'.join(typename[:-2])) import @(typename[-2])
            if @(typename[-1]).__class__._TYPE_SUPPORT is None:
                @(typename[-1]).__class__.__import_type_support__()
@[end for]@

    @@classmethod
    def __prepare__(cls, name, bases, **kwargs):
        # list constant names here so that they appear in the help text of
        # the message class under "Data and other attributes defined here:"
        # as well as populate each message instance
        return {
@[for constant in message.constants]@
            '@(constant.name)': cls.__constants['@(constant.name)'],
@[end for]@
@[for member in message.structure.members]@
@[  if member.has_annotation('default')]@
            '@(member.name.upper())__DEFAULT': @(value_to_py(member.type, member.get_annotation_value('default')['value'])),
@[  end if]@
@[end for]@
        }
@[for constant in message.constants]@

    @@property
    def @(constant.name)(self):
        """Message constant '@(constant.name)'."""
        return Metaclass_@(message.structure.namespaced_type.name).__constants['@(constant.name)']
@[end for]@
@[for member in message.structure.members]@
@[  if member.has_annotation('default')]@

    @@property
    def @(member.name.upper())__DEFAULT(cls):
        """Return default value for message field '@(member.name)'."""
        return @(value_to_py(member.type, member.get_annotation_value('default')['value']))
@[  end if]@
@[end for]@


class @(message.structure.namespaced_type.name)(metaclass=Metaclass_@(message.structure.namespaced_type.name)):
@[if not message.constants]@
    """Message class '@(message.structure.namespaced_type.name)'."""
@[else]@
    """
    Message class '@(message.structure.namespaced_type.name)'.

    Constants:
@[  for constant_name in [c.name for c in message.constants]]@
      @(constant_name)
@[  end for]@
    """
@[end if]@

    __slots__ = [
@[for member in message.structure.members]@
@[  if len(message.structure.members) == 1 and member.name == EMPTY_STRUCTURE_REQUIRED_MEMBER_NAME]@
@[    continue]@
@[  end if]@
        '_@(member.name)',
@[end for]@
    ]

    _fields_and_field_types = {
@[for member in message.structure.members]@
@[  if len(message.structure.members) == 1 and member.name == EMPTY_STRUCTURE_REQUIRED_MEMBER_NAME]@
@[    continue]@
@[  end if]@
@{
type_ = member.type
if isinstance(type_, AbstractNestedType):
    type_ = type_.value_type
}@
        '@(member.name)': '@
@# the prefix for nested types
@[  if isinstance(member.type, AbstractSequence)]@
sequence<@
@[  end if]@
@# the typename of the non-nested type or the nested basetype
@[  if isinstance(type_, BasicType)]@
@(type_.typename)@
@[  elif isinstance(type_, AbstractGenericString)]@
@
@[    if isinstance(type_, AbstractWString)]@
w@
@[    end if]@
string@
@[    if type_.has_maximum_size()]@
<@(type_.maximum_size)>@
@[    end if]@
@[  elif isinstance(type_, NamespacedType)]@
@('/'.join([type_.namespaces[0], type_.name]))@
@[  end if]@
@# the suffix for nested types
@[  if isinstance(member.type, AbstractSequence)]@
@[    if isinstance(member.type, BoundedSequence)]@
, @(member.type.maximum_size)@
@[    end if]@
>@
@[  elif isinstance(member.type, Array)]@
[@(member.type.size)]@
@[  end if]@
',
@[end for]@
    }

    SLOT_TYPES = (
@[for member in message.structure.members]@
@[  if len(message.structure.members) == 1 and member.name == EMPTY_STRUCTURE_REQUIRED_MEMBER_NAME]@
@[    continue]@
@[  end if]@
@{
type_ = member.type
if isinstance(type_, AbstractNestedType):
    type_ = type_.value_type
}@
        @
@[  if isinstance(member.type, AbstractNestedType)]@
@(member.type.__class__.__module__).@(member.type.__class__.__name__)(@
@[  end if]@
@# the typename of the non-nested type or the nested value_type
@(type_.__class__.__module__).@(type_.__class__.__name__)(@
@[  if isinstance(type_, BasicType)]@
'@(type_.typename)'@
@[  elif isinstance(type_, AbstractGenericString) and type_.has_maximum_size()]@
@(type_.maximum_size)@
@[  elif isinstance(type_, NamespacedType)]@
[@(', '.join("'%s'" % n for n in type_.namespaces))], '@(type_.name)'@
@[  end if]@
)@
@[  if isinstance(member.type, Array)]@
, @(member.type.size)@
@[  elif isinstance(member.type, BoundedSequence)]@
, @(member.type.maximum_size)@
@[  end if]@
@[  if isinstance(member.type, AbstractNestedType)]@
)@
@[  end if]@
,  # noqa: E501
@[end for]@
    )

    def __init__(self, **kwargs):
        assert all('_' + key in self.__slots__ for key in kwargs.keys()), \
            'Invalid arguments passed to constructor: %s' % \
            ', '.join(sorted(k for k in kwargs.keys() if '_' + k not in self.__slots__))
@[for member in message.structure.members]@
@[  if len(message.structure.members) == 1 and member.name == EMPTY_STRUCTURE_REQUIRED_MEMBER_NAME]@
@[    continue]@
@[  end if]@
@{
type_ = member.type
if isinstance(type_, AbstractNestedType):
    type_ = type_.value_type
}@
@[  if member.has_annotation('default')]@
        self.@(member.name) = kwargs.get(
            '@(member.name)', @(message.structure.namespaced_type.name).@(member.name.upper())__DEFAULT)
@[  else]@
@[    if isinstance(type_, NamespacedType) and not isinstance(member.type, AbstractSequence)]@
@[      if (
            type_.name.endswith(ACTION_GOAL_SUFFIX) or
            type_.name.endswith(ACTION_RESULT_SUFFIX) or
            type_.name.endswith(ACTION_FEEDBACK_SUFFIX)
        )]@
        from @('.'.join(type_.namespaces))._@(convert_camel_case_to_lower_case_underscore(type_.name.rsplit('_', 1)[0])) import @(type_.name)
@[      else]@
        from @('.'.join(type_.namespaces)) import @(type_.name)
@[      end if]@
@[    end if]@
@[    if isinstance(member.type, Array)]@
@[      if isinstance(type_, BasicType) and type_.typename == 'octet']@
        self.@(member.name) = kwargs.get(
            '@(member.name)',
            [bytes([0]) for x in range(@(member.type.size))]
        )
@[      elif isinstance(type_, BasicType) and type_.typename in CHARACTER_TYPES]@
        self.@(member.name) = kwargs.get(
            '@(member.name)',
            [chr(0) for x in range(@(member.type.size))]
        )
@[      else]@
@[        if isinstance(member.type.value_type, BasicType) and member.type.value_type.typename in SPECIAL_NESTED_BASIC_TYPES]@
        if '@(member.name)' not in kwargs:
            self.@(member.name) = numpy.zeros(@(member.type.size), dtype=@(SPECIAL_NESTED_BASIC_TYPES[member.type.value_type.typename]['dtype']))
        else:
            self.@(member.name) = numpy.array(kwargs.get('@(member.name)'), dtype=@(SPECIAL_NESTED_BASIC_TYPES[member.type.value_type.typename]['dtype']))
            assert self.@(member.name).shape == (@(member.type.size), )
@[        else]@
        self.@(member.name) = kwargs.get(
            '@(member.name)',
            [@(get_python_type(type_))() for x in range(@(member.type.size))]
        )
@[        end if]@
@[      end if]@
@[    elif isinstance(member.type, AbstractSequence)]@
@[      if isinstance(member.type.value_type, BasicType) and member.type.value_type.typename in SPECIAL_NESTED_BASIC_TYPES]@
        self.@(member.name) = array.array('@(SPECIAL_NESTED_BASIC_TYPES[member.type.value_type.typename]['type_code'])', kwargs.get('@(member.name)', []))
@[      else]@
        self.@(member.name) = kwargs.get('@(member.name)', [])
@[      end if]@
@[    elif isinstance(type_, BasicType) and type_.typename == 'octet']@
        self.@(member.name) = kwargs.get('@(member.name)', bytes([0]))
@[    elif isinstance(type_, BasicType) and type_.typename in CHARACTER_TYPES]@
        self.@(member.name) = kwargs.get('@(member.name)', chr(0))
@[    else]@
        self.@(member.name) = kwargs.get('@(member.name)', @(get_python_type(type_))())
@[    end if]@
@[  end if]@
@[end for]@

    def __repr__(self):
        typename = self.__class__.__module__.split('.')
        typename.pop()
        typename.append(self.__class__.__name__)
        args = []
        for s, t in zip(self.__slots__, self.SLOT_TYPES):
            field = getattr(self, s)
            fieldstr = repr(field)
            # We use Python array type for fields that can be directly stored
            # in them, and "normal" sequences for everything else.  If it is
            # a type that we store in an array, strip off the 'array' portion.
            if (
                isinstance(t, rosidl_parser.definition.AbstractSequence) and
                isinstance(t.value_type, rosidl_parser.definition.BasicType) and
                t.value_type.typename in @([*SPECIAL_NESTED_BASIC_TYPES])
            ):
                if len(field) == 0:
                    fieldstr = '[]'
                else:
                    assert fieldstr.startswith('array(')
                    prefix = "array('X', "
                    suffix = ')'
                    fieldstr = fieldstr[len(prefix):-len(suffix)]
            args.append(s[1:] + '=' + fieldstr)
        return '%s(%s)' % ('.'.join(typename), ', '.join(args))

    def __eq__(self, other):
        if not isinstance(other, self.__class__):
            return False
@[for member in message.structure.members]@
@[  if len(message.structure.members) == 1 and member.name == EMPTY_STRUCTURE_REQUIRED_MEMBER_NAME]@
@[    continue]@
@[  end if]@
@[  if isinstance(member.type, Array) and isinstance(member.type.value_type, BasicType) and member.type.value_type.typename in SPECIAL_NESTED_BASIC_TYPES]@
        if any(self.@(member.name) != other.@(member.name)):
@[  else]@
        if self.@(member.name) != other.@(member.name):
@[  end if]@
            return False
@[end for]@
        return True

    @@classmethod
    def get_fields_and_field_types(cls):
        from copy import copy
        return copy(cls._fields_and_field_types)
@[for member in message.structure.members]@
@[  if len(message.structure.members) == 1 and member.name == EMPTY_STRUCTURE_REQUIRED_MEMBER_NAME]@
@[    continue]@
@[  end if]@

@{
type_ = member.type
if isinstance(type_, AbstractNestedType):
    type_ = type_.value_type

import inspect
import builtins
noqa_string = ''
if member.name in dict(inspect.getmembers(builtins)).keys():
    noqa_string = '  # noqa: A003'
}@
    @@builtins.property@(noqa_string)
    def @(member.name)(self):@(noqa_string)
        """Message field '@(member.name)'."""
        return self._@(member.name)

    @@@(member.name).setter@(noqa_string)
    def @(member.name)(self, value):@(noqa_string)
@[  if isinstance(member.type, AbstractNestedType) and isinstance(member.type.value_type, BasicType) and member.type.value_type.typename in SPECIAL_NESTED_BASIC_TYPES]@
@[    if isinstance(member.type, Array)]@
        if isinstance(value, numpy.ndarray):
            assert value.dtype == @(SPECIAL_NESTED_BASIC_TYPES[member.type.value_type.typename]['dtype']), \
                "The '@(member.name)' numpy.ndarray() must have the dtype of '@(SPECIAL_NESTED_BASIC_TYPES[member.type.value_type.typename]['dtype'])'"
            assert value.size == @(member.type.size), \
                "The '@(member.name)' numpy.ndarray() must have a size of @(member.type.size)"
            self._@(member.name) = value
            return
@[    elif isinstance(member.type, AbstractSequence)]@
        if isinstance(value, array.array):
            assert value.typecode == '@(SPECIAL_NESTED_BASIC_TYPES[member.type.value_type.typename]['type_code'])', \
                "The '@(member.name)' array.array() must have the type code of '@(SPECIAL_NESTED_BASIC_TYPES[member.type.value_type.typename]['type_code'])'"
@[      if isinstance(member.type, BoundedSequence)]@
            assert len(value) <= @(member.type.maximum_size), \
                "The '@(member.name)' array.array() must have a size <= @(member.type.maximum_size)"
@[      end if]@
            self._@(member.name) = value
            return
@[    end if]@
@[  end if]@
        if __debug__:
@[  if isinstance(type_, NamespacedType)]@
@[      if (
            type_.name.endswith(ACTION_GOAL_SUFFIX) or
            type_.name.endswith(ACTION_RESULT_SUFFIX) or
            type_.name.endswith(ACTION_FEEDBACK_SUFFIX)
        )]@
            from @('.'.join(type_.namespaces))._@(convert_camel_case_to_lower_case_underscore(type_.name.rsplit('_', 1)[0])) import @(type_.name)
@[      else]@
            from @('.'.join(type_.namespaces)) import @(type_.name)
@[      end if]@
@[  end if]@
@[  if isinstance(member.type, AbstractNestedType)]@
            from collections.abc import Sequence
            from collections.abc import Set
            from collections import UserList
            from collections import UserString
@[  elif isinstance(type_, AbstractGenericString) and type_.has_maximum_size()]@
            from collections import UserString
@[  elif isinstance(type_, BasicType) and type_.typename == 'octet']@
            from collections.abc import ByteString
@[  elif isinstance(type_, BasicType) and type_.typename in CHARACTER_TYPES]@
            from collections import UserString
@[  end if]@
            assert \
@[  if isinstance(member.type, AbstractNestedType)]@
                ((isinstance(value, Sequence) or
                  isinstance(value, Set) or
                  isinstance(value, UserList)) and
                 not isinstance(value, str) and
                 not isinstance(value, UserString) and
@{assert_msg_suffixes = ['a set or sequence']}@
@[    if isinstance(type_, AbstractGenericString) and type_.has_maximum_size()]@
                 all(len(val) <= @(type_.maximum_size) for val in value) and
@{assert_msg_suffixes.append('and each string value not longer than %d' % type_.maximum_size)}@
@[    end if]@
@[    if isinstance(member.type, (Array, BoundedSequence))]@
@[      if isinstance(member.type, BoundedSequence)]@
                 len(value) <= @(member.type.maximum_size) and
@{assert_msg_suffixes.insert(1, 'with length <= %d' % member.type.maximum_size)}@
@[      else]@
                 len(value) == @(member.type.size) and
@{assert_msg_suffixes.insert(1, 'with length %d' % member.type.size)}@
@[      end if]@
@[    end if]@
                 all(isinstance(v, @(get_python_type(type_))) for v in value) and
@{assert_msg_suffixes.append("and each value of type '%s'" % get_python_type(type_))}@
@[    if isinstance(type_, BasicType) and type_.typename in SIGNED_INTEGER_TYPES]@
@{
nbits = int(type_.typename[3:])
bound = 2**(nbits - 1)
}@
                 all(val >= -@(bound) and val < @(bound) for val in value)), \
@{assert_msg_suffixes.append('and each integer in [%d, %d]' % (-bound, bound - 1))}@
@[    elif isinstance(type_, BasicType) and type_.typename in UNSIGNED_INTEGER_TYPES]@
@{
nbits = int(type_.typename[4:])
bound = 2**nbits
}@
                 all(val >= 0 and val < @(bound) for val in value)), \
@{assert_msg_suffixes.append('and each unsigned integer in [0, %d]' % (bound - 1))}@
@[    elif isinstance(type_, BasicType) and type_.typename == 'char']@
                 all(ord(val) >= 0 and ord(val) < 256 for val in value)), \
@{assert_msg_suffixes.append('and each char in [0, 255]')}@
@[    elif isinstance(type_, BasicType) and type_.typename in FLOATING_POINT_TYPES]@
@[      if type_.typename == "float"]@
@{
name = "float"
bound = 3.402823466e+38
}@
                 all(not (val < -@(bound) or val > @(bound)) or math.isinf(val) for val in value)), \
@{assert_msg_suffixes.append('and each float in [%f, %f]' % (-bound, bound))}@
@[      elif type_.typename == "double"]@
@{
name = "double"
bound = 1.7976931348623157e+308
}@
                 all(not (val < -@(bound) or val > @(bound)) or math.isinf(val) for val in value)), \
@{assert_msg_suffixes.append('and each double in [%f, %f]' % (-bound, bound))}@
@[      end if]@
@[    else]@
                 True), \
@[    end if]@
                "The '@(member.name)' field must be @(' '.join(assert_msg_suffixes))"
@[  elif isinstance(member.type, AbstractGenericString) and member.type.has_maximum_size()]@
                (isinstance(value, (str, UserString)) and
                 len(value) <= @(member.type.maximum_size)), \
                "The '@(member.name)' field must be string value " \
                'not longer than @(type_.maximum_size)'
@[  elif isinstance(type_, NamespacedType)]@
                isinstance(value, @(type_.name)), \
                "The '@(member.name)' field must be a sub message of type '@(type_.name)'"
@[  elif isinstance(type_, BasicType) and type_.typename == 'octet']@
                (isinstance(value, (bytes, ByteString)) and
                 len(value) == 1), \
                "The '@(member.name)' field must be of type 'bytes' or 'ByteString' with length 1"
@[  elif isinstance(type_, BasicType) and type_.typename == 'char']@
                (isinstance(value, (str, UserString)) and
                 len(value) == 1 and ord(value) >= -128 and ord(value) < 128), \
                "The '@(member.name)' field must be of type 'str' or 'UserString' " \
                'with length 1 and the character ord() in [-128, 127]'
@[  elif isinstance(type_, AbstractGenericString)]@
                isinstance(value, str), \
                "The '@(member.name)' field must be of type '@(get_python_type(type_))'"
@[  elif isinstance(type_, BasicType) and type_.typename in (BOOLEAN_TYPE, *FLOATING_POINT_TYPES, *INTEGER_TYPES)]@
                isinstance(value, @(get_python_type(type_))), \
                "The '@(member.name)' field must be of type '@(get_python_type(type_))'"
@[    if type_.typename in SIGNED_INTEGER_TYPES]@
@{
nbits = int(type_.typename[3:])
bound = 2**(nbits - 1)
}@
            assert value >= -@(bound) and value < @(bound), \
                "The '@(member.name)' field must be an integer in [@(-bound), @(bound - 1)]"
@[    elif type_.typename in UNSIGNED_INTEGER_TYPES]@
@{
nbits = int(type_.typename[4:])
bound = 2**nbits
}@
            assert value >= 0 and value < @(bound), \
                "The '@(member.name)' field must be an unsigned integer in [0, @(bound - 1)]"
@[    elif type_.typename in FLOATING_POINT_TYPES]@
@[      if type_.typename == "float"]@
@{
name = "float"
bound = 3.402823466e+38
}@
@[      elif type_.typename == "double"]@
@{
name = "double"
bound = 1.7976931348623157e+308
}@
@[      end if]@
            assert not (value < -@(bound) or value > @(bound)) or math.isinf(value), \
                "The '@(member.name)' field must be a @(name) in [@(-bound), @(bound)]"
@[    end if]@
@[  else]@
                False
@[  end if]@
@[  if isinstance(member.type, AbstractNestedType) and isinstance(member.type.value_type, BasicType) and member.type.value_type.typename in SPECIAL_NESTED_BASIC_TYPES]@
@[    if isinstance(member.type, Array)]@
        self._@(member.name) = numpy.array(value, dtype=@(SPECIAL_NESTED_BASIC_TYPES[member.type.value_type.typename]['dtype']))
@[    elif isinstance(member.type, AbstractSequence)]@
        self._@(member.name) = array.array('@(SPECIAL_NESTED_BASIC_TYPES[member.type.value_type.typename]['type_code'])', value)
@[    end if]@
@[  else]@
        self._@(member.name) = value
@[  end if]@
@[end for]@
