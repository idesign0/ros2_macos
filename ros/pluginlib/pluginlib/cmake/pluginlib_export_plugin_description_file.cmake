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

if(PLUGINLIB_EXPORT_PLUGIN_DESCRIPTION_FILE_CMAKE_GUARD)
  return()
endif()
set(PLUGINLIB_EXPORT_PLUGIN_DESCRIPTION_FILE_CMAKE_GUARD TRUE)

set(__PLUGINLIB_PLUGIN_CATEGORIES "")

#
# Export a plugin description file provided by the current package for a given plugin category.
#
# In this macro "Export" implies both installing the file to
# the current package's share folder as well as installing the
# required ament index resources needed for the plugin description to be
# discovered later using the plugin category as a filter.
#
# The plugin category is commonly the name of the package which uses the
# plugins or some name based on that and the type of package it is, e.g. `rviz`
# or `rviz_plugins` or `rviz_display_plugins`.
#
# The relative_filename should be relative to the CMake file that calls this
# macro.
# It's relative path to this file will be preserved when being installed.
# For example, if that argument is "plugins/my_desc.xml", then it will be
# installed to "<prefix>/share/<package>/plugins/my_desc.xml" or if the
# argument is just "my_desc.xml" it would be installed to
# "<prefix>/share/<package>/my_desc.xml".
#
# This macro can be called multiple times, even multiple times per
# "plugin_category", but "ament_package" must be called to finish the process.
#
# Where the file is installed to and the use of the ament index to locate the
# file at runtime should be considered an implementation detail, and other
# pluginlib functions should be used in both CMake and C++/Python/etc. code to
# access these files and information.
#
# If migrating from previous pluginlib versions, you might start with a snippet
# in the package.xml's export section like this:
#
#   <rviz plugin="${prefix}/plugin_description.xml"/>
#                ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ the description file location
#         ^^^^^^ referred to as the "attribute" name, is usually "plugin"
#    ^^^^ the package name that uses/defines the plugins described in plugin_description
#
# For this new way of registering plugins, the attribute is no longer
# configurable, and therefore has not corresponding place in this new API.
# The package name is now the "plugin_category", and can stay the same or
# it can become more specific, but it is determined by the packages that will
# load the plugins at runtime.
# The plugin description file location should no longer include the "${prefix}"
# placeholder, and instead should only include a relative path to the file in
# the current package's sources.
#
# Based on the example above this function would be used like this:
#
#   pluginlib_export_plugin_description_file(rviz "plugin_description.xml")
#
# This macro assumes the package name is PROJECT_NAME.
#
# :param plugin_category: category for the type of plugin described; used at
#   runtime to locate all description files for that kind of plugin
# :type plugin_category: string
# :param relative_filename: relative path to the plugin description file
# :type relative_filename: string
#
# @public
#
macro(pluginlib_export_plugin_description_file plugin_category relative_filename)
  set(abs_filename "${CMAKE_CURRENT_SOURCE_DIR}/${relative_filename}")
  if(NOT EXISTS "${abs_filename}")
    message(FATAL_ERROR "Given plugin description file '${abs_filename}' does not exist")
  endif()

  set(relative_dir "")
  get_filename_component(relative_dir "${relative_filename}" DIRECTORY)
  install(FILES ${relative_filename} DESTINATION share/${PROJECT_NAME}/${relative_dir})

  # this accumulated value is written to the ament index resource file in the
  # ament_package() call via the pluginlib hook
  set(__PLUGINLIB_CATEGORY_CONTENT__${plugin_category}
    "${__PLUGINLIB_CATEGORY_CONTENT__${plugin_category}}share/${PROJECT_NAME}/${relative_filename}\n")
  list(APPEND __PLUGINLIB_PLUGIN_CATEGORIES ${plugin_category})  # duplicates are removes on use
endmacro()
