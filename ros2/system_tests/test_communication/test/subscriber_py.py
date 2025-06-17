# Copyright 2016 Open Source Robotics Foundation, Inc.
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
import functools
import importlib
import sys


def listener_cb(msg, received_messages, expected_msgs):
    known_msg = False
    msg_repr = repr(msg)
    for num, exp in expected_msgs:
        if msg_repr == exp:
            print('received message #{} of {}'.format(num + 1, len(expected_msgs)))
            known_msg = True
            already_received = False
            for rmsg in received_messages:
                if rmsg == msg_repr:
                    already_received = True
                    break

            if not already_received:
                received_messages.append(msg_repr)
            break
    if known_msg is False:
        raise RuntimeError('received unexpected message %r' % msg)


def listener(message_name, namespace):
    from test_msgs.message_fixtures import get_test_msg
    import rclpy

    message_pkg = 'test_msgs'
    module = importlib.import_module(message_pkg + '.msg')
    msg_mod = getattr(module, message_name)

    rclpy.init(args=[])

    node = rclpy.create_node('listener', namespace=namespace)

    received_messages = []
    expected_msgs = [(i, repr(msg)) for i, msg in enumerate(get_test_msg(message_name))]

    chatter_callback = functools.partial(
        listener_cb, received_messages=received_messages, expected_msgs=expected_msgs)

    node.create_subscription(
        msg_mod, 'test/message/' + message_name, chatter_callback, 10)

    spin_count = 1
    print('subscriber: beginning loop')
    while (rclpy.ok() and len(received_messages) != len(expected_msgs)):
        rclpy.spin_once(node)
        spin_count += 1
        print('spin_count: ' + str(spin_count))
    node.destroy_node()
    rclpy.shutdown()

    assert len(received_messages) == len(expected_msgs),\
        'Should have received {} {} messages from talker'.format(len(expected_msgs), message_name)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('message_name', help='name of the ROS message')
    parser.add_argument('namespace', help='namespace of the ROS node')
    args = parser.parse_args()
    try:
        listener(message_name=args.message_name, namespace=args.namespace)
    except KeyboardInterrupt:
        print('subscriber stopped cleanly')
    except BaseException:
        print('exception in subscriber:', file=sys.stderr)
        raise
