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


def replier_callback(request, response, srv_fixtures):
    for req, resp in srv_fixtures:
        if request.__repr__() == req.__repr__():
            print('received request #%d of %d' %
                  (srv_fixtures.index([req, resp]) + 1, len(srv_fixtures)))
            return resp


def replier(service_name, number_of_cycles, namespace):
    from test_msgs.service_fixtures import get_test_srv
    import rclpy

    service_pkg = 'test_msgs'
    module = importlib.import_module(service_pkg + '.srv')
    srv_mod = getattr(module, service_name)

    rclpy.init(args=[])

    node = rclpy.create_node('replier', namespace=namespace)

    srv_fixtures = get_test_srv(service_name)

    chatter_callback = functools.partial(
        replier_callback, srv_fixtures=srv_fixtures)

    node.create_service(
        srv_mod, 'test/service/' + service_name, chatter_callback)

    spin_count = 1
    print('replier: beginning loop')
    while rclpy.ok() and spin_count < number_of_cycles:
        rclpy.spin_once(node, timeout_sec=2)
        spin_count += 1
        print('spin_count: ' + str(spin_count))
    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('service_name', help='name of the ROS message')
    parser.add_argument('namespace', help='namespace of the ROS node')
    parser.add_argument('-n', '--number_of_cycles', type=int, default=20,
                        help='number of sending attempts')
    args = parser.parse_args()
    try:
        replier(
            service_name=args.service_name,
            number_of_cycles=args.number_of_cycles,
            namespace=args.namespace
        )
    except KeyboardInterrupt:
        print('server stopped cleanly')
    except BaseException:
        print('exception in server:', file=sys.stderr)
        raise
