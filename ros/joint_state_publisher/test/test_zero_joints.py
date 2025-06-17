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
            arguments=[os.path.join(test_urdfs_dir, 'zero_joint_robot.urdf')]),
        launch_testing.actions.ReadyToTest(),
    ])


class TestZeroJoints(unittest.TestCase):
    TIMEOUT = 10

    @classmethod
    def setUpClass(cls):
        rclpy.init()

    @classmethod
    def tearDownClass(cls):
        rclpy.shutdown()

    def setUp(self):
        # Create a ROS node for tests
        self.node = rclpy.create_node('test_zero_joints')

    def tearDown(self):
        self.node.destroy_node()

    def test_joints_not_published(self):
        msgs_rx = []
        sub = self.node.create_subscription(
            sensor_msgs.msg.JointState, 'joint_states', lambda msg: msgs_rx.append(msg), 1)
        try:
            end_time = time.monotonic() + self.TIMEOUT
            while time.monotonic() < end_time:
                rclpy.spin_once(self.node, timeout_sec=0.1)

            # Nothing should be published
            assert [] == msgs_rx
        finally:
            self.node.destroy_subscription(sub)
