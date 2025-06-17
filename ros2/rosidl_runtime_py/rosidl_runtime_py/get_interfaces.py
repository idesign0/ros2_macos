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
from typing import Dict
from typing import Iterable
from typing import List

from ament_index_python import get_resource
from ament_index_python import get_resources
from ament_index_python import has_resource


def get_interface_packages() -> List[str]:
    """Get all packages that generate interfaces."""
    return get_resources('rosidl_interfaces')


def _get_interfaces(package_names: Iterable[str] = []) -> Dict[str, List[str]]:
    interfaces = {}
    if len(package_names) == 0:
        package_names = get_resources('rosidl_interfaces')

    for package_name in package_names:
        if not has_resource('packages', package_name):
            raise LookupError(f"Unknown package '{package_name}'")
        try:
            content, _ = get_resource('rosidl_interfaces', package_name)
        except LookupError:
            continue
        interfaces[package_name] = content.splitlines()
    return interfaces


def get_interfaces(package_names: Iterable[str] = []) -> Dict[str, List[str]]:
    """
    Get interfaces for one or more packages.

    If a package does not contain any interfaces, then it will not exist in the returned
    dictionary.

    :param package_names: Interfaces are returned for these packages.
      If no package names are provided, then this function returns interfaces for all packages.
    :return: A dictionary where keys are package names and values are lists of interface names.
    :raises LookupError: If one or more packages can not be found.
    """
    interfaces = _get_interfaces(package_names)
    # filter out hidden interfaces
    filtered_interfaces = {}
    for package_name, interface_names in interfaces.items():
        _interfaces = list({
            interface_name.rsplit('.', 1)[0]
            for interface_name in interface_names
            if '_' not in interface_name
        })
        if _interfaces:
            filtered_interfaces[package_name] = _interfaces
    return filtered_interfaces


def get_message_interfaces(package_names: Iterable[str] = []) -> Dict[str, List[str]]:
    """
    Get message interfaces for one or more packages.

    If a package does not contain any message interfaces, then it will not exist in the returned
    dictionary.

    :param package_names: Message interfaces are returned for these packages.
      If no package names are provided, then this function returns interfaces for all packages.
    :return: A dictionary where keys are package names and values are lists of interface names.
    :raises LookupError: If one or more packages can not be found.
    """
    interfaces = _get_interfaces(package_names)
    # filter out hidden interfaces and identify message interfaces by the namespace and suffix
    filtered_interfaces = {}
    for package_name, interface_names in interfaces.items():
        message_interfaces = list({
            interface_name[:-4]
            for interface_name in interface_names
            if '_' not in interface_name and
            interface_name.startswith('msg/') and
            interface_name[-4:] in ('.idl', '.msg')
        })
        if message_interfaces:
            filtered_interfaces[package_name] = message_interfaces
    return filtered_interfaces


def get_service_interfaces(package_names: Iterable[str] = []) -> Dict[str, List[str]]:
    """
    Get service interfaces for one or more packages.

    If a package does not contain any service interfaces, then it will not exist in the returned
    dictionary.

    :param package_names: Service interfaces are returned for these packages.
      If no package names are provided, then this function returns interfaces for all packages.
    :return: A dictionary where keys are package names and values are lists of interface names.
    :raises LookupError: If one or more packages can not be found.
    """
    interfaces = _get_interfaces(package_names)
    # filter out hidden interfaces and identify service interfaces by the namespace and suffix
    filtered_interfaces = {}
    for package_name, interface_names in interfaces.items():
        service_interfaces = list({
            interface_name[:-4]
            for interface_name in interface_names
            if '_' not in interface_name and
            interface_name.startswith('srv/') and
            interface_name[-4:] in ('.idl', '.srv')
        })
        if service_interfaces:
            filtered_interfaces[package_name] = service_interfaces
    return filtered_interfaces


def get_action_interfaces(package_names: Iterable[str] = []) -> Dict[str, List[str]]:
    """
    Get action interfaces for one or more packages.

    If a package does not contain any action interfaces, then it will not exist in the returned
    dictionary.

    :param package_names: Action interfaces are returned for these packages.
      If no package names are provided, then this function returns interfaces for all packages.
    :return: A dictionary where keys are package names and values are lists of interface names.
    :raises LookupError: If one or more packages can not be found.
    """
    interfaces = _get_interfaces(package_names)
    # filter out hidden interfaces and identify action interfaces by the namespace and suffix
    filtered_interfaces = {}
    for package_name, interface_names in interfaces.items():
        action_interfaces = list({
            interface_name.rsplit('.', 1)[0]
            for interface_name in interface_names
            if '_' not in interface_name and
            interface_name.startswith('action/') and
            (interface_name[-4:] == '.idl' or interface_name[-7:] == '.action')
        })
        if action_interfaces:
            filtered_interfaces[package_name] = action_interfaces
    return filtered_interfaces


def get_interface_path(interface_name: str) -> str:
    """
    Get the path to an interface definition file.

    :param interface_name: The name of the interface (e.g. builtin_interfaces/msg/Time.msg)
      If no dot-separated suffix is provided, then it is inferred from the namespace.
    :return: The path to the interface definition file.
    :raises ValueError: If the interface name is malformed.
    :raises LookupError: If the package or interface cannot be found.
    """
    # Split up the name for analysis
    parts = interface_name.split('/')
    # By convention we expect there to be at least two parts to the interface
    if len(parts) < 2:
        raise ValueError(
            f"Invalid name '{interface_name}'. Expected at least two parts separated by '/'")
    if not all(parts):
        raise ValueError(f"Invalid name '{interface_name}'. Must not contain empty parts")
    if '..' in parts:
        raise ValueError(f"Invalid name '{interface_name}'. Must not contain '..'")
    # By convention we expect the first part to be the package name
    prefix_path = has_resource('packages', parts[0])
    if not prefix_path:
        raise LookupError(f"Unknown package '{parts[0]}'")

    interface_path = os.path.join(prefix_path, 'share', interface_name)
    # Check if there is a dot-separated suffix
    if len(parts[-1].rsplit('.', 1)) == 1:
        # If there is no suffix, try appending parent namespace (e.g. '.msg', '.srv', '.action')
        interface_path_with_suffix = interface_path + '.' + parts[-2]
        if os.path.exists(interface_path_with_suffix):
            return interface_path_with_suffix
        # Finally, try appending '.idl'
        interface_path += '.idl'

    if not os.path.exists(interface_path):
        raise LookupError(f"Could not find the interface '{interface_path}'")
    return interface_path
