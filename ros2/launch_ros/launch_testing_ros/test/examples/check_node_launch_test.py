# Copyright 2021 Open Source Robotics Foundation, Inc.
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
import sys
import time
import unittest

import launch
import launch.actions
import launch_ros.actions
import launch_testing.actions
import launch_testing.markers
import pytest
import rclpy


@pytest.mark.launch_test
@launch_testing.markers.keep_alive
def generate_test_description():
    path_to_test = os.path.dirname(__file__)

    return launch.LaunchDescription([
        launch.actions.TimerAction(
            period=5.0,
            actions=[
                launch_ros.actions.Node(
                    executable=sys.executable,
                    arguments=[os.path.join(path_to_test, 'talker.py')],
                    additional_env={'PYTHONUNBUFFERED': '1'},
                    name='demo_node_1',
                )
            ]),
        launch_testing.actions.ReadyToTest()
    ])


class TestFixture(unittest.TestCase):

    def setUp(self):
        rclpy.init()
        self.node = rclpy.create_node('test_node')

    def tearDown(self):
        self.node.destroy_node()
        rclpy.shutdown()

    def test_node_start(self, proc_output):
        found = False
        print('Waiting for node...')
        # demo_node_1 won't start for at least 5 seconds after this test
        # is launched, so we wait for a total of up to 20 seconds for it
        # to appear.
        start = time.time()
        while time.time() - start < 20.0 and not found:
            found = 'demo_node_1' in self.node.get_node_names()
            time.sleep(0.1)

        assert found, 'Node not found!'
