# Copyright 2020 Open Source Robotics Foundation, Inc.
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
import time
import unittest

from launch import LaunchDescription
from launch_ros.actions import Node
import launch_testing
import pytest
import rclpy
import sensor_msgs.msg


@pytest.mark.rostest
def generate_test_description():
    test_urdfs_dir = os.path.dirname(__file__)
    return LaunchDescription([
        Node(
            package='joint_state_publisher',
            executable='joint_state_publisher',
            arguments=[os.path.join(test_urdfs_dir, 'mimic_chain.urdf')]),
        launch_testing.actions.ReadyToTest(),
    ])


class TestMimicChain(unittest.TestCase):
    TIMEOUT = 10

    @classmethod
    def setUpClass(cls):
        rclpy.init()

    @classmethod
    def tearDownClass(cls):
        rclpy.shutdown()

    def setUp(self):
        # Create a ROS node for tests
        self.node = rclpy.create_node('test_mimic_chain')

    def tearDown(self):
        self.node.destroy_node()

    def test_joints_published(self):
        msgs_rx = []
        sub = self.node.create_subscription(
            sensor_msgs.msg.JointState, 'joint_states', lambda msg: msgs_rx.append(msg), 1)
        try:
            end_time = time.monotonic() + self.TIMEOUT
            while time.monotonic() < end_time and not msgs_rx:
                rclpy.spin_once(self.node, timeout_sec=0.1)

            assert 3 == len(msgs_rx[0].name)
            assert {'j12', 'j23', 'j34'} == set(msgs_rx[0].name)
            assert 3 == len(msgs_rx[0].position)
            assert 0 == len(msgs_rx[0].velocity)
            assert 0 == len(msgs_rx[0].effort)

            expected_values = {'j12': 0.0, 'j23': 0.1, 'j34': -0.1}
            assert expected_values == dict(zip(msgs_rx[0].name, msgs_rx[0].position))
        finally:
            self.node.destroy_subscription(sub)
