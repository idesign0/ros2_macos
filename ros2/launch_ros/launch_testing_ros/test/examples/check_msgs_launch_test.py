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

from threading import Event
from threading import Thread
import unittest

import launch
import launch.actions
import launch_ros.actions
import launch_testing.actions
import launch_testing.markers
import pytest
import rclpy
from std_msgs.msg import String


@pytest.mark.launch_test
@launch_testing.markers.keep_alive
def generate_test_description():
    path_to_test = os.path.dirname(__file__)

    return launch.LaunchDescription([
        launch_ros.actions.Node(
            executable=sys.executable,
            arguments=[os.path.join(path_to_test, 'talker.py')],
            additional_env={'PYTHONUNBUFFERED': '1'},
            name='demo_node_1',
        ),

        launch_testing.actions.ReadyToTest()
    ])


class TestFixture(unittest.TestCase):

    def subscription_callback(self, data: String):
        self.msg_event_object.set()

    def spin(self):
        try:
            while rclpy.ok() and not self.spinning.is_set():
                rclpy.spin_once(self.node, timeout_sec=0.1)
        finally:
            return

    def setUp(self):
        rclpy.init()
        self.node = rclpy.create_node('test_node')
        self.spinning = Event()
        self.msg_event_object = Event()
        self.subscription = self.node.create_subscription(
            String,
            'chatter',
            self.subscription_callback,
            10
        )

        # Add a spin thread
        self.ros_spin_thread = Thread(target=self.spin)
        self.ros_spin_thread.start()

    def tearDown(self):
        self.spinning.set()
        self.ros_spin_thread.join()
        self.node.destroy_subscription(self.subscription)
        self.node.destroy_node()
        rclpy.shutdown()

    def test_check_if_msgs_published(self, proc_output):
        msgs_received_flag = self.msg_event_object.wait(timeout=15.0)
        assert msgs_received_flag, 'Did not receive msgs !'
