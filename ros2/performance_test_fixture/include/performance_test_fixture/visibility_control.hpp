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

#ifndef PERFORMANCE_TEST_FIXTURE__VISIBILITY_CONTROL_HPP_
#define PERFORMANCE_TEST_FIXTURE__VISIBILITY_CONTROL_HPP_

// This logic was borrowed (then namespaced) from the examples on the gcc wiki:
//     https://gcc.gnu.org/wiki/Visibility

#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define PERFORMANCE_TEST_FIXTURE_EXPORT __attribute__ ((dllexport))
    #define PERFORMANCE_TEST_FIXTURE_IMPORT __attribute__ ((dllimport))
  #else
    #define PERFORMANCE_TEST_FIXTURE_EXPORT __declspec(dllexport)
    #define PERFORMANCE_TEST_FIXTURE_IMPORT __declspec(dllimport)
  #endif
  #ifdef PERFORMANCE_TEST_FIXTURE_BUILDING_DLL
    #define PERFORMANCE_TEST_FIXTURE_PUBLIC PERFORMANCE_TEST_FIXTURE_EXPORT
  #else
    #define PERFORMANCE_TEST_FIXTURE_PUBLIC PERFORMANCE_TEST_FIXTURE_IMPORT
  #endif
  #define PERFORMANCE_TEST_FIXTURE_PUBLIC_TYPE PERFORMANCE_TEST_FIXTURE_PUBLIC
  #define PERFORMANCE_TEST_FIXTURE_LOCAL
#else
  #define PERFORMANCE_TEST_FIXTURE_EXPORT __attribute__ ((visibility("default")))
  #define PERFORMANCE_TEST_FIXTURE_IMPORT
  #if __GNUC__ >= 4
    #define PERFORMANCE_TEST_FIXTURE_PUBLIC __attribute__ ((visibility("default")))
    #define PERFORMANCE_TEST_FIXTURE_LOCAL  __attribute__ ((visibility("hidden")))
  #else
    #define PERFORMANCE_TEST_FIXTURE_PUBLIC
    #define PERFORMANCE_TEST_FIXTURE_LOCAL
  #endif
  #define PERFORMANCE_TEST_FIXTURE_PUBLIC_TYPE
#endif

#endif  // PERFORMANCE_TEST_FIXTURE__VISIBILITY_CONTROL_HPP_
