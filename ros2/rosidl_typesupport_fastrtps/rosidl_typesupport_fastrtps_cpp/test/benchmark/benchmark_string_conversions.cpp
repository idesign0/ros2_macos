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

#include "rosidl_typesupport_fastrtps_cpp/wstring_conversion.hpp"

#include "performance_test_fixture/performance_test_fixture.hpp"

using performance_test_fixture::PerformanceTest;

namespace
{
constexpr const uint64_t kSize = 1024;
}

BENCHMARK_F(PerformanceTest, wstring_to_u16string)(benchmark::State & st)
{
  std::wstring wstring(kSize, '*');

  reset_heap_counters();

  for (auto _ : st) {
    std::u16string u16string;
    rosidl_typesupport_fastrtps_cpp::wstring_to_u16string(wstring, u16string);
  }
}

BENCHMARK_F(PerformanceTest, u16string_to_wstring)(benchmark::State & st)
{
  std::u16string u16string(kSize, '*');

  reset_heap_counters();

  for (auto _ : st) {
    std::wstring wstring;
    rosidl_typesupport_fastrtps_cpp::u16string_to_wstring(u16string, wstring);
  }
}
