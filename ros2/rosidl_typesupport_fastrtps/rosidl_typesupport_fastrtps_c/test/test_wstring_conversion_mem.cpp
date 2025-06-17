// Copyright 2020 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string>

#include "gtest/gtest.h"

#include "rosidl_runtime_c/u16string_functions.h"
#include "osrf_testing_tools_cpp/memory_tools/memory_tools.hpp"
#include "osrf_testing_tools_cpp/scope_exit.hpp"

#include "rosidl_typesupport_fastrtps_c/wstring_conversion.hpp"

using rosidl_typesupport_fastrtps_c::wstring_to_u16string;

TEST(test_wstring_conversion, u16string_resize_failure)
{
  osrf_testing_tools_cpp::memory_tools::initialize();
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    osrf_testing_tools_cpp::memory_tools::uninitialize();
  });

  std::wstring wstring(L"¡Hola, Mundo!");
  rosidl_runtime_c__U16String u16string;
  ASSERT_TRUE(rosidl_runtime_c__U16String__init(&u16string));

  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rosidl_runtime_c__U16String__fini(&u16string);
  });

  // Force malloc/realloc to fail
  auto on_unexpected_operation =
    []() {
      throw std::exception();
    };
  osrf_testing_tools_cpp::memory_tools::on_unexpected_malloc(on_unexpected_operation);
  osrf_testing_tools_cpp::memory_tools::on_unexpected_realloc(on_unexpected_operation);
  osrf_testing_tools_cpp::memory_tools::enable_monitoring();

  EXPECT_NO_MEMORY_OPERATIONS(
  {
    EXPECT_FALSE(wstring_to_u16string(wstring, u16string));
  });
}
