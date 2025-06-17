# Copyright 2018 Open Source Robotics Foundation, Inc.
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
from rcl_interfaces.msg import ParameterType
from rcl_interfaces.srv import GetParameters
import rclpy

from .utils import BackgroundExecutor
from .utils import HelperCommand
from .utils import require_environment_variable
from .utils import TemporaryFileWithContent

CLIENT_LIBRARY_EXECUTABLES = (
    require_environment_variable('INITIAL_PARAMS_RCLCPP'),
    require_environment_variable('INITIAL_PARAMS_RCLPY'),
)


@pytest.fixture(scope='module', params=CLIENT_LIBRARY_EXECUTABLES)
def node_fixture(request):
    """Create a fixture with a node and helper executable."""
    rclpy.init()
    node = rclpy.create_node(
        'tests_yaml',
        allow_undeclared_parameters=True,
        automatically_declare_parameters_from_overrides=True)
    try:
        yield {
            'node': node,
            'executable': request.param,
        }
    finally:
        node.destroy_node()
        rclpy.shutdown()


def get_params(node, node_name, param_names, wfs_timeout=5.0):
    client = node.create_client(GetParameters, '/{name}/get_parameters'.format(name=node_name))
    resp = None
    with BackgroundExecutor(node):
        assert client.wait_for_service(timeout_sec=wfs_timeout)
        request = GetParameters.Request()
        request.names = param_names
        resp = client.call(request)

    # Don't destroy client while spinning ros2/rmw_fastrtps#205
    node.destroy_client(client)
    return resp


def test_bool_params(node_fixture):
    param_file_content = """
bool_params:
    ros__parameters:
        b1: False
        b2: True
"""
    with TemporaryFileWithContent(param_file_content) as yaml_file:
        command = (
            node_fixture['executable'],
            '--ros-args',
            '--remap',
            '__node:=bool_params',
            '--params-file', yaml_file
        )

        with HelperCommand(command):
            resp = get_params(node_fixture['node'], 'bool_params', ['b1', 'b2'])

        assert 2 == len(resp.values)
        assert ParameterType.PARAMETER_BOOL == resp.values[0].type
        assert ParameterType.PARAMETER_BOOL == resp.values[1].type
        assert not resp.values[0].bool_value
        assert resp.values[1].bool_value


def test_integer_params(node_fixture):
    param_file_content = """
int_params:
    ros__parameters:
        i1: 42
        i2: -27
"""
    with TemporaryFileWithContent(param_file_content) as yaml_file:
        command = (
            node_fixture['executable'],
            '--ros-args',
            '--remap',
            '__node:=int_params',
            '--params-file', yaml_file
        )

        with HelperCommand(command):
            resp = get_params(node_fixture['node'], 'int_params', ['i1', 'i2'])

        assert 2 == len(resp.values)
        assert ParameterType.PARAMETER_INTEGER == resp.values[0].type
        assert ParameterType.PARAMETER_INTEGER == resp.values[1].type
        assert 42 == resp.values[0].integer_value
        assert -27 == resp.values[1].integer_value


def test_double_params(node_fixture):
    param_file_content = """
double_params:
    ros__parameters:
        d1: 3.14
        d2: -2.718
"""
    with TemporaryFileWithContent(param_file_content) as yaml_file:
        command = (
            node_fixture['executable'],
            '--ros-args',
            '--remap',
            '__node:=double_params',
            '--params-file', yaml_file
        )

        with HelperCommand(command):
            resp = get_params(node_fixture['node'], 'double_params', ['d1', 'd2'])

        assert 2 == len(resp.values)
        assert ParameterType.PARAMETER_DOUBLE == resp.values[0].type
        assert ParameterType.PARAMETER_DOUBLE == resp.values[1].type
        assert pytest.approx(3.14) == resp.values[0].double_value
        assert pytest.approx(-2.718) == resp.values[1].double_value


def test_string_params(node_fixture):
    param_file_content = """
str_params:
    ros__parameters:
        s1: hello
        s2: world
"""
    with TemporaryFileWithContent(param_file_content) as yaml_file:
        command = (
            node_fixture['executable'],
            '--ros-args',
            '--remap',
            '__node:=str_params',
            '--params-file', yaml_file
        )

        with HelperCommand(command):
            resp = get_params(node_fixture['node'], 'str_params', ['s1', 's2'])

        assert 2 == len(resp.values)
        assert ParameterType.PARAMETER_STRING == resp.values[0].type
        assert ParameterType.PARAMETER_STRING == resp.values[1].type
        assert resp.values[0].string_value == 'hello'
        assert resp.values[1].string_value == 'world'


# TODO(sloretz) PARAMETER_BYTE_ARRAY when rcl_yaml_param_parser supports it


def test_bool_array_params(node_fixture):
    param_file_content = """
ba_params:
    ros__parameters:
        ba1: [true, false]
        ba2: [false, true]
"""
    with TemporaryFileWithContent(param_file_content) as yaml_file:
        command = (
            node_fixture['executable'],
            '--ros-args',
            '--remap',
            '__node:=ba_params',
            '--params-file', yaml_file
        )

        with HelperCommand(command):
            resp = get_params(node_fixture['node'], 'ba_params', ['ba1', 'ba2'])

        assert 2 == len(resp.values)
        assert ParameterType.PARAMETER_BOOL_ARRAY == resp.values[0].type
        assert ParameterType.PARAMETER_BOOL_ARRAY == resp.values[1].type
        assert resp.values[0].bool_array_value == [True, False]
        assert resp.values[1].bool_array_value == [False, True]


def test_integer_array_params(node_fixture):
    param_file_content = """
ia_params:
    ros__parameters:
        ia1: [42, -27]
        ia2: [1234, 5678]
"""
    with TemporaryFileWithContent(param_file_content) as yaml_file:
        command = (
            node_fixture['executable'],
            '--ros-args',
            '--remap',
            '__node:=ia_params',
            '--params-file', yaml_file
        )

        with HelperCommand(command):
            resp = get_params(node_fixture['node'], 'ia_params', ['ia1', 'ia2'])

        assert 2 == len(resp.values)
        assert ParameterType.PARAMETER_INTEGER_ARRAY == resp.values[0].type
        assert ParameterType.PARAMETER_INTEGER_ARRAY == resp.values[1].type
        assert resp.values[0].integer_array_value.tolist() == [42, -27]
        assert resp.values[1].integer_array_value.tolist() == [1234, 5678]


def test_double_array_params(node_fixture):
    param_file_content = """
da_params:
    ros__parameters:
        da1: [3.14, -2.718]
        da2: [1234.5, -9999.0]
"""
    with TemporaryFileWithContent(param_file_content) as yaml_file:
        command = (
            node_fixture['executable'],
            '--ros-args',
            '--remap',
            '__node:=da_params',
            '--params-file', yaml_file
        )

        with HelperCommand(command):
            resp = get_params(node_fixture['node'], 'da_params', ['da1', 'da2'])

        assert 2 == len(resp.values)
        assert ParameterType.PARAMETER_DOUBLE_ARRAY == resp.values[0].type
        assert ParameterType.PARAMETER_DOUBLE_ARRAY == resp.values[1].type
        assert resp.values[0].double_array_value == pytest.approx([3.14, -2.718])
        assert resp.values[1].double_array_value == pytest.approx([1234.5, -9999.0])


def test_string_array_params(node_fixture):
    param_file_content = """
sa_params:
    ros__parameters:
        sa1: ['Four', 'score']
        sa2: ['and', 'seven']
"""
    with TemporaryFileWithContent(param_file_content) as yaml_file:
        command = (
            node_fixture['executable'],
            '--ros-args',
            '--remap',
            '__node:=sa_params',
            '--params-file', yaml_file
        )

        with HelperCommand(command):
            resp = get_params(node_fixture['node'], 'sa_params', ['sa1', 'sa2'])

        assert 2 == len(resp.values)
        assert ParameterType.PARAMETER_STRING_ARRAY == resp.values[0].type
        assert ParameterType.PARAMETER_STRING_ARRAY == resp.values[1].type
        assert resp.values[0].string_array_value == ['Four', 'score']
        assert resp.values[1].string_array_value == ['and', 'seven']


def test_multiple_parameter_files(node_fixture):
    first_yaml_content = """
multi_params:
    ros__parameters:
        i1: 42
        i2: -27
"""
    second_yaml_content = """
multi_params:
    ros__parameters:
        i2: 12345
        i3: -27
"""

    with TemporaryFileWithContent(first_yaml_content) as first_yaml_file:
        with TemporaryFileWithContent(second_yaml_content) as second_yaml_file:
            command = (
                node_fixture['executable'],
                '--ros-args',
                '--remap',
                '__node:=multi_params'
                '--params-file', first_yaml_file,
                '--params-file', second_yaml_file
            )
            with HelperCommand(command):
                resp = get_params(node_fixture['node'], 'multi_params', ['i1', 'i2', 'i3'])

            assert 3 == len(resp.values)
            assert ParameterType.PARAMETER_INTEGER == resp.values[0].type
            assert ParameterType.PARAMETER_INTEGER == resp.values[1].type
            assert ParameterType.PARAMETER_INTEGER == resp.values[2].type
            assert 42 == resp.values[0].integer_value
            assert 12345 == resp.values[1].integer_value
            assert -27 == resp.values[2].integer_value
