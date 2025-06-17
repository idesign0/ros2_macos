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

from rclpy.executors import ExternalShutdownException

from test_msgs.action import Fibonacci
from test_msgs.action import NestedMessage


class ActionClientTest:

    def __init__(self, goal):
        self.goal = goal

    def is_feedback_valid(self, feedback):
        raise NotImplementedError('is_feedback_valid() is not implemented')

    def is_result_valid(self, result):
        raise NotImplementedError('is_result_valid() is not implemented')


def send_goals(node, action_type, tests):
    import rclpy
    from rclpy.action import ActionClient

    action_client = ActionClient(node, action_type, 'test/action/' + action_type.__name__)

    if not action_client.wait_for_server(20):
        print('Action server not available after waiting', file=sys.stderr)
        return 1

    test_index = 0
    while rclpy.ok() and test_index < len(tests):
        print('Sending goal for test number {}'.format(test_index), file=sys.stderr)

        test = tests[test_index]

        invalid_feedback = False

        # On feedback, check the feedback is valid
        def feedback_callback(feedback):
            nonlocal invalid_feedback
            if not test.is_feedback_valid(feedback):
                invalid_feedback = True

        goal_handle_future = action_client.send_goal_async(
            test.goal,
            feedback_callback=feedback_callback)

        rclpy.spin_until_future_complete(node, goal_handle_future)

        goal_handle = goal_handle_future.result()
        if not goal_handle.accepted:
            print('Goal rejected', file=sys.stderr)
            return 1

        get_result_future = goal_handle.get_result_async()

        rclpy.spin_until_future_complete(node, get_result_future)

        result = get_result_future.result()

        if not test.is_result_valid(result):
            return 1

        if invalid_feedback:
            return 1

        test_index += 1

    return 0


def generate_fibonacci_goal_tests():
    tests = []

    order = 10
    goal = Fibonacci.Goal()
    goal.order = order

    valid_result_sequence = [0, 1]
    for i in range(1, goal.order):
        valid_result_sequence.append(valid_result_sequence[i] + valid_result_sequence[i-1])

    test = ActionClientTest(goal)

    def is_feedback_valid(feedback_message):
        feedback = feedback_message.feedback
        if len(feedback.sequence) > (order + 1):
            print('Feedback sequence is greater than goal order', file=sys.stderr)
            return False
        for i in range(0, len(feedback.sequence)):
            if feedback.sequence[i] != valid_result_sequence[i]:
                print('Feedback sequence not correct, expected {} but got {} for order {}'.format(
                    valid_result_sequence[i], feedback.sequence[i], order), file=sys.stderr)
                return False
        return True

    def is_result_valid(get_result_response):
        result = get_result_response.result
        if len(result.sequence) != (order + 1):
            print('Result sequence does not equal goal order', file=sys.stderr)
            return False
        for i in range(0, len(result.sequence)):
            if result.sequence[i] != valid_result_sequence[i]:
                print('Result sequence not correct, expected {} but got {} for order {}'.format(
                    valid_result_sequence[i], result.sequence[i], order), file=sys.stderr)
                return False
        return True

    test.is_feedback_valid = is_feedback_valid
    test.is_result_valid = is_result_valid

    tests.append(test)

    return tests


def generate_nested_message_goal_tests():
    tests = []

    initial_value = 123
    expected_feedback_value = 2 * initial_value
    expected_result_value = 4 * initial_value

    goal = NestedMessage.Goal()
    goal.nested_field_no_pkg.duration_value.sec = initial_value

    test = ActionClientTest(goal)

    def is_feedback_valid(feedback_message):
        feedback = feedback_message.feedback
        if feedback.nested_different_pkg.sec != expected_feedback_value:
            print('Expected feedback {} but got {} for initial value {}'.format(
                expected_feedback_value, feedback.nested_different_pkg.sec, initial_value),
                file=sys.stderr)
            return False
        return True

    def is_result_valid(get_result_response):
        result = get_result_response.result
        if result.nested_field.int32_value != expected_result_value:
            print('Expected result {} but got {} for initial value {}'.format(
                expected_result_value, result.nested_field.int32_value, initial_value),
                file=sys.stderr)
            return False
        return True

    test.is_feedback_valid = is_feedback_valid
    test.is_result_valid = is_result_valid

    tests.append(test)

    return tests


if __name__ == '__main__':
    import rclpy

    parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('action_type', help='type of the ROS action')
    parser.add_argument('namespace', help='namespace of the ROS node')
    args = parser.parse_args()

    rclpy.init(args=[])
    node = rclpy.create_node('action_client', namespace=args.namespace)

    rc = 1
    try:
        if 'Fibonacci' == args.action_type:
            rc = send_goals(node, Fibonacci, generate_fibonacci_goal_tests())
        elif 'NestedMessage' == args.action_type:
            rc = send_goals(node, NestedMessage, generate_nested_message_goal_tests())
        else:
            print('Unknown action type {!r}'.format(args.action_type), file=sys.stderr)
    except KeyboardInterrupt:
        print('Action client stopped cleanly')
    except ExternalShutdownException:
        print('Action client stopped with sigterm', file=sys.stderr)
        rc = 1
    except BaseException:
        print('Exception in action client:', file=sys.stderr)
        raise
    finally:
        rclpy.try_shutdown()
        node.destroy_node()
    sys.exit(rc)
