# Copyright 2020 Open Source Robotics Foundation, Inc.
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

# Set variable for accessing template inside function scope
set(PLUGINLIB_ENABLE_PLUGIN_TESTING_DIR "${CMAKE_CURRENT_LIST_DIR}")

#
# Enable testing plugins by mock-installing needed files to the build folder.
#
# Pluginlib needs a certain folder structure in the ament_index to recognize
# the existance of a package and its exported plugins.
# This structure must exist at test time in order for unit tests to load
# plugins.
# The macro `pluginlib_export_plugin_description_file()` sets up this structure
# in the install space, but a package's install space is not accessible to it
# when its own unit tests are run.
# This function sets up the structure in the build space so that unit tests
# can load plugins from the same package they reside in.
#
# The input to this function is everything required by pluginlib to recognize a
# package and its plugins.
# The output of this function is two CMake variables.
# One CMake variable holds a path to be appended to the environment variable
# `AMENT_PREFIX_PATH` used by the test.
# Another CMake variable holds the name of a CMake target that must be
# depended on to ensure the mock environment has been created before the test
# runs.
#
# CMake macros provided by ament_cmake for creating tests have an argument
# called APPEND_ENV that should be used for modifying `AMENT_PREFIX_PATH`.
# `add_dependencies()` must be used to ensure the test runs after the mock
# install environment has been created.
#
#   pluginlib_enable_plugin_testing(...
#     CMAKE_TARGET_VAR mock_install_target
#     AMENT_PREFIX_PATH_VAR mock_install_path
#     ...)
#   ament_add_[some kind of test](some_test_target ...
#     APPEND_ENV AMENT_PREFIX_PATH="${mock_install_path}"
#     ...)
#   add_dependencies(some_test_target "${mock_install_target}")
#
# This function must only be called once for each unique combination of
# "PACKAGE_NAME" and "PLUGIN_CATEGORY".
#
# :param CMAKE_TARGET_VAR: the name of the target that creates the mock install
#   environment.
# :type CMAKE_TARGET_VAR: string
# :param AMENT_PREFIX_PATH_VAR: the name of the variable that will contain the
#   mock installation path for tests to use after this function is run.
# :type AMENT_PREFIX_PATH_VAR: string
# :param PACKAGE_NAME: the name of the mock package to install.
#   If unspecified this defaults to "${PROJECT_NAME}"
# :type PACKAGE_NAME: string
# :param PACKAGE_XML: the path to a package.xml to install.
#   If unspecified this defaults to "${CMAKE_SOURCE_DIR}/package.xml".
# :type PACKAGE_XML: string
# :param PLUGIN_CATEGORY: the plugin category these plugins belong to.
# :type PLUGIN_CATEGORY: string
# :param PLUGIN_DESCRIPTIONS: At least one plugin description to mock install.
#   The paths should match the specification for relative_filename in the macro
#   `pluginlib_export_plugin_description_file()`.
# :type PLUGIN_DESCRIPTIONS: list of strings
# :param PLUGIN_LIBRARIES: At least one library target for pluginlib plugins.
#   There should be one target for every plugin library described in the plugin
#   description files.
# :type PLUGIN_LIBRARIES: list of targets
#
# @public
#
function(pluginlib_enable_plugin_testing)
  #####
  # Make sure inputs are valid
  #####

  cmake_parse_arguments(ARG
    ""
    "CMAKE_TARGET_VAR;AMENT_PREFIX_PATH_VAR;PACKAGE_XML;PACKAGE_NAME;PLUGIN_CATEGORY"
    "PLUGIN_DESCRIPTIONS;PLUGIN_LIBRARIES"
    ${ARGN})
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "pluginlib_enable_plugin_testing() called with unused arguments: "
      "${ARG_UNPARSED_ARGUMENTS}")
  endif()

  # Error or set defaults for required vs optional arguments
  if(NOT ARG_CMAKE_TARGET_VAR)
    message(FATAL_ERROR "CMAKE_TARGET_VAR is a required argument")
  endif()
  if(NOT ARG_AMENT_PREFIX_PATH_VAR)
    message(FATAL_ERROR "AMENT_PREFIX_PATH_VAR is a required argument")
  endif()
  if(NOT ARG_PACKAGE_NAME)
    set(ARG_PACKAGE_NAME "${PROJECT_NAME}")
  endif()
  if(NOT ARG_PACKAGE_XML)
    set(ARG_PACKAGE_XML "${CMAKE_SOURCE_DIR}/package.xml")
  endif()
  if(NOT ARG_PLUGIN_CATEGORY)
    message(FATAL_ERROR "PLUGIN_CATEGORY is a required argument")
  endif()
  if(NOT ARG_PLUGIN_DESCRIPTIONS)
    message(FATAL_ERROR "At least one plugin description via PLUGIN_DESCRIPTIONS is required")
  endif()
  if(NOT ARG_PLUGIN_LIBRARIES)
    message(FATAL_ERROR "At least one plugin library target via PLUGIN_LIBRARIES is required")
  endif()

  set(target_name "pluginlib_enable_plugin_testing__${ARG_PLUGIN_CATEGORY}__${ARG_PACKAGE_NAME}")
  if(TARGET "${target_name}")
    message(FATAL_ERROR "pluginlib_enable_plugin_testing has already been called with category '${ARG_PLUGIN_CATEGORY}' and package name '${ARG_PACKAGE_NAME}'")
  endif()

  #####
  # Plan out mock install space before configuring things to create it
  #####

  # Lists of equal size
  # If input_files is not "", then copy that to the path in "output_files"
  # If input_files is "", then write string from input_content to the path in "output_files"
  # FOOBAR is a workaround to enable appending empty elements when the list is empty
  set(input_content "FOOBAR")
  set(input_files "FOOBAR")
  set(output_files "FOOBAR")

  # Each "PACKAGE_NAME" goes to it's own folder
  set(prefix "${CMAKE_CURRENT_BINARY_DIR}/pluginlib_enable_plugin_testing/install/${ARG_PACKAGE_NAME}__${ARG_PLUGIN_CATEGORY}")

  # Install package.xml, renaming to just `package.xml` if needed
  set(fake_install_dir "${prefix}/share/${ARG_PACKAGE_NAME}/")
  list(APPEND input_files "${ARG_PACKAGE_XML}")
  list(APPEND input_content "")
  list(APPEND output_files "${fake_install_dir}/package.xml")

  # Add blank file indicating existance of package in the ament index
  list(APPEND input_files "")
  list(APPEND input_content "")
  list(APPEND output_files "${prefix}/share/ament_index/resource_index/packages/${ARG_PACKAGE_NAME}")

  # Write the plugin descriptions in share/
  set(ament_index_plugin_content "")
  foreach(plugin_description_path "${ARG_PLUGIN_DESCRIPTIONS}")
    list(APPEND input_files "${plugin_description_path}")
    list(APPEND input_content "")
    list(APPEND output_files "${prefix}/share/${ARG_PACKAGE_NAME}/${plugin_description_path}")

    set(ament_index_plugin_content "${ament_index_plugin_content}\nshare/${ARG_PACKAGE_NAME}/${plugin_description_path}")
  endforeach()
  # Get rid of leading newline
  string(STRIP "${ament_index_plugin_content}" ament_index_plugin_content)

  # Write one file that says where all of this package's plugin description files are
  list(APPEND input_files "")
  list(APPEND input_content "${ament_index_plugin_content}")
  list(APPEND output_files "${prefix}/share/ament_index/resource_index/${ARG_PLUGIN_CATEGORY}__pluginlib__plugin/${ARG_PACKAGE_NAME}")

  # Get rid of FOOBAR
  list(REMOVE_AT input_files 0)
  list(REMOVE_AT input_content 0)
  list(REMOVE_AT output_files 0)

  #####
  # Create commands to generate mock install space at build time
  #####

  list(LENGTH output_files num_output_files)
  # subtract 1 to avoid looping out of range
  math(EXPR range_stop "${num_output_files} - 1")
  foreach(idx RANGE ${range_stop})
    list(GET input_files ${idx} input_file)
    list(GET input_content ${idx} content)
    list(GET output_files ${idx} output_file)

    # Make the directory at configure time because configure_file() does not make directories
    get_filename_component(output_dir "${output_file}" DIRECTORY)
    file(MAKE_DIRECTORY "${output_dir}")

    if(IS_ABSOLUTE "${input_file}")
      set(input_file_abs "${input_file}")
    else()
      set(input_file_abs "${CMAKE_CURRENT_SOURCE_DIR}/${input_file}")
    endif()

    # Copy or write file contents at build time
    if("${input_file}" STREQUAL "")
      set(FILE_CONTENT "${content}")
      configure_file("${PLUGINLIB_ENABLE_PLUGIN_TESTING_DIR}/blank.in" "${output_file}" @ONLY)
    else()
      configure_file("${input_file_abs}" "${output_file}" COPYONLY)
    endif()
  endforeach()

  # Copy library targets at build time; but make directory at configure time
  # because `cmake -E copy` does not make directories
  file(MAKE_DIRECTORY "${prefix}/lib")
  foreach(plugin_library "${ARG_PLUGIN_LIBRARIES}")
    add_custom_command(TARGET "${plugin_library}"
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${plugin_library}>
      "${prefix}/lib/"
    )
  endforeach()

  add_custom_target("${target_name}" DEPENDS ${output_files})

  #####
  # Set output variables
  #####
  set("${ARG_CMAKE_TARGET_VAR}" "${target_name}" PARENT_SCOPE)
  set("${ARG_AMENT_PREFIX_PATH_VAR}" "${prefix}" PARENT_SCOPE)
endfunction()
