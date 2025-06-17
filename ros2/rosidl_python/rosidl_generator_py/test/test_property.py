# Copyright 2021 Open Source Robotics Foundation, Inc.
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

from rosidl_generator_py.msg import Property


def test_msg_property():
    msg = Property()

    # types
    assert isinstance(msg.property, str)
    assert isinstance(msg.anything, str)

    # default values
    assert '' == msg.property
    assert '' == msg.anything

    # set values
    msg.property = 'a_value'
    msg.anything = 'another_value'

    # get values
    assert 'a_value' == msg.property
    assert 'another_value' == msg.anything
