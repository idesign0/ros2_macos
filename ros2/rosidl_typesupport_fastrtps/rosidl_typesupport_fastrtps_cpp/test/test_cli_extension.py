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

import pathlib

from rosidl_cli.command.generate.api import generate

TEST_DIR = str(pathlib.Path(__file__).parent)


def test_cli_extension_for_smoke(tmp_path):
    assert len(generate(
        package_name='rosidl_typesupport_fastrtps_cpp',
        interface_files=[TEST_DIR + ':msg/Test.msg'],
        typesupports=['fastrtps_cpp'],
        output_path=tmp_path
    )) > 0
