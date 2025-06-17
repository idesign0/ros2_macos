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

# install the ament resource for the plugin descriptions exported by
# this package
list(REMOVE_DUPLICATES __PLUGINLIB_PLUGIN_CATEGORIES)
foreach(plugin_category ${__PLUGINLIB_PLUGIN_CATEGORIES})
  # this assumes PROJECT_NAME is the package name
  # note the trailing `plugin` is the invariant "attribute name" from the old
  # style of registering plugins through pluginlib
  ament_index_register_resource(${plugin_category}__pluginlib__plugin
    CONTENT "${__PLUGINLIB_CATEGORY_CONTENT__${plugin_category}}")
endforeach()
