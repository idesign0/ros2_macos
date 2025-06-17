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

#include "osrf_testing_tools_cpp/scope_exit.hpp"

#include "rosidl_typesupport_fastrtps_cpp/wstring_conversion.hpp"

using rosidl_typesupport_fastrtps_cpp::u16string_to_wstring;
using rosidl_typesupport_fastrtps_cpp::wstring_to_u16string;

TEST(test_wstring_conversion, wstring_to_u16string)
{
  std::u16string actual;

  // Default string
  EXPECT_TRUE(wstring_to_u16string(std::wstring(), actual));
  EXPECT_EQ(std::u16string(), actual);

  // Empty string
  EXPECT_TRUE(wstring_to_u16string(std::wstring(L""), actual));
  EXPECT_EQ(std::u16string(u""), actual);

  // Non-empty string
  EXPECT_TRUE(wstring_to_u16string(std::wstring(L"¡Hola, Mundo!"), actual));
  EXPECT_EQ(std::u16string(u"¡Hola, Mundo!"), actual);
}

TEST(test_wstring_conversion, u16string_to_wstring)
{
  std::wstring actual;

  // Default string
  u16string_to_wstring(std::u16string(), actual);
  EXPECT_EQ(std::wstring(), actual);

  // Empty string
  u16string_to_wstring(std::u16string(u""), actual);
  EXPECT_EQ(std::wstring(L""), actual);

  // Non-empty string
  u16string_to_wstring(std::u16string(u"¡Hola, Mundo!"), actual);
  EXPECT_EQ(std::wstring(L"¡Hola, Mundo!"), actual);
}
