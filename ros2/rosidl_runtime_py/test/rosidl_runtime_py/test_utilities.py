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

import pytest

from rosidl_runtime_py import utilities
from rosidl_runtime_py.import_message import import_message_from_namespaced_type

import test_msgs.action
import test_msgs.msg
import test_msgs.srv


class Config:

    def __init__(
        self,
        *,
        path,
        object_type=None,
        to_namespaced_type_function=None,
        is_function=None
    ):
        self.path = path
        self.object_type = object_type
        self.to_namespaced_type_function = to_namespaced_type_function
        self.is_function = is_function


@pytest.mark.parametrize('config', (
    Config(
        path='test_msgs/msg/Empty',
        object_type=test_msgs.msg.Empty,
        to_namespaced_type_function=utilities.get_message_namespaced_type,
        is_function=utilities.is_message
    ),
    Config(
        path='test_msgs/Empty',
        object_type=test_msgs.msg.Empty,
        to_namespaced_type_function=utilities.get_message_namespaced_type,
        is_function=utilities.is_message
    ),
    Config(
        path='test_msgs/srv/Empty',
        object_type=test_msgs.srv.Empty,
        to_namespaced_type_function=utilities.get_service_namespaced_type,
        is_function=utilities.is_service
    ),
    Config(
        path='test_msgs/Empty',
        object_type=test_msgs.srv.Empty,
        to_namespaced_type_function=utilities.get_service_namespaced_type,
        is_function=utilities.is_service
    ),
    Config(
        path='test_msgs/action/Fibonacci',
        object_type=test_msgs.action.Fibonacci,
        to_namespaced_type_function=utilities.get_action_namespaced_type,
        is_function=utilities.is_action
    ),
    Config(
        path='test_msgs/Fibonacci',
        object_type=test_msgs.action.Fibonacci,
        to_namespaced_type_function=utilities.get_action_namespaced_type,
        is_function=utilities.is_action
    ),
))
def test_get_namespaced_type_functions(config):
    message = import_message_from_namespaced_type(config.to_namespaced_type_function(config.path))
    assert message is config.object_type
    assert config.is_function(message)


def test_is_interface_functions():
    assert utilities.is_message(test_msgs.msg.Empty)
    assert not utilities.is_message(test_msgs.srv.Empty)
    assert not utilities.is_message(test_msgs.action.Fibonacci)
    assert not utilities.is_service(test_msgs.msg.Empty)
    assert utilities.is_service(test_msgs.srv.Empty)
    assert not utilities.is_service(test_msgs.action.Fibonacci)
    assert not utilities.is_action(test_msgs.msg.Empty)
    assert not utilities.is_action(test_msgs.srv.Empty)
    assert utilities.is_action(test_msgs.action.Fibonacci)


@pytest.mark.parametrize('function,identifier,result', (
    (utilities.get_interface, 'test_msgs/msg/Empty', test_msgs.msg.Empty),
    (utilities.get_interface, 'test_msgs/srv/Empty', test_msgs.srv.Empty),
    (utilities.get_interface, 'test_msgs/action/Fibonacci', test_msgs.action.Fibonacci),
    (utilities.get_message, 'test_msgs/msg/Empty', test_msgs.msg.Empty),
    (utilities.get_message, 'test_msgs/Empty', test_msgs.msg.Empty),
    (utilities.get_service, 'test_msgs/srv/Empty', test_msgs.srv.Empty),
    (utilities.get_service, 'test_msgs/Empty', test_msgs.srv.Empty),
    (utilities.get_action, 'test_msgs/action/Fibonacci', test_msgs.action.Fibonacci),
    (utilities.get_action, 'test_msgs/Fibonacci', test_msgs.action.Fibonacci),
))
def test_get_interface_functions(function, identifier, result):
    assert function(identifier) is result


@pytest.mark.parametrize('function,identifier,interface_type', (
    (utilities.get_message, 'test_msgs/srv/Empty', 'a message'),
    (utilities.get_message, 'test_msgs/action/Fibonacci', 'a message'),
    (utilities.get_service, 'test_msgs/msg/Empty', 'a service'),
    (utilities.get_service, 'test_msgs/action/Fibonacci', 'a service'),
    (utilities.get_action, 'test_msgs/msg/Empty', 'an action'),
    (utilities.get_action, 'test_msgs/srv/Empty', 'an action'),
))
def test_get_interface_raises(function, identifier, interface_type):
    with pytest.raises(ValueError) as ex:
        function(identifier)
    ex.match("Expected the full name of {}, got '{}'".format(interface_type, identifier))
