// Copyright 2018 Open Source Robotics Foundation, Inc.
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

#ifndef OSRF_TESTING_TOOLS_CPP__VARIANT_HELPER_HPP_
#define OSRF_TESTING_TOOLS_CPP__VARIANT_HELPER_HPP_

#if defined(__has_include)

# if __has_include(<variant>)
#  if _MSC_VER < 2000  // this is any version @ VS 2017 and earlier
// VS 2017 (_MSC_VER or 19XX) has <variant>, but it just contains an error macro...
#   define __SHOULD_USE_MPARK_VARIANT 1
#  else  // _MSC_VER < 2000
#   define __SHOULD_USE_MPARK_VARIANT 0
#  endif  // _MSC_VER < 2000
# else  // __has_include(<variant>)
#  define __SHOULD_USE_MPARK_VARIANT 1
# endif  // __has_include(<variant>)

#else  // defined(__has_include)
# define __SHOULD_USE_MPARK_VARIANT 1
#endif  // defined(__has_include)

#if __SHOULD_USE_MPARK_VARIANT

// This is a header-only version of std::variant (part of C++17) for C++14.
// In the future this could be replaced with #include <variant>.
#include "./vendor/mpark/variant/variant.hpp"

namespace std
{

using namespace mpark;  // NOLINT(build/namespaces)

}  // namespace std

#else  // __SHOULD_USE_MPARK_VARIANT

#include <variant>

#endif  // __SHOULD_USE_MPARK_VARIANT

#undef __SHOULD_USE_MPARK_VARIANT

#endif  // OSRF_TESTING_TOOLS_CPP__VARIANT_HELPER_HPP_
