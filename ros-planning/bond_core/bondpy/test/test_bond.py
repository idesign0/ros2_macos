# Copyright 2024 Open Source Robotics Foundation, Inc.
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

from bondpy.bondpy import Bond

import rclpy
from rclpy.duration import Duration


def test_connection():
    context = rclpy.Context()
    rclpy.init(context=context)
    try:
        node_1 = rclpy.create_node('test_node_1', context=context)
        node_2 = rclpy.create_node('test_node_2', context=context)

        bond_a = Bond(node_1, 'test_topic_bond', 'test_id')
        bond_b = Bond(node_2, 'test_topic_bond', 'test_id')

        bond_a.start()
        bond_b.start()

        assert bond_a.wait_until_formed(timeout=Duration(seconds=30))
        assert bond_b.wait_until_formed(timeout=Duration(seconds=30))

    finally:
        rclpy.shutdown(context=context)


def test_disconnection():
    context = rclpy.Context()
    rclpy.init(context=context)
    try:
        node_1 = rclpy.create_node('test_node_1', context=context)
        node_2 = rclpy.create_node('test_node_2', context=context)

        bond_a = Bond(node_1, 'test_topic_bond', 'test_id')
        bond_b = Bond(node_2, 'test_topic_bond', 'test_id')

        bond_a.start()
        bond_b.start()

        bond_a.wait_until_formed()
        bond_a.break_bond()

        assert bond_a.wait_until_broken(timeout=Duration(seconds=30))
        assert bond_b.wait_until_broken(timeout=Duration(seconds=30))

    finally:
        rclpy.shutdown(context=context)
