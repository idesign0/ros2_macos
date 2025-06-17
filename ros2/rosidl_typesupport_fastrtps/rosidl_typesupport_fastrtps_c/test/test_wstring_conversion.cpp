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
#include "osrf_testing_tools_cpp/scope_exit.hpp"

#include "rosidl_typesupport_fastrtps_c/wstring_conversion.hpp"

using rosidl_typesupport_fastrtps_c::u16string_to_wstring;
using rosidl_typesupport_fastrtps_c::wstring_to_u16string;

TEST(test_wstring_conversion, wstring_to_u16string)
{
  rosidl_runtime_c__U16String actual;
  ASSERT_TRUE(rosidl_runtime_c__U16String__init(&actual));
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rosidl_runtime_c__U16String__fini(&actual);
  });

  // Default string
  EXPECT_TRUE(wstring_to_u16string(std::wstring(), actual));
  EXPECT_EQ(0, memcmp(u"", actual.data, actual.size));

  // Empty string
  EXPECT_TRUE(wstring_to_u16string(std::wstring(L""), actual));
  EXPECT_EQ(0, memcmp(u"", actual.data, actual.size));

  // Non-empty string
  EXPECT_TRUE(wstring_to_u16string(std::wstring(L"¡Hola, Mundo!"), actual));
  EXPECT_EQ(0, memcmp(u"¡Hola, Mundo!", actual.data, actual.size));
}

TEST(test_wstring_conversion, u16string_to_wstring)
{
  std::wstring actual;
  rosidl_runtime_c__U16String input;
  ASSERT_TRUE(rosidl_runtime_c__U16String__init(&input));
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rosidl_runtime_c__U16String__fini(&input);
  });

  // Default string
  u16string_to_wstring(input, actual);
  EXPECT_EQ(std::wstring(), actual);

  // Empty string
  ASSERT_TRUE(rosidl_runtime_c__U16String__assign(&input, (const uint16_t *)u""));
  u16string_to_wstring(input, actual);
  EXPECT_EQ(std::wstring(L""), actual);

  // Non-empty string
  ASSERT_TRUE(rosidl_runtime_c__U16String__assign(&input, (const uint16_t *)u"¡Hola, Mundo!"));
  u16string_to_wstring(input, actual);
  EXPECT_EQ(std::wstring(L"¡Hola, Mundo!"), actual);
}
