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
import importlib
import sys
import time


def talker(message_name, number_of_cycles, namespace):
    from test_msgs.message_fixtures import get_test_msg
    import rclpy

    message_pkg = 'test_msgs'
    module = importlib.import_module(message_pkg + '.msg')
    msg_mod = getattr(module, message_name)

    rclpy.init(args=[])

    node = rclpy.create_node('talker', namespace=namespace)

    chatter_pub = node.create_publisher(
        msg_mod, 'test/message/' + message_name, 10)

    cycle_count = 0
    print('talker: beginning loop')
    msgs = get_test_msg(message_name)
    while rclpy.ok() and cycle_count < number_of_cycles:
        msg_count = 0
        for msg in msgs:
            chatter_pub.publish(msg)
            msg_count += 1
            print('publishing message #%d' % msg_count)
            time.sleep(0.01)
        cycle_count += 1
        time.sleep(0.1)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('message_name', help='name of the ROS message')
    parser.add_argument('namespace', help='namespace of the ROS node')
    parser.add_argument('-n', '--number_of_cycles', type=int, default=100,
                        help='number of sending attempts')
    args = parser.parse_args()
    try:
        talker(
            message_name=args.message_name,
            namespace=args.namespace,
            number_of_cycles=args.number_of_cycles)
    except KeyboardInterrupt:
        print('talker stopped cleanly')
    except BaseException:
        print('exception in talker:', file=sys.stderr)
        raise
