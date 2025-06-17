// Copyright 2015 Open Source Robotics Foundation, Inc.
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

// Code to do replacing of malloc/free/etc... inspired by:
//   https://dxr.mozilla.org/mozilla-central/rev/
//    cc9c6cd756cb744596ba039dcc5ad3065a7cc3ea/memory/build/replace_malloc.c

#ifndef OSRF_TESTING_TOOLS_CPP__MEMORY_TOOLS__MEMORY_TOOLS_HPP_
#define OSRF_TESTING_TOOLS_CPP__MEMORY_TOOLS__MEMORY_TOOLS_HPP_

#include "./initialize.hpp"
#include "./is_working.hpp"
#include "./memory_tools_service.hpp"
#include "./monitoring.hpp"
#include "./register_hooks.hpp"
#include "./testing_helpers.hpp"
#include "./visibility_control.hpp"

#endif  // OSRF_TESTING_TOOLS_CPP__MEMORY_TOOLS__MEMORY_TOOLS_HPP_
