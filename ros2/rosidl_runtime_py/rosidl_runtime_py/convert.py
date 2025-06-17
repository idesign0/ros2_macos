# Copyright 2016-2019 Open Source Robotics Foundation, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import array
from collections import OrderedDict
import sys
from typing import Any

import numpy
import rosidl_parser.definition
import yaml


__yaml_representer_registered = False


def __get_type_name(value_type):
    if isinstance(value_type, rosidl_parser.definition.BasicType):
        return value_type.typename
    elif isinstance(value_type, rosidl_parser.definition.AbstractString):
        return 'string'
    elif isinstance(value_type, rosidl_parser.definition.AbstractWString):
        return 'wstring'
    elif isinstance(value_type, rosidl_parser.definition.NamedType):
        return value_type.name
    elif isinstance(value_type, rosidl_parser.definition.NamespacedType):
        return '/'.join(value_type.namespaced_name())
    else:
        return 'unknown'


def __abbreviate_array_info(value, field_type):
    value_type_name = __get_type_name(field_type.value_type)
    if isinstance(field_type, rosidl_parser.definition.Array):
        return '<array type: {0}[{1}]>'.format(
            value_type_name, field_type.size)
    elif isinstance(field_type, rosidl_parser.definition.BoundedSequence):
        return '<sequence type: {0}[{1}], length: {2}>'.format(
            value_type_name, field_type.maximum_size, len(value))
    elif isinstance(field_type, rosidl_parser.definition.UnboundedSequence):
        return '<sequence type: {0}, length: {1}>'.format(
            value_type_name, len(value))
    return 'unknown'


# Custom representer for getting clean YAML output that preserves the order in an OrderedDict.
# Inspired by: http://stackoverflow.com/a/16782282/7169408
def __represent_ordereddict(dumper, data):
    items = []
    for k, v in data.items():
        items.append((dumper.represent_data(k), dumper.represent_data(v)))
    return yaml.nodes.MappingNode(u'tag:yaml.org,2002:map', items)


def message_to_yaml(
    msg: Any,
    *,
    truncate_length: int = None,
    no_arr: bool = False,
    no_str: bool = False,
    flow_style: bool = False
) -> str:
    """
    Convert a ROS message to a YAML string.

    :param msg: The ROS message to convert.
    :param truncate_length: Truncate values for all message fields to this length.
        This does not truncate the list of message fields.
    :param no_arr: Exclude array fields of the message.
    :param no_str: Exclude string fields of the message.
    :returns: A YAML string representation of the input ROS message.
    """
    global __yaml_representer_registered

    # Register our custom representer for YAML output
    if not __yaml_representer_registered:
        yaml.add_representer(OrderedDict, __represent_ordereddict)
        __yaml_representer_registered = True

    return yaml.dump(
        message_to_ordereddict(
            msg, truncate_length=truncate_length, no_arr=no_arr, no_str=no_str),
        allow_unicode=True, width=sys.maxsize, default_flow_style=flow_style,
    )


def message_to_csv(
    msg: Any,
    *,
    truncate_length: int = None,
    no_arr: bool = False,
    no_str: bool = False
) -> str:
    """
    Convert a ROS message to string of comma-separated values.

    :param msg: The ROS message to convert.
    :param truncate_length: Truncate values for all message fields to this length.
        This does not truncate the list of message fields.
    :param no_arr: Exclude array fields of the message.
    :param no_str: Exclude string fields of the message.
    :returns: A string of comma-separated values representing the input message.
    """
    def to_string(val, field_type=None):
        nonlocal truncate_length, no_arr, no_str
        r = ''
        if any(isinstance(val, t) for t in [list, tuple, array.array, numpy.ndarray]):
            if no_arr is True and field_type is not None:
                r = __abbreviate_array_info(val, field_type)
            else:
                for i, v in enumerate(val):
                    if r:
                        r += ','
                    if truncate_length is not None and i >= truncate_length:
                        r += '...'
                        break
                    r += to_string(v)
        elif any(isinstance(val, t) for t in [bool, bytes, float, int, str, numpy.number]):
            if no_str is True and isinstance(val, str):
                val = '<string length: <{0}>>'.format(len(val))
            elif any(isinstance(val, t) for t in [bytes, str]):
                if truncate_length is not None and len(val) > truncate_length:
                    val = val[:truncate_length]
                    if isinstance(val, bytes):
                        val += b'...'
                    else:
                        val += '...'
            r = str(val)
        else:
            r = message_to_csv(val, truncate_length=truncate_length, no_arr=no_arr, no_str=no_str)
        return r
    result = ''

    # We rely on __slots__ retaining the order of the fields in the .msg file.
    for field_name, field_type in zip(msg.__slots__, msg.SLOT_TYPES):
        value = getattr(msg, field_name)

        if result:
            result += ','

        result += to_string(value, field_type)
    return result


# Convert a msg to an OrderedDict. We do this instead of implementing a generic __dict__() method
# in the msg because we want to preserve order of fields from the .msg file(s).
def message_to_ordereddict(
    msg: Any,
    *,
    truncate_length: int = None,
    no_arr: bool = False,
    no_str: bool = False
) -> OrderedDict:
    """
    Convert a ROS message to an OrderedDict.

    :param msg: The ROS message to convert.
    :param truncate_length: Truncate values for all message fields to this length.
        This does not truncate the list of fields (ie. the dictionary keys).
    :param no_arr: Exclude array fields of the message.
    :param no_str: Exclude string fields of the message.
    :returns: An OrderedDict where the keys are the ROS message fields and the values are
        set to the values of the input message.
    """
    d = OrderedDict()

    # We rely on __slots__ retaining the order of the fields in the .msg file.
    for field_name, field_type in zip(msg.__slots__, msg.SLOT_TYPES):
        value = getattr(msg, field_name, None)

        value = _convert_value(
            value, field_type=field_type,
            truncate_length=truncate_length, no_arr=no_arr, no_str=no_str)
        # Remove leading underscore from field name
        d[field_name[1:]] = value
    return d


def _convert_value(
    value,
    *,
    field_type=None,
    truncate_length=None,
    no_arr=False,
    no_str=False
):

    if isinstance(value, bytes):
        if truncate_length is not None and len(value) > truncate_length:
            value = ''.join([chr(c) for c in value[:truncate_length]]) + '...'
        else:
            value = ''.join([chr(c) for c in value])
    elif isinstance(value, str):
        if no_str is True:
            value = '<string length: <{0}>>'.format(len(value))
        elif truncate_length is not None and len(value) > truncate_length:
            value = value[:truncate_length] + '...'
    elif isinstance(value, (list, tuple, array.array, numpy.ndarray)):
        # Since arrays and ndarrays can't contain mixed types convert to list
        typename = tuple if isinstance(value, tuple) else list
        if no_arr is True and field_type is not None:
            value = __abbreviate_array_info(value, field_type)
        elif truncate_length is not None and len(value) > truncate_length:
            # Truncate the sequence
            value = value[:truncate_length]
            # Truncate every item in the sequence
            value = typename(
                [_convert_value(v, truncate_length=truncate_length,
                                no_arr=no_arr, no_str=no_str) for v in value] + ['...'])
        else:
            # Truncate every item in the list
            value = typename(
                [_convert_value(v, truncate_length=truncate_length,
                                no_arr=no_arr, no_str=no_str) for v in value])
    elif isinstance(value, dict) or isinstance(value, OrderedDict):
        # Convert each key and value in the mapping
        new_value = {} if isinstance(value, dict) else OrderedDict()
        for k, v in value.items():
            # Don't truncate keys because that could result in key collisions and data loss
            new_value[_convert_value(k)] = _convert_value(
                v, truncate_length=truncate_length, no_arr=no_arr, no_str=no_str)
        value = new_value
    elif isinstance(value, numpy.number):
        value = value.item()
    elif not isinstance(value, (bool, float, int)):
        # Assuming value is a message since it is neither a collection nor a primitive type
        value = message_to_ordereddict(
            value, truncate_length=truncate_length, no_arr=no_arr, no_str=no_str)
    return value


def get_message_slot_types(msg: Any) -> OrderedDict:
    """
    Return an OrderedDict of the slot types of a message.

    :param msg: The ROS message to get members types from.
    :returns: An OrderedDict with message member names as keys and slot types as values.
    """
    return OrderedDict(zip([s[1:] for s in msg.__slots__], msg.SLOT_TYPES))
