# Copyright 2022 Open Source Robotics Foundation, Inc.
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

"""Test for the Parameter substitutions."""

from launch import LaunchContext

from launch.substitutions.substitution_failure import SubstitutionFailure

from launch_ros.actions import SetParameter
from launch_ros.substitutions import Parameter

import pytest


def test_parameter_substitution():
    context = LaunchContext()
    SetParameter('name', 'value').execute(context)
    assert Parameter('name').perform(context) == 'value'
    with pytest.raises(SubstitutionFailure):
        Parameter('name-invalid').perform(context)
