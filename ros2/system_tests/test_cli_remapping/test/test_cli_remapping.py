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

import os
import random
import sys
import time

import unittest

from launch import LaunchDescription
from launch.actions import ExecuteProcess
import launch_testing
import launch_testing.actions

import rclpy


def get_environment_variable(name):
    """Get environment variable or raise if it does not exist."""
    path = os.getenv(name)
    if not path:
        raise EnvironmentError('Missing environment variable "%s"' % name)
    return path


CLIENT_LIBRARY_EXECUTABLES = (
    get_environment_variable('NAME_MAKER_RCLCPP'),
    get_environment_variable('NAME_MAKER_RCLPY')
)

TEST_CASES = {
    'namespace_replacement': (
        '/ns/s{random_string}/relative/name',
        '__ns:=/ns/s{random_string}'
    ),
    'node_name_replacement': (
        'node_{random_string}',
        '__node:=node_{random_string}'
    ),
    'topic_and_service_replacement': (
        '/remapped/ts{random_string}',
        '/fully/qualified/name:=/remapped/ts{random_string}'
    ),
    'topic_replacement': (
        '/remapped/t{random_string}',
        'rostopic://~/private/name:=/remapped/t{random_string}'
    ),
    'service_replacement': (
        '/remapped/s{random_string}',
        'rosservice://~/private/name:=/remapped/s{random_string}'
    )
}


@launch_testing.parametrize('executable', CLIENT_LIBRARY_EXECUTABLES)
def generate_test_description(executable):
    command = [executable]
    # Execute python files using same python used to start this test
    env = dict(os.environ)
    if command[0][-3:] == '.py':
        command.insert(0, sys.executable)
    env['PYTHONUNBUFFERED'] = '1'

    launch_description = LaunchDescription()

    test_context = {}
    for replacement_name, (replacement_value, cli_argument) in TEST_CASES.items():
        random_string = '%d_%s' % (
            random.randint(0, 9999), time.strftime('%H_%M_%S', time.gmtime()))
        launch_description.add_action(
            ExecuteProcess(
                cmd=command + ['--ros-args', '--remap', cli_argument.format(**locals())],
                name='name_maker_' + replacement_name, env=env
            )
        )
        test_context[replacement_name] = replacement_value.format(random_string=random_string)

    launch_description.add_action(
        launch_testing.actions.ReadyToTest()
    )

    return launch_description, test_context


class TestCLIRemapping(unittest.TestCase):

    ATTEMPTS = 10
    TIME_BETWEEN_ATTEMPTS = 1

    @classmethod
    def setUpClass(cls):
        rclpy.init()
        cls.node = rclpy.create_node('test_cli_remapping')

    @classmethod
    def tearDownClass(cls):
        cls.node.destroy_node()
        rclpy.shutdown()

    def get_topics(self):
        topic_names_and_types = self.node.get_topic_names_and_types()
        return [name for name, _ in topic_names_and_types]

    def get_services(self):
        service_names_and_types = self.node.get_service_names_and_types()
        return [name for name, _ in service_names_and_types]

    def test_namespace_replacement(self, namespace_replacement):
        for attempt in range(self.ATTEMPTS):
            if (
                namespace_replacement in self.get_topics() and
                namespace_replacement in self.get_services()
            ):
                break
            time.sleep(self.TIME_BETWEEN_ATTEMPTS)
            rclpy.spin_once(self.node, timeout_sec=0)
        self.assertIn(namespace_replacement, self.get_topics())
        self.assertIn(namespace_replacement, self.get_services())

    def test_node_name_replacement(self, node_name_replacement):
        for attempt in range(self.ATTEMPTS):
            if node_name_replacement in self.node.get_node_names():
                break
            time.sleep(self.TIME_BETWEEN_ATTEMPTS)
            rclpy.spin_once(self.node, timeout_sec=0)
        self.assertIn(node_name_replacement, self.node.get_node_names())

    def test_topic_and_service_replacement(self, topic_and_service_replacement):
        for attempt in range(self.ATTEMPTS):
            if (
                topic_and_service_replacement in self.get_topics() and
                topic_and_service_replacement in self.get_services()
            ):
                break
            time.sleep(self.TIME_BETWEEN_ATTEMPTS)
            rclpy.spin_once(self.node, timeout_sec=0)
        self.assertIn(topic_and_service_replacement, self.get_topics())
        self.assertIn(topic_and_service_replacement, self.get_services())

    def test_topic_replacement(self, topic_replacement):
        for attempt in range(self.ATTEMPTS):
            if topic_replacement in self.get_topics():
                break
            time.sleep(self.TIME_BETWEEN_ATTEMPTS)
            rclpy.spin_once(self.node, timeout_sec=0)
        self.assertIn(topic_replacement, self.get_topics())
        self.assertNotIn(topic_replacement, self.get_services())

    def test_service_replacement(self, service_replacement):
        for attempt in range(self.ATTEMPTS):
            if service_replacement in self.get_services():
                break
            time.sleep(self.TIME_BETWEEN_ATTEMPTS)
            rclpy.spin_once(self.node, timeout_sec=0)
        self.assertNotIn(service_replacement, self.get_topics())
        self.assertIn(service_replacement, self.get_services())


@launch_testing.post_shutdown_test()
class TestCLIRemappingAfterShutdown(unittest.TestCase):

    def test_processes_finished_gracefully(self, proc_info):
        """Test that both executables finished gracefully."""
        launch_testing.asserts.assertExitCodes(proc_info)
