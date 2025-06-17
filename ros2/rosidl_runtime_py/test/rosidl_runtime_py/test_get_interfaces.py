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

import os

import pytest

from rosidl_runtime_py import get_action_interfaces
from rosidl_runtime_py import get_interface_packages
from rosidl_runtime_py import get_interface_path
from rosidl_runtime_py import get_interfaces
from rosidl_runtime_py import get_message_interfaces
from rosidl_runtime_py import get_service_interfaces

# these packages are listed as dependencies in the package.xml
INTERFACE_PACKAGE = 'test_msgs'
MESSAGE_INTERFACE_ONLY_PACKAGE = 'std_msgs'
SERVICE_INTERFACE_ONLY_PACKAGE = 'std_srvs'
NON_INTERFACE_PACKAGE = 'rosidl_parser'


def test_get_interface_packages():
    packages = get_interface_packages()
    assert len(packages) > 0
    assert INTERFACE_PACKAGE in packages
    assert MESSAGE_INTERFACE_ONLY_PACKAGE in packages
    assert SERVICE_INTERFACE_ONLY_PACKAGE in packages
    assert NON_INTERFACE_PACKAGE not in packages


def test_get_interfaces():
    # no input
    interfaces = get_interfaces()
    assert len(interfaces) > 0
    assert INTERFACE_PACKAGE in interfaces
    assert NON_INTERFACE_PACKAGE not in interfaces
    assert len(interfaces[INTERFACE_PACKAGE]) > 0

    # one package
    interfaces = get_interfaces([INTERFACE_PACKAGE])
    assert len(interfaces) > 0
    assert INTERFACE_PACKAGE in interfaces
    assert NON_INTERFACE_PACKAGE not in interfaces
    assert len(interfaces[INTERFACE_PACKAGE]) > 0

    # multiple packages
    interfaces = get_interfaces([
        INTERFACE_PACKAGE,
        MESSAGE_INTERFACE_ONLY_PACKAGE,
        SERVICE_INTERFACE_ONLY_PACKAGE,
        NON_INTERFACE_PACKAGE
    ])
    assert len(interfaces) > 0
    assert INTERFACE_PACKAGE in interfaces
    assert MESSAGE_INTERFACE_ONLY_PACKAGE in interfaces
    assert SERVICE_INTERFACE_ONLY_PACKAGE in interfaces
    assert NON_INTERFACE_PACKAGE not in interfaces
    assert len(interfaces[INTERFACE_PACKAGE]) > 0

    # unknown package name
    with pytest.raises(LookupError):
        get_interfaces(['test_not_package_name_you_are_bad_to_use_this_name'])


def test_get_message_interfaces():
    # no input
    interfaces = get_message_interfaces()
    assert len(interfaces) > 0
    assert INTERFACE_PACKAGE in interfaces
    assert MESSAGE_INTERFACE_ONLY_PACKAGE in interfaces
    assert SERVICE_INTERFACE_ONLY_PACKAGE not in interfaces
    assert NON_INTERFACE_PACKAGE not in interfaces
    assert len(interfaces[INTERFACE_PACKAGE]) > 0

    # one package
    interfaces = get_message_interfaces([INTERFACE_PACKAGE])
    assert len(interfaces) > 0
    assert INTERFACE_PACKAGE in interfaces
    assert MESSAGE_INTERFACE_ONLY_PACKAGE not in interfaces
    assert SERVICE_INTERFACE_ONLY_PACKAGE not in interfaces
    assert NON_INTERFACE_PACKAGE not in interfaces
    assert len(interfaces[INTERFACE_PACKAGE]) > 0

    # multiple packages
    interfaces = get_message_interfaces([
        INTERFACE_PACKAGE,
        MESSAGE_INTERFACE_ONLY_PACKAGE,
        SERVICE_INTERFACE_ONLY_PACKAGE,
        NON_INTERFACE_PACKAGE
    ])
    assert len(interfaces) > 0
    assert INTERFACE_PACKAGE in interfaces
    assert MESSAGE_INTERFACE_ONLY_PACKAGE in interfaces
    assert SERVICE_INTERFACE_ONLY_PACKAGE not in interfaces
    assert NON_INTERFACE_PACKAGE not in interfaces
    assert len(interfaces[INTERFACE_PACKAGE]) > 0

    # unknown package name
    with pytest.raises(LookupError):
        get_message_interfaces(['test_not_package_name_you_are_bad_to_use_this_name'])


def test_get_service_interfaces():
    # no input
    interfaces = get_service_interfaces()
    assert len(interfaces) > 0
    assert INTERFACE_PACKAGE in interfaces
    assert MESSAGE_INTERFACE_ONLY_PACKAGE not in interfaces
    assert SERVICE_INTERFACE_ONLY_PACKAGE in interfaces
    assert NON_INTERFACE_PACKAGE not in interfaces
    assert len(interfaces[INTERFACE_PACKAGE]) > 0

    # one package
    interfaces = get_service_interfaces([INTERFACE_PACKAGE])
    assert len(interfaces) > 0
    assert INTERFACE_PACKAGE in interfaces
    assert MESSAGE_INTERFACE_ONLY_PACKAGE not in interfaces
    assert SERVICE_INTERFACE_ONLY_PACKAGE not in interfaces
    assert NON_INTERFACE_PACKAGE not in interfaces
    assert len(interfaces[INTERFACE_PACKAGE]) > 0

    # multiple packages
    interfaces = get_service_interfaces([
        INTERFACE_PACKAGE,
        MESSAGE_INTERFACE_ONLY_PACKAGE,
        SERVICE_INTERFACE_ONLY_PACKAGE,
        NON_INTERFACE_PACKAGE
    ])
    assert len(interfaces) > 0
    assert INTERFACE_PACKAGE in interfaces
    assert MESSAGE_INTERFACE_ONLY_PACKAGE not in interfaces
    assert SERVICE_INTERFACE_ONLY_PACKAGE in interfaces
    assert NON_INTERFACE_PACKAGE not in interfaces
    assert len(interfaces[INTERFACE_PACKAGE]) > 0

    # unknown package name
    with pytest.raises(LookupError):
        get_service_interfaces(['test_not_package_name_you_are_bad_to_use_this_name'])


def test_get_action_interfaces():
    # no input
    interfaces = get_action_interfaces()
    assert len(interfaces) > 0
    assert INTERFACE_PACKAGE in interfaces
    assert MESSAGE_INTERFACE_ONLY_PACKAGE not in interfaces
    assert SERVICE_INTERFACE_ONLY_PACKAGE not in interfaces
    assert NON_INTERFACE_PACKAGE not in interfaces
    assert len(interfaces[INTERFACE_PACKAGE]) > 0

    # one package
    interfaces = get_action_interfaces([INTERFACE_PACKAGE])
    assert len(interfaces) > 0
    assert INTERFACE_PACKAGE in interfaces
    assert MESSAGE_INTERFACE_ONLY_PACKAGE not in interfaces
    assert SERVICE_INTERFACE_ONLY_PACKAGE not in interfaces
    assert NON_INTERFACE_PACKAGE not in interfaces
    assert len(interfaces[INTERFACE_PACKAGE]) > 0

    # multiple packages
    interfaces = get_action_interfaces([INTERFACE_PACKAGE, NON_INTERFACE_PACKAGE])
    assert len(interfaces) > 0
    assert INTERFACE_PACKAGE in interfaces
    assert MESSAGE_INTERFACE_ONLY_PACKAGE not in interfaces
    assert SERVICE_INTERFACE_ONLY_PACKAGE not in interfaces
    assert NON_INTERFACE_PACKAGE not in interfaces
    assert len(interfaces[INTERFACE_PACKAGE]) > 0

    # unknown package name
    with pytest.raises(LookupError):
        get_action_interfaces(['test_not_package_name_you_are_bad_to_use_this_name'])


def test_get_interface_path():
    # get message with suffix
    interface_path = get_interface_path('test_msgs/msg/BasicTypes.msg')
    assert os.path.exists(interface_path)
    assert interface_path[-4:] == '.msg'

    # get message without suffix
    interface_path = get_interface_path('test_msgs/msg/BasicTypes')
    assert os.path.exists(interface_path)
    assert interface_path[-4:] == '.msg'

    # get service with suffix
    interface_path = get_interface_path('test_msgs/srv/BasicTypes.srv')
    assert os.path.exists(interface_path)
    assert interface_path[-4:] == '.srv'

    # get service without suffix
    interface_path = get_interface_path('test_msgs/srv/BasicTypes')
    assert os.path.exists(interface_path)
    assert interface_path[-4:] == '.srv'

    # get action with suffix
    interface_path = get_interface_path('test_msgs/action/Fibonacci.action')
    assert os.path.exists(interface_path)
    assert interface_path[-7:] == '.action'

    # get action without suffix
    interface_path = get_interface_path('test_msgs/action/Fibonacci')
    assert os.path.exists(interface_path)
    assert interface_path[-7:] == '.action'

    # get message with .idl suffix
    interface_path = get_interface_path('test_msgs/msg/BasicTypes.idl')
    assert os.path.exists(interface_path)
    assert interface_path[-4:] == '.idl'
