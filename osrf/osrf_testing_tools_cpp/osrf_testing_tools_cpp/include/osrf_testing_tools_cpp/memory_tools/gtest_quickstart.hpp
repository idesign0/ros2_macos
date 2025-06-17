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

#ifndef OSRF_TESTING_TOOLS_CPP__MEMORY_TOOLS__GTEST_QUICKSTART_HPP_
#define OSRF_TESTING_TOOLS_CPP__MEMORY_TOOLS__GTEST_QUICKSTART_HPP_

#include <map>
#include <string>
#include <utility>

#include <gtest/gtest.h>

#include "./memory_tools.hpp"
#include "./testing_helpers.hpp"

/// This header provides convenience patterns for using memory tools in googletest based tests.
/**
 * Here's a typical example:
 *
 *   #include <gtest/gtest.h>
 *   #include "osrf_testing_tools_cpp/memory_tools/gtest_quickstart.hpp"
 *
 *   TEST(test_my_thing, some_test_case) {
 *     osrf_testing_tools_cpp::memory_tools::ScopedQuickstartGtest sqg;
 *     int return_value;
 *     EXPECT_NO_MEMORY_OPERATIONS({
 *       return_value = some_function_that_should_not_use_memory();
 *     });
 *     ASSERT_EQ(0, return_value);
 *   }
 *
 * The arguments to ScopedQuickstartGtest() are perfect forwarded to the
 * constructor of the QuickstartConfiguration class, which means you can
 * configure away from the default settings.
 * For example setting a custom unexpected malloc message:
 *
 *   osrf_testing_tools_cpp::memory_tools::ScopedQuickstartGtest sqg({
 *     {"malloc", "my custom unexpected malloc message"},
 *   });
 *
 * Or making it print the backtrace when an unexpected free is encountered:
 *
 *   osrf_testing_tools_cpp::memory_tools::ScopedQuickstartGtest sqg({
 *     {"free", true},
 *   });
 *
 * Or changing both for all types:
 *
 *   osrf_testing_tools_cpp::memory_tools::ScopedQuickstartGtest sqg({
 *     {"malloc", {"my custom unexpected malloc message", true}},
 *     {"realloc", {"my custom unexpected realloc message", true}},
 *     {"calloc", {"my custom unexpected calloc message", true}},
 *     {"free", {"my custom unexpected free message", true}},
 *   });
 */

namespace osrf_testing_tools_cpp
{
namespace memory_tools
{

/// Wrapper for configuration information used during memory tools setup.
class QuickstartConfiguration
{
public:
  using ConfigMap = std::map<
    std::string,  // one of malloc, realloc, calloc, free
    std::pair<
      std::string,  // error message or "" for default
      bool  // whether or not the backtrace should be printed
    >
  >;

  /// Default settings.
  QuickstartConfiguration()
  : config_({
    {"malloc", {"unexpected call to malloc", false}},
    {"realloc", {"unexpected call to realloc", false}},
    {"calloc", {"unexpected call to calloc", false}},
    {"free", {"unexpected call to free", false}},
  })
  {}

  /// Custom setting which only changes the print backtrace behavior.
  explicit
  QuickstartConfiguration(bool should_print_backtrace)
  : QuickstartConfiguration()
  {
    for (auto & kv_pair : config_) {
      kv_pair.second.second = should_print_backtrace;
    }
  }

  /// Completely custom settings for some or all memory operations.
  explicit
  QuickstartConfiguration(const ConfigMap & config)
  : QuickstartConfiguration()
  {
    for (const auto & kv_pair : config) {
      if (config_.count(kv_pair.first) == 0) {
        throw std::runtime_error("unexpected QuickstartConfiguration key '" + kv_pair.first + "'");
      }
      config_[kv_pair.first] = kv_pair.second;
    }
  }

  /// Custom settings where only the error message changes for some or all memory operations.
  explicit
  QuickstartConfiguration(const std::map<std::string, std::string> & config_with_error_message)
  : QuickstartConfiguration()
  {
    for (const auto & kv_pair : config_with_error_message) {
      if (config_.count(kv_pair.first) == 0) {
        throw std::runtime_error("unexpected QuickstartConfiguration key '" + kv_pair.first + "'");
      }
      config_[kv_pair.first] = {kv_pair.second, config_[kv_pair.first].second};
    }
  }

  /// Custom settings where only the print backtrace changes for some or all memory operations.
  explicit
  QuickstartConfiguration(const std::map<std::string, bool> & config_with_error_message)
  : QuickstartConfiguration()
  {
    for (const auto & kv_pair : config_with_error_message) {
      if (config_.count(kv_pair.first) == 0) {
        throw std::runtime_error("unexpected QuickstartConfiguration key '" + kv_pair.first + "'");
      }
      config_[kv_pair.first] = {config_[kv_pair.first].first, kv_pair.second};
    }
  }

  /// Return the internally stored configuration map.
  const ConfigMap &
  get_config() const
  {
    return config_;
  }

private:
  ConfigMap config_;
};

/// Quickstart function for enabling memory tools when using with googletest.
/**
 * This function will initialize memory tools, setup callbacks for unexpected
 * calls to each memory function and make gtest fail (non-fatal, i.e. EXPECT
 * and not ASSERT) if unexpected memory calls are made, and then enable
 * in the current thread monitoring.
 *
 * If you want monitoring in all threads you will need to enable that yourself.
 *
 * After calling this, you can use the ``EXPECT_NO_MEMORY_OPERATIONS`` and
 * related macros.
 *
 * Even after this memory tools may not be working (if LD_PRELOAD was not used)
 * or if you're on an operating system that doesn't support memory tools.
 *
 * Parameters are forwarded to the QuickstartConfiguration class's constructor.
 *
 * \returns true if memory tools is working, and false if not
 */
template<typename ...Args>
bool
quickstart_gtest_setup(Args &&... args)
{
  QuickstartConfiguration quickstart_config(std::forward<Args>(args)...);
  const auto & config_map = quickstart_config.get_config();
  osrf_testing_tools_cpp::memory_tools::initialize();

  auto callback_factory = [](const std::string & message, bool should_print_backtrace) {
    return
      [message, should_print_backtrace]
      (osrf_testing_tools_cpp::memory_tools::MemoryToolsService & service) {
        ADD_FAILURE() << message;
          // this will cause a bracktrace to be printed for each unexpected malloc
          if (should_print_backtrace) {
            service.print_backtrace();
          }
      };
  };

  for (const auto & kv_pair : config_map) {
    if ("malloc" == kv_pair.first) {
      osrf_testing_tools_cpp::memory_tools::on_unexpected_malloc(
        callback_factory(kv_pair.second.first, kv_pair.second.second));
    } else if ("realloc" == kv_pair.first) {
      osrf_testing_tools_cpp::memory_tools::on_unexpected_realloc(
        callback_factory(kv_pair.second.first, kv_pair.second.second));
    } else if ("calloc" == kv_pair.first) {
      osrf_testing_tools_cpp::memory_tools::on_unexpected_calloc(
        callback_factory(kv_pair.second.first, kv_pair.second.second));
    } else if ("free" == kv_pair.first) {
      osrf_testing_tools_cpp::memory_tools::on_unexpected_free(
        callback_factory(kv_pair.second.first, kv_pair.second.second));
    } else {
      throw std::runtime_error("unexpected config key '" + kv_pair.first + "'");
    }
  }
  osrf_testing_tools_cpp::memory_tools::enable_monitoring();

  return osrf_testing_tools_cpp::memory_tools::is_working();
}

/// Quickstart function for cleaning up after the quickstart_gtest_setup() function.
inline
void
quickstart_gtest_teardown()
{
  osrf_testing_tools_cpp::memory_tools::disable_monitoring();
  osrf_testing_tools_cpp::memory_tools::uninitialize();
}

/// RAII-style scoped setup and teardown for memory tools used with googletest.
class ScopedQuickstartGtest
{
public:
  template<typename... Args>
  explicit
  ScopedQuickstartGtest(Args &&... args)
  : is_working_(quickstart_gtest_setup(std::forward<Args>(args)...))
  {}

  virtual
  ~ScopedQuickstartGtest()
  {
    quickstart_gtest_teardown();
  }

  /// Return true if memory tools was installed correctly and working.
  bool
  memory_tools_is_working() const
  {
    return is_working_;
  }

private:
  bool is_working_;
};

}  // namespace memory_tools
}  // namespace osrf_testing_tools_cpp

#endif  // OSRF_TESTING_TOOLS_CPP__MEMORY_TOOLS__GTEST_QUICKSTART_HPP_
