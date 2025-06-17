// Copyright 2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

#ifndef LIBSTATISTICS_COLLECTOR__VISIBILITY_CONTROL_HPP_
#define LIBSTATISTICS_COLLECTOR__VISIBILITY_CONTROL_HPP_

#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define LIBSTATISTICS_COLLECTOR_EXPORT __attribute__ ((dllexport))
    #define LIBSTATISTICS_COLLECTOR_IMPORT __attribute__ ((dllimport))
  #else
    #define LIBSTATISTICS_COLLECTOR_EXPORT __declspec(dllexport)
    #define LIBSTATISTICS_COLLECTOR_IMPORT __declspec(dllimport)
  #endif
  #ifdef LIBSTATISTICS_COLLECTOR_BUILDING_LIBRARY
    #define LIBSTATISTICS_COLLECTOR_PUBLIC LIBSTATISTICS_COLLECTOR_EXPORT
  #else
    #define LIBSTATISTICS_COLLECTOR_PUBLIC LIBSTATISTICS_COLLECTOR_IMPORT
  #endif
  #define LIBSTATISTICS_COLLECTOR_PUBLIC_TYPE LIBSTATISTICS_COLLECTOR_PUBLIC
  #define LIBSTATISTICS_COLLECTOR_LOCAL
#else
  #define LIBSTATISTICS_COLLECTOR_EXPORT __attribute__ ((visibility("default")))
  #define LIBSTATISTICS_COLLECTOR_IMPORT
  #if __GNUC__ >= 4
    #define LIBSTATISTICS_COLLECTOR_PUBLIC __attribute__ ((visibility("default")))
    #define LIBSTATISTICS_COLLECTOR_LOCAL  __attribute__ ((visibility("hidden")))
  #else
    #define LIBSTATISTICS_COLLECTOR_PUBLIC
    #define LIBSTATISTICS_COLLECTOR_LOCAL
  #endif
  #define LIBSTATISTICS_COLLECTOR_PUBLIC_TYPE
#endif

#endif  // LIBSTATISTICS_COLLECTOR__VISIBILITY_CONTROL_HPP_
