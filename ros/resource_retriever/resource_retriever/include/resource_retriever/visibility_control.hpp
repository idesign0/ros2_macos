// Copyright 2017 Open Source Robotics Foundation, Inc.
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

/* This header must be included by all rclcpp headers which declare symbols
 * which are defined in the rclcpp library. When not building the rclcpp
 * library, i.e. when using the headers in other package's code, the contents
 * of this header change the visibility of certain symbols which the rclcpp
 * library cannot have, but the consuming code must have inorder to link.
 */

#ifndef RESOURCE_RETRIEVER__VISIBILITY_CONTROL_HPP_
#define RESOURCE_RETRIEVER__VISIBILITY_CONTROL_HPP_

// This logic was borrowed (then namespaced) from the examples on the gcc wiki:
//     https://gcc.gnu.org/wiki/Visibility

#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define RESOURCE_RETRIEVER_EXPORT __attribute__ ((dllexport))
    #define RESOURCE_RETRIEVER_IMPORT __attribute__ ((dllimport))
  #else
    #define RESOURCE_RETRIEVER_EXPORT __declspec(dllexport)
    #define RESOURCE_RETRIEVER_IMPORT __declspec(dllimport)
  #endif
  #ifdef RESOURCE_RETRIEVER_BUILDING_LIBRARY
    #define RESOURCE_RETRIEVER_PUBLIC RESOURCE_RETRIEVER_EXPORT
  #else
    #define RESOURCE_RETRIEVER_PUBLIC RESOURCE_RETRIEVER_IMPORT
  #endif
  #define RESOURCE_RETRIEVER_PUBLIC_TYPE RESOURCE_RETRIEVER_PUBLIC
  #define RESOURCE_RETRIEVER_LOCAL
#else
  #define RESOURCE_RETRIEVER_EXPORT __attribute__ ((visibility("default")))
  #define RESOURCE_RETRIEVER_IMPORT
  #if __GNUC__ >= 4
    #define RESOURCE_RETRIEVER_PUBLIC __attribute__ ((visibility("default")))
    #define RESOURCE_RETRIEVER_LOCAL  __attribute__ ((visibility("hidden")))
  #else
    #define RESOURCE_RETRIEVER_PUBLIC
    #define RESOURCE_RETRIEVER_LOCAL
  #endif
  #define RESOURCE_RETRIEVER_PUBLIC_TYPE
#endif

#endif  // RESOURCE_RETRIEVER__VISIBILITY_CONTROL_HPP_
