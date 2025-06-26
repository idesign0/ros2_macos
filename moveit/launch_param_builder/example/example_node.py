#!/usr/bin/env python3

# Copyright 2021 PickNik Inc.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#
#    * Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#
#    * Neither the name of the PickNik Inc. nor the names of its
#      contributors may be used to endorse or promote products derived from
#      this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.


import rclpy

from rclpy.node import Node
from rclpy.parameter import ParameterType
from rcl_interfaces.msg import ParameterDescriptor


class ParameterBuilderExample(Node):
    def __init__(self):
        super().__init__("launch_param_builder_example")

        self.declare_parameter(
            "my_parameter",
            descriptor=ParameterDescriptor(type=ParameterType.PARAMETER_DOUBLE),
        )
        self.declare_parameter(
            "parameter_file",
            descriptor=ParameterDescriptor(type=ParameterType.PARAMETER_STRING),
        )
        self.declare_parameter(
            "ros2_versions",
            descriptor=ParameterDescriptor(type=ParameterType.PARAMETER_STRING_ARRAY),
        )
        self.declare_parameter(
            "the_answer_to_life",
            descriptor=ParameterDescriptor(type=ParameterType.PARAMETER_INTEGER),
        )
        self.declare_parameter(
            "package_name",
            descriptor=ParameterDescriptor(type=ParameterType.PARAMETER_STRING),
        )
        self.declare_parameter(
            "my_robot",
            descriptor=ParameterDescriptor(type=ParameterType.PARAMETER_STRING),
        )

        self.get_logger().info(
            f"my_parameter: {self.get_parameter('my_parameter').value}"
        )
        self.get_logger().info(
            f"parameter_file: {self.get_parameter('parameter_file').value}"
        )
        self.get_logger().info(f"my_robot: {self.get_parameter('my_robot').value}")
        self.get_logger().info(
            f"ros2_versions: {self.get_parameter('ros2_versions').value}"
        )
        self.get_logger().info(
            f"the_answer_to_life: {self.get_parameter('the_answer_to_life').value}"
        )
        self.get_logger().info(
            f"package_name: {self.get_parameter('package_name').value}"
        )


def main(args=None):
    rclpy.init(args=args)
    node = ParameterBuilderExample()
    node.destroy_node()
    rclpy.shutdown()


if __name__ == "__main__":
    main()
