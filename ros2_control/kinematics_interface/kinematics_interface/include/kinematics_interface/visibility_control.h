// Copyright (c) 2022, PickNik, Inc.
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

#ifndef KINEMATICS_INTERFACE__VISIBILITY_CONTROL_H_
#define KINEMATICS_INTERFACE__VISIBILITY_CONTROL_H_

// This logic was borrowed (then namespaced) from the examples on the gcc wiki:
//     https://gcc.gnu.org/wiki/Visibility

#if defined _WIN32 || defined __CYGWIN__
#ifdef __GNUC__
#define IK_PLUGIN_BASE_EXPORT __attribute__((dllexport))
#define IK_PLUGIN_BASE_IMPORT __attribute__((dllimport))
#else
#define IK_PLUGIN_BASE_EXPORT __declspec(dllexport)
#define IK_PLUGIN_BASE_IMPORT __declspec(dllimport)
#endif
#ifdef IK_PLUGIN_BASE_BUILDING_DLL
#define IK_PLUGIN_BASE_PUBLIC IK_PLUGIN_BASE_EXPORT
#else
#define IK_PLUGIN_BASE_PUBLIC IK_PLUGIN_BASE_IMPORT
#endif
#define IK_PLUGIN_BASE_PUBLIC_TYPE IK_PLUGIN_BASE_PUBLIC
#define IK_PLUGIN_BASE_LOCAL
#else
#define IK_PLUGIN_BASE_EXPORT __attribute__((visibility("default")))
#define IK_PLUGIN_BASE_IMPORT
#if __GNUC__ >= 4
#define IK_PLUGIN_BASE_PUBLIC __attribute__((visibility("default")))
#define IK_PLUGIN_BASE_LOCAL __attribute__((visibility("hidden")))
#else
#define IK_PLUGIN_BASE_PUBLIC
#define IK_PLUGIN_BASE_LOCAL
#endif
#define IK_PLUGIN_BASE_PUBLIC_TYPE
#endif

#endif  // KINEMATICS_INTERFACE__VISIBILITY_CONTROL_H_
