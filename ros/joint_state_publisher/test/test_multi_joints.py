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

import itertools
import os
import time
import unittest

from launch import LaunchDescription
from launch.actions import ExecuteProcess
from launch_ros.actions import Node
import launch_testing
import pytest
import rclpy
import sensor_msgs.msg
import yaml


@pytest.mark.rostest
def generate_test_description():
    test_dir = os.path.dirname(__file__)

    dae_path = os.path.join(test_dir, 'multi_joint_robot.dae')
    sdf_path = os.path.join(test_dir, 'multi_joint_robot.sdf')
    urdf_path = os.path.join(test_dir, 'multi_joint_robot.urdf')

    with open(dae_path, 'r') as dae_file:
        dae_text = dae_file.read()

    with open(sdf_path, 'r') as sdf_file:
        sdf_text = sdf_file.read()

    with open(urdf_path, 'r') as urdf_file:
        urdf_text = urdf_file.read()

    dae_yaml = yaml.dump({'data': dae_text})
    sdf_yaml = yaml.dump({'data': sdf_text})
    urdf_yaml = yaml.dump({'data': urdf_text})

    ros2_topic_cmd = ['ros2', 'topic', 'pub', '--qos-durability', 'transient_local']

    ros2_topic_dae = ros2_topic_cmd + ['robot_description/dae', 'std_msgs/msg/String', dae_yaml]
    ros2_topic_sdf = ros2_topic_cmd + ['robot_description/sdf', 'std_msgs/msg/String', sdf_yaml]
    ros2_topic_urdf = ros2_topic_cmd + ['robot_description/urdf', 'std_msgs/msg/String', urdf_yaml]

    return LaunchDescription([
        Node(
            package='joint_state_publisher',
            executable='joint_state_publisher',
            name='joint_state_publisher_collada',
            arguments=[dae_path],
            remappings=[('joint_states', 'joint_states/collada/from_cli')],
            output=['screen']),
        Node(
            package='joint_state_publisher',
            executable='joint_state_publisher',
            name='joint_state_publisher_sdf',
            arguments=[sdf_path],
            remappings=[('joint_states', 'joint_states/sdf/from_cli')],
            output=['screen']),
        Node(
            package='joint_state_publisher',
            executable='joint_state_publisher',
            name='joint_state_publisher_urdf',
            arguments=[urdf_path],
            remappings=[('joint_states', 'joint_states/urdf/from_cli')],
            output=['screen']),
        ExecuteProcess(cmd=ros2_topic_dae),
        ExecuteProcess(cmd=ros2_topic_sdf),
        ExecuteProcess(cmd=ros2_topic_urdf),
        Node(
            package='joint_state_publisher',
            executable='joint_state_publisher',
            name='joint_state_publisher_collada_from_topic',
            remappings=[
                ('joint_states', 'joint_states/collada/from_topic'),
                ('robot_description', 'robot_description/dae'),
            ],
            output=['screen']),
        Node(
            package='joint_state_publisher',
            executable='joint_state_publisher',
            name='joint_state_publisher_sdf_from_topic',
            remappings=[
                ('joint_states', 'joint_states/sdf/from_topic'),
                ('robot_description', 'robot_description/sdf'),
            ],
            output=['screen']),
        Node(
            package='joint_state_publisher',
            executable='joint_state_publisher',
            name='joint_state_publisher_urdf_from_topic',
            remappings=[
                ('joint_states', 'joint_states/urdf/from_topic'),
                ('robot_description', 'robot_description/urdf'),
            ],
            output=['screen']),
        launch_testing.actions.ReadyToTest(),
    ])


class TestMultiJoint(unittest.TestCase):
    TIMEOUT = 10

    @classmethod
    def setUpClass(cls):
        rclpy.init()

    @classmethod
    def tearDownClass(cls):
        rclpy.shutdown()

    def setUp(self):
        # Create a ROS node for tests
        self.node = rclpy.create_node('test_multi_joint')

    def tearDown(self):
        self.node.destroy_node()

    def test_joints_published(self):
        msgs_rx_collada_from_cli = []
        sub_collada = self.node.create_subscription(
            sensor_msgs.msg.JointState, 'joint_states/collada/from_cli',
            lambda msg: msgs_rx_collada_from_cli.append(msg), 1)
        msgs_rx_sdf_from_cli = []
        sub_sdf = self.node.create_subscription(
            sensor_msgs.msg.JointState, 'joint_states/sdf/from_cli',
            lambda msg: msgs_rx_sdf_from_cli.append(msg), 1)
        msgs_rx_urdf_from_cli = []
        sub_urdf = self.node.create_subscription(
            sensor_msgs.msg.JointState, 'joint_states/urdf/from_cli',
            lambda msg: msgs_rx_urdf_from_cli.append(msg), 1)
        msgs_rx_collada_from_topic = []
        sub_collada = self.node.create_subscription(
            sensor_msgs.msg.JointState, 'joint_states/collada/from_topic',
            lambda msg: msgs_rx_collada_from_topic.append(msg), 1)
        msgs_rx_sdf_from_topic = []
        sub_sdf = self.node.create_subscription(
            sensor_msgs.msg.JointState, 'joint_states/sdf/from_topic',
            lambda msg: msgs_rx_sdf_from_topic.append(msg), 1)
        msgs_rx_urdf_from_topic = []
        sub_urdf = self.node.create_subscription(
            sensor_msgs.msg.JointState, 'joint_states/urdf/from_topic',
            lambda msg: msgs_rx_urdf_from_topic.append(msg), 1)
        try:
            end_time = time.monotonic() + self.TIMEOUT
            msg_lists = (
                msgs_rx_collada_from_cli, msgs_rx_sdf_from_cli, msgs_rx_urdf_from_cli,
                msgs_rx_collada_from_topic, msgs_rx_sdf_from_topic, msgs_rx_urdf_from_topic)
            while time.monotonic() < end_time and not all(msg_lists):
                rclpy.spin_once(self.node, timeout_sec=0.1)

            assert msgs_rx_collada_from_cli
            assert msgs_rx_sdf_from_cli
            assert msgs_rx_urdf_from_cli
            assert msgs_rx_collada_from_topic
            assert msgs_rx_sdf_from_topic
            assert msgs_rx_urdf_from_topic
            for msg in itertools.chain.from_iterable(msg_lists):
                assert 2 == len(msg.name)
                assert 2 == len(msg.position)
                assert 0 == len(msg.velocity)
                assert 0 == len(msg.effort)
                assert {'j12': 0.0, 'j23': 0.0} == dict(zip(msg.name, msg.position))
        finally:
            self.node.destroy_subscription(sub_collada)
            self.node.destroy_subscription(sub_sdf)
            self.node.destroy_subscription(sub_urdf)
