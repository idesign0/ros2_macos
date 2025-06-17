# Copyright 2017 Open Source Robotics Foundation, Inc.
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

# copied from pluginlib/pluginlib-extras.cmake

find_package(ament_cmake_core QUIET REQUIRED)
ament_register_extension("ament_package" "pluginlib"
  "pluginlib_package_hook.cmake")

include("${pluginlib_DIR}/pluginlib_export_plugin_description_file.cmake")
include("${pluginlib_DIR}/pluginlib_enable_plugin_testing.cmake")
