# Copyright 2023 Open Source Robotics Foundation, Inc.
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

cmake_minimum_required(VERSION 3.7)

set(node_names_list "/publisher;/subscriber;/publisher_missing_key;/publisher_invalid_cert")
file(REMOVE_RECURSE "${KEYSTORE_DIRECTORY}/enclaves")
set(generate_artifacts_command ${ROS2_EXECUTABLE} security generate_artifacts -k ${KEYSTORE_DIRECTORY_NATIVE_PATH} -e ${node_names_list})
execute_process(
  COMMAND ${generate_artifacts_command}
  RESULT_VARIABLE GENERATE_ARTIFACTS_RESULT
  ERROR_VARIABLE GENERATE_ARTIFACTS_ERROR
)
if(NOT ${GENERATE_ARTIFACTS_RESULT} EQUAL 0)
  message(FATAL_ERROR "Failed to generate security artifacts: ${GENERATE_ARTIFACTS_ERROR}")
endif()

# deleting key of /publisher_missing_key
file(REMOVE "${KEYSTORE_DIRECTORY}/enclaves/publisher_missing_key/key.pem")

# copy invalid certificate from source tree
file(COPY ${CMAKE_CURRENT_LIST_DIR}/test_security_files/publisher_invalid_cert/cert.pem
  DESTINATION ${KEYSTORE_DIRECTORY}/enclaves/publisher_invalid_cert/
)
