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

import io
import textwrap

from launch import LaunchService
from launch.frontend import Parser
from launch.utilities import perform_substitutions


def test_parameter_substitution_yaml():
    yaml_file = textwrap.dedent(
        r"""
        launch:
            - set_parameter:
                name: name
                value: value

            - let:
                name: result
                value: $(param name)
        """
    )
    with io.StringIO(yaml_file) as f:
        check_parameter_substitution(f)


def test_parameter_substitution_xml():
    xml_file = textwrap.dedent(
        r"""
        <launch>
            <set_parameter name="name" value="value" />
            <let name="result" value="$(param name)" />
        </launch>
        """
    )
    with io.StringIO(xml_file) as f:
        check_parameter_substitution(f)


def check_parameter_substitution(file):
    root_entity, parser = Parser.load(file)
    ld = parser.parse_description(root_entity)
    ls = LaunchService()
    ls.include_launch_description(ld)
    assert 0 == ls.run()

    def perform(substitution):
        return perform_substitutions(ls.context, substitution)

    set_parameter, let = ld.describe_sub_entities()
    assert perform(set_parameter.name) == 'name'
    assert perform(set_parameter.value) == 'value'
    assert perform(let.name) == 'result'
    assert perform(let.value) == 'value'
