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

import contextlib
import os

import domain_coordinator
import launch_testing.launch_test
import launch_testing_ros

from ros2cli.command import CommandExtension


class TestCommand(CommandExtension):
    """Run a ROS2 launch test."""

    def add_arguments(self, parser, cli_name):
        """Add arguments to argparse."""
        launch_testing.launch_test.add_arguments(parser)
        parser.add_argument(
            '--disable-isolation', action='store_true', default=False,
            help='Disable automatic ROS_DOMAIN_ID isolation.'
            'If ROS_DOMAIN_ID is already set, ros2 test will respect and use it. If it is not'
            ' set, a ROS_DOMAIN_ID not being used by another ros2 test will be chosen '
            'unless isolation is disabled.'
        )

    def main(self, *, parser, args):
        """Entry point for CLI program."""
        with contextlib.ExitStack() as stack:
            if 'ROS_DOMAIN_ID' not in os.environ and not args.disable_isolation:
                domain_id = stack.enter_context(domain_coordinator.domain_id())
                print('Running with ROS_DOMAIN_ID {}'.format(domain_id))
                os.environ['ROS_DOMAIN_ID'] = str(domain_id)
            if 'ROS_DOMAIN_ID' in os.environ:
                print('ROS_DOMAIN_ID', os.environ['ROS_DOMAIN_ID'])
            return launch_testing.launch_test.run(
                parser, args, test_runner_cls=launch_testing_ros.LaunchTestRunner
            )
