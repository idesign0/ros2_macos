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

#include <gtest/gtest.h>
#include <gtest/gtest-spi.h>

#include <thread>

#include "osrf_testing_tools_cpp/memory_tools/testing_helpers.hpp"
#include "osrf_testing_tools_cpp/memory_tools/memory_tools.hpp"
#include "osrf_testing_tools_cpp/scope_exit.hpp"

void my_first_function(const std::string & str)
{
  osrf_testing_tools_cpp::memory_tools::guaranteed_malloc(str);
}

int my_second_function(int a, int b)
{
  return a + b;
}

TEST(TestMemoryTools, test_example) {
  // See the comment in my_first_function() for why we need this.
  const std::string dummy("hello");

  // you must initialize memory tools, but uninitialization is optional
  osrf_testing_tools_cpp::memory_tools::initialize();
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    osrf_testing_tools_cpp::memory_tools::uninitialize();
  });

  // create a callback for "unexpected" mallocs that does a non-fatal gtest
  // failure and then register it with memory tools
  auto on_unexpected_malloc =
    [](osrf_testing_tools_cpp::memory_tools::MemoryToolsService & service) {
      ADD_FAILURE() << "unexpected malloc";
      // this will cause a bracktrace to be printed for each unexpected malloc
      service.print_backtrace();
    };
  osrf_testing_tools_cpp::memory_tools::on_unexpected_malloc(on_unexpected_malloc);

  // at this point, you'll still not get callbacks, since monitoring is not enabled
  // so calling either user function should not fail
  my_first_function(dummy);
  EXPECT_EQ(my_second_function(1, 2), 3);

  // enabling monitoring will allow checking to begin, but the default state is
  // that dynamic memory calls are expected, so again either function will pass
  osrf_testing_tools_cpp::memory_tools::enable_monitoring();
  ASSERT_TRUE(osrf_testing_tools_cpp::memory_tools::is_working());
  my_first_function(dummy);
  EXPECT_EQ(my_second_function(1, 2), 3);

  // if you then tell memory tools that malloc is unexpected, then it will call
  // your above callback, at least until you indicate malloc is expected again
  EXPECT_NONFATAL_FAILURE({
    EXPECT_NO_MALLOC({
      my_first_function(dummy);
    });
  }, "unexpected malloc");
  // There are also explicit begin/end functions if you need variables to leave the scope
  osrf_testing_tools_cpp::memory_tools::expect_no_malloc_begin();
  int result = my_second_function(1, 2);
  osrf_testing_tools_cpp::memory_tools::expect_no_malloc_end();
  EXPECT_EQ(result, 3);

  // enable monitoring only works in the current thread, but you can enable it for all threads
  osrf_testing_tools_cpp::memory_tools::enable_monitoring_in_all_threads();
  std::thread t1([&dummy]() {
    EXPECT_NONFATAL_FAILURE({
      EXPECT_NO_MALLOC({
        my_first_function(dummy);
      });
    }, "unexpected malloc");
    osrf_testing_tools_cpp::memory_tools::expect_no_malloc_begin();
    int result = my_second_function(1, 2);
    osrf_testing_tools_cpp::memory_tools::expect_no_malloc_end();
    EXPECT_EQ(result, 3);
  });
  t1.join();

  // disabling monitoring in all threads should not catch the malloc in my_first_function()
  osrf_testing_tools_cpp::memory_tools::disable_monitoring_in_all_threads();
  osrf_testing_tools_cpp::memory_tools::enable_monitoring();
  std::thread t2([&dummy]() {
    EXPECT_NO_MALLOC({
      my_first_function(dummy);
    });
    osrf_testing_tools_cpp::memory_tools::expect_no_malloc_begin();
    int result = my_second_function(1, 2);
    osrf_testing_tools_cpp::memory_tools::expect_no_malloc_end();
    EXPECT_EQ(result, 3);
  });
  t2.join();
}
