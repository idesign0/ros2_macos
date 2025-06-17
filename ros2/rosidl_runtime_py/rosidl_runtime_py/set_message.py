# Copyright 2017-2019 Open Source Robotics Foundation, Inc.
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
from functools import partial

from typing import Any
from typing import Dict
from typing import List

import numpy

from rosidl_parser.definition import AbstractNestedType
from rosidl_parser.definition import NamespacedType
from rosidl_runtime_py.convert import get_message_slot_types
from rosidl_runtime_py.import_message import import_message_from_namespaced_type


def set_message_fields(
        msg: Any, values: Dict[str, str], expand_header_auto: bool = False,
        expand_time_now: bool = False) -> List[Any]:
    """
    Set the fields of a ROS message.

    :param msg: The ROS message to populate.
    :param values: The values to set in the ROS message. The keys of the dictionary represent
        fields of the message.
    :param expand_header_auto: If enabled and 'auto' is passed as a value to a
        'std_msgs.msg.Header' field, an empty Header will be instantiated and a setter function
        will be returned so that its 'stamp' field can be set to the current time.
    :param expand_time_now: If enabled and 'now' is passed as a value to a
        'builtin_interfaces.msg.Time' field, a setter function will be returned so that
        its value can be set to the current time.
    :returns: A list of setter functions that can be used to update 'builtin_interfaces.msg.Time'
        fields, useful for setting them to the current time. The list will be empty if the message
        does not have any 'builtin_interfaces.msg.Time' fields, or if expand_header_auto and
        expand_time_now are false.
    :raises AttributeError: If the message does not have a field provided in the input dictionary.
    :raises TypeError: If a message value does not match its field type.
    """
    timestamp_fields = []

    def set_message_fields_internal(
            msg: Any, values: Dict[str, str],
            timestamp_fields: List[Any]) -> List[Any]:
        try:
            items = values.items()
        except AttributeError:
            raise TypeError(
                "Value '%s' is expected to be a dictionary but is a %s" %
                (values, type(values).__name__))
        for field_name, field_value in items:
            field = getattr(msg, field_name)
            field_type = type(field)
            qualified_class_name = '{}.{}'.format(field_type.__module__, field_type.__name__)
            if field_type is array.array:
                value = field_type(field.typecode, field_value)
            elif field_type is numpy.ndarray:
                value = numpy.array(field_value, dtype=field.dtype)
            elif type(field_value) is field_type:
                value = field_value
            # We can't import these types directly, so we use the qualified class name to
            # distinguish them from other fields
            elif qualified_class_name == 'std_msgs.msg._header.Header' and \
                    field_value == 'auto' and expand_header_auto:
                timestamp_fields.append(partial(setattr, field, 'stamp'))
                continue
            elif qualified_class_name == 'builtin_interfaces.msg._time.Time' and \
                    field_value == 'now' and expand_time_now:
                timestamp_fields.append(partial(setattr, msg, field_name))
                continue
            else:
                try:
                    value = field_type(field_value)
                except TypeError:
                    value = field_type()
                    set_message_fields_internal(
                        value, field_value, timestamp_fields)
            rosidl_type = get_message_slot_types(msg)[field_name]
            # Check if field is an array of ROS messages
            if isinstance(rosidl_type, AbstractNestedType):
                if isinstance(rosidl_type.value_type, NamespacedType):
                    field_elem_type = import_message_from_namespaced_type(rosidl_type.value_type)
                    for n in range(len(value)):
                        submsg = field_elem_type()
                        set_message_fields_internal(
                            submsg, value[n], timestamp_fields)
                        value[n] = submsg
            setattr(msg, field_name, value)
    set_message_fields_internal(msg, values, timestamp_fields)
    return timestamp_fields
