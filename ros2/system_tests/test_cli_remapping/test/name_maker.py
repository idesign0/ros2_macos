# Copyright 2018 Open Source Robotics Foundation, Inc.
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

import rclpy
import rclpy.node
from test_msgs.msg import Empty as EmptyMsg
from test_msgs.srv import Empty as EmptySrv


class NameMaker(rclpy.node.Node):

    def __init__(self):
        super().__init__('original_node_name', namespace='/original/namespace')

        self._pubs = []
        self._pubs.append(self.create_publisher(EmptyMsg, '~/private/name', 10))
        self._pubs.append(self.create_publisher(EmptyMsg, 'relative/name', 10))
        self._pubs.append(self.create_publisher(EmptyMsg, '/fully/qualified/name', 10))

        self._srvs = []
        self._srvs.append(self.create_service(EmptySrv, '~/private/name', lambda x: None))
        self._srvs.append(self.create_service(EmptySrv, 'relative/name', lambda x: None))
        self._srvs.append(self.create_service(EmptySrv, '/fully/qualified/name', lambda x: None))


if __name__ == '__main__':
    rclpy.init()

    node = NameMaker()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        print('Shutting down name_maker.py')
    else:
        rclpy.shutdown()
    finally:
        node.destroy_node()
