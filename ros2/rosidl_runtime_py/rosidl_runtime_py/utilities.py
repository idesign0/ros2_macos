# Copyright 2019 Open Source Robotics Foundation, Inc.
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

from typing import Text

from rosidl_parser.definition import NamespacedType
from rosidl_runtime_py.import_message import import_message_from_namespaced_type


def get_interface(identifier: Text):
    """Get an interface from its full name."""
    return import_message_from_namespaced_type(get_namespaced_type(identifier))


def get_message(identifier: Text):
    """Get a message from its full name."""
    interface = import_message_from_namespaced_type(get_message_namespaced_type(identifier))
    if not is_message(interface):
        raise ValueError("Expected the full name of a message, got '{}'".format(identifier))
    return interface


def get_service(identifier: Text):
    """Get a service from its full name."""
    interface = import_message_from_namespaced_type(get_service_namespaced_type(identifier))
    if not is_service(interface):
        raise ValueError("Expected the full name of a service, got '{}'".format(identifier))
    return interface


def get_action(identifier: Text):
    """Get a message from its full name."""
    interface = import_message_from_namespaced_type(get_action_namespaced_type(identifier))
    if not is_action(interface):
        raise ValueError("Expected the full name of an action, got '{}'".format(identifier))
    return interface


def get_namespaced_type(identifier: Text):
    """Create a `NamespacedType` object from the full name of an interface."""
    return _get_namespaced_type(identifier)


def get_message_namespaced_type(identifier: Text):
    """Create a `NamespacedType` object from the full name of a message."""
    return _get_namespaced_type(identifier, default_namespace='msg')


def get_service_namespaced_type(identifier: Text):
    """Create a `NamespacedType` object from the full name of a service."""
    return _get_namespaced_type(identifier, default_namespace='srv')


def get_action_namespaced_type(identifier: Text):
    """Create a `NamespacedType` object from the full name of an action."""
    return _get_namespaced_type(identifier, default_namespace='action')


def is_message(interface):
    """Return `True` if `interface` is a message."""
    return hasattr(interface, 'SLOT_TYPES')


def is_service(interface):
    """Return `True` if `interface` is a service."""
    return hasattr(interface, 'Request') and hasattr(interface, 'Response')


def is_action(interface):
    """Return `True` if `interface` is an action."""
    return (
        hasattr(interface, 'Goal') and
        hasattr(interface, 'Result') and
        hasattr(interface, 'Feedback')
    )


def _get_namespaced_type(identifier: Text, *, default_namespace=None):
    namespace = identifier.split(sep='/')
    name = namespace[-1]
    namespace = namespace[0:-1]
    if len(namespace) == 1 and default_namespace is not None:
        namespace.append(default_namespace)
    return NamespacedType(namespace, name)
