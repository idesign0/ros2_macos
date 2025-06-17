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

import argparse
import sys
import time

from rclpy.executors import ExternalShutdownException

from test_msgs.action import Fibonacci
from test_msgs.action import NestedMessage


class ExpectedGoal:

    def is_goal_expected(self, goal):
        raise NotImplementedError('is_goal_expected() is not implemented')

    def execute_goal(self, goal_handle):
        raise NotImplementedError('execute_goal() is not implemented')


def receive_goals(node, action_type, expected_goals):
    from rclpy.action import ActionServer

    def execute_callback(goal_handle):
        for expected_goal in expected_goals:
            if expected_goal.is_goal_expected(goal_handle.request):
                return expected_goal.execute_goal(goal_handle)
        # Not an expected goal (this should not happen)
        print('Unexpected goal received by action server', file=sys.stderr)
        goal_handle.abort()
        return action_type.Result()

    action_name = 'test/action/' + action_type.__name__
    return ActionServer(node, action_type, action_name, execute_callback)


def generate_expected_fibonacci_goals():
    import rclpy

    expected_goals = []

    def is_goal_expected(goal):
        return (isinstance(goal, Fibonacci.Goal) and 10 == goal.order)

    def execute_goal(goal_handle):
        goal = goal_handle.request

        feedback = Fibonacci.Feedback()
        feedback.sequence = [0, 1]

        for i in range(1, goal.order):
            if not rclpy.ok():
                goal_handle.abort()
                return Fibonacci.Result()

            # Check if the goal was canceled
            if goal_handle.is_cancel_requested:
                goal_handle.canceled()
                result = Fibonacci.Result()
                result.sequence = feedback.sequence
                print('Goal was canceled')
                return result

            # Update the sequence.
            feedback.sequence.append(feedback.sequence[i] + feedback.sequence[i-1])

            # Publish feedback
            goal_handle.publish_feedback(feedback)
            print('Publishing feedback')

            # 10 Hz update rate
            time.sleep(0.1)

        # Send final result
        result = Fibonacci.Result()
        result.sequence = feedback.sequence
        goal_handle.succeed()
        print('Goal succeeded')
        return result

    expected_goal = ExpectedGoal()
    expected_goal.is_goal_expected = is_goal_expected
    expected_goal.execute_goal = execute_goal

    expected_goals.append(expected_goal)

    return expected_goals


def generate_expected_nested_message_goals():
    import rclpy

    expected_goals = []

    def is_goal_expected(goal):
        return (isinstance(goal, NestedMessage.Goal) and
                goal.nested_field_no_pkg.duration_value.sec > 0)

    def execute_goal(goal_handle):
        goal = goal_handle.request

        feedback = NestedMessage.Feedback()
        feedback.nested_different_pkg.sec = 2 * goal.nested_field_no_pkg.duration_value.sec
        result = NestedMessage.Result()
        result.nested_field.int32_value = 4 * goal.nested_field_no_pkg.duration_value.sec

        num_feedback = 10
        for i in range(0, num_feedback):
            if not rclpy.ok():
                goal_handle.abort()
                return NestedMessage.Result()

            # Check if the goal was canceled
            if goal_handle.is_cancel_requested:
                goal_handle.canceled()
                print('Goal was canceled')
                return result

            # Publish feedback
            goal_handle.publish_feedback(feedback)
            print('Publishing feedback')

            # 10 Hz update rate
            time.sleep(0.1)

        # Send final result
        goal_handle.succeed()
        print('Goal succeeded')
        return result

    expected_goal = ExpectedGoal()
    expected_goal.is_goal_expected = is_goal_expected
    expected_goal.execute_goal = execute_goal

    expected_goals.append(expected_goal)

    return expected_goals


if __name__ == '__main__':
    import rclpy

    parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('action_type', help='type of the ROS action')
    parser.add_argument('namespace', help='namespace of the ROS node')
    args = parser.parse_args()

    rclpy.init(args=[])
    node = rclpy.create_node('action_server', namespace=args.namespace)

    if 'Fibonacci' == args.action_type:
        action_server = receive_goals(node, Fibonacci, generate_expected_fibonacci_goals())
    elif 'NestedMessage' == args.action_type:
        action_server = receive_goals(
            node,
            NestedMessage,
            generate_expected_nested_message_goals(),
        )
    else:
        print('Unknown action type {!r}'.format(args.action_type), file=sys.stderr)
        node.destroy_node()
        rclpy.shutdown()
        sys.exit(1)

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        print('Action server stopped cleanly')
    except ExternalShutdownException:
        sys.exit(1)
    except BaseException:
        print('Exception in action server:', file=sys.stderr)
        raise
    finally:
        rclpy.try_shutdown()
        node.destroy_node()
