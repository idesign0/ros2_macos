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

#include <atomic>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

#include "./safe_fwrite.hpp"
#include "osrf_testing_tools_cpp/memory_tools/verbosity.hpp"

namespace osrf_testing_tools_cpp
{
namespace memory_tools
{

static
VerbosityLevel
get_verbosity_level_from_env()
{
#if !defined(_WIN32)
  const char * value = std::getenv("MEMORY_TOOLS_VERBOSITY");
  size_t size_of_value = 0;
  if (value) {
    size_of_value = strnlen(value, 2);
  }
#else
  char value[256];
  size_t size_of_value;
  errno_t my_errno = getenv_s(&size_of_value, value, sizeof(value), "MEMORY_TOOLS_VERBOSITY");
  if (0 != my_errno) {
    throw std::runtime_error("getenv_s() falied");
  }
#endif
  if (!value || size_of_value == 0) {
    return VerbosityLevel::quiet;
  }
  if (0 == std::strncmp("quiet", value, 5) || 0 == std::strncmp("QUIET", value, 5)) {
    return VerbosityLevel::quiet;
  }
  if (0 == std::strncmp("debug", value, 5) || 0 == std::strncmp("DEBUG", value, 5)) {
    return VerbosityLevel::debug;
  }
  if (0 == std::strncmp("trace", value, 5) || 0 == std::strncmp("TRACE", value, 5)) {
    return VerbosityLevel::trace;
  }
  SAFE_FWRITE(stderr, "[memory_tools][WARN] Given MEMORY_TOOLS_VERBOSITY=");
  SAFE_FWRITE(stderr, value);
  SAFE_FWRITE(stderr, " but that is not one of {quiet, debug, trace}, using quiet.\n");
  return VerbosityLevel::quiet;
}

static std::atomic<VerbosityLevel> g_verbosity_level(get_verbosity_level_from_env());

VerbosityLevel
get_verbosity_level()
{
  return g_verbosity_level.load();
}

VerbosityLevel
set_verbosity_level(VerbosityLevel verbosity_level)
{
  return g_verbosity_level.exchange(verbosity_level);
}

}  // namespace memory_tools
}  // namespace osrf_testing_tools_cpp
