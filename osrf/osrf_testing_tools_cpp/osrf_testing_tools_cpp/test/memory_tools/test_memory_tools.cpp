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

#include "osrf_testing_tools_cpp/memory_tools/memory_tools.hpp"
#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "memory_tools/impl/static_allocator.hpp"

/**
 * Tests the dynamic memory checking tools.
 */
TEST(TestMemoryTools, test_allocation_checking_tools) {
  osrf_testing_tools_cpp::memory_tools::initialize();
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT({
    osrf_testing_tools_cpp::memory_tools::uninitialize();
  });

  size_t unexpected_mallocs = 0;
  auto on_unexpected_malloc =
    [&unexpected_mallocs]() {
      unexpected_mallocs++;
    };
  osrf_testing_tools_cpp::memory_tools::on_unexpected_malloc(on_unexpected_malloc);
  size_t unexpected_reallocs = 0;
  auto on_unexpected_realloc =
    [&unexpected_reallocs]() {
      unexpected_reallocs++;
    };
  osrf_testing_tools_cpp::memory_tools::on_unexpected_realloc(on_unexpected_realloc);
  size_t unexpected_callocs = 0;
  auto on_unexpected_calloc =
    [&unexpected_callocs]() {
      unexpected_callocs++;
    };
  osrf_testing_tools_cpp::memory_tools::on_unexpected_calloc(on_unexpected_calloc);
  size_t unexpected_frees = 0;
  auto on_unexpected_free =
    [&unexpected_frees]() {
      unexpected_frees++;
    };
  osrf_testing_tools_cpp::memory_tools::on_unexpected_free(on_unexpected_free);
  void * mem = nullptr;
  void * remem = nullptr;

  auto use_all_dynamic_memory_functions = [&mem, &remem]() -> void {
    mem = malloc(1024);
    ASSERT_NE(nullptr, mem);
    remem = realloc(mem, 2048);
    ASSERT_NE(nullptr, remem);
    free(remem);
    mem = calloc(1024, sizeof(void *));
    ASSERT_NE(nullptr, mem);
    free(mem);
  };

  // First try before enabling, should have no effect.
  use_all_dynamic_memory_functions();
  EXPECT_EQ(0u, unexpected_mallocs);
  EXPECT_EQ(0u, unexpected_reallocs);
  EXPECT_EQ(0u, unexpected_callocs);
  EXPECT_EQ(0u, unexpected_frees);

  // Enable checking, but no assert, should have no effect.
  osrf_testing_tools_cpp::memory_tools::enable_monitoring();
  use_all_dynamic_memory_functions();
  EXPECT_EQ(0u, unexpected_mallocs);
  EXPECT_EQ(0u, unexpected_reallocs);
  EXPECT_EQ(0u, unexpected_callocs);
  EXPECT_EQ(0u, unexpected_frees);

  // Enable no_* asserts, should increment all once.
  osrf_testing_tools_cpp::memory_tools::expect_no_malloc_begin();
  osrf_testing_tools_cpp::memory_tools::expect_no_realloc_begin();
  osrf_testing_tools_cpp::memory_tools::expect_no_calloc_begin();
  osrf_testing_tools_cpp::memory_tools::expect_no_free_begin();
  use_all_dynamic_memory_functions();
  osrf_testing_tools_cpp::memory_tools::expect_no_malloc_end();
  osrf_testing_tools_cpp::memory_tools::expect_no_realloc_end();
  osrf_testing_tools_cpp::memory_tools::expect_no_calloc_end();
  osrf_testing_tools_cpp::memory_tools::expect_no_free_end();
  EXPECT_EQ(1u, unexpected_mallocs);
  EXPECT_EQ(1u, unexpected_reallocs);
  EXPECT_EQ(1u, unexpected_callocs);
  EXPECT_EQ(2u, unexpected_frees);

  // Enable on malloc assert, only malloc should increment.
  osrf_testing_tools_cpp::memory_tools::expect_no_malloc_begin();
  use_all_dynamic_memory_functions();
  osrf_testing_tools_cpp::memory_tools::expect_no_malloc_end();
  EXPECT_EQ(2u, unexpected_mallocs);
  EXPECT_EQ(1u, unexpected_reallocs);
  EXPECT_EQ(1u, unexpected_callocs);
  EXPECT_EQ(2u, unexpected_frees);

  // Enable on realloc assert, only realloc should increment.
  osrf_testing_tools_cpp::memory_tools::expect_no_realloc_begin();
  use_all_dynamic_memory_functions();
  osrf_testing_tools_cpp::memory_tools::expect_no_realloc_end();
  EXPECT_EQ(2u, unexpected_mallocs);
  EXPECT_EQ(2u, unexpected_reallocs);
  EXPECT_EQ(1u, unexpected_callocs);
  EXPECT_EQ(2u, unexpected_frees);

  // Enable on calloc assert, only calloc should increment.
  osrf_testing_tools_cpp::memory_tools::expect_no_calloc_begin();
  use_all_dynamic_memory_functions();
  osrf_testing_tools_cpp::memory_tools::expect_no_calloc_end();
  EXPECT_EQ(2u, unexpected_mallocs);
  EXPECT_EQ(2u, unexpected_reallocs);
  EXPECT_EQ(2u, unexpected_callocs);
  EXPECT_EQ(2u, unexpected_frees);

  // Enable on free assert, only free should increment.
  osrf_testing_tools_cpp::memory_tools::expect_no_free_begin();
  use_all_dynamic_memory_functions();
  osrf_testing_tools_cpp::memory_tools::expect_no_free_end();
  EXPECT_EQ(2u, unexpected_mallocs);
  EXPECT_EQ(2u, unexpected_reallocs);
  EXPECT_EQ(2u, unexpected_callocs);
  EXPECT_EQ(4u, unexpected_frees);

  // Go again, after disabling asserts, should have no effect.
  use_all_dynamic_memory_functions();
  EXPECT_EQ(2u, unexpected_mallocs);
  EXPECT_EQ(2u, unexpected_reallocs);
  EXPECT_EQ(2u, unexpected_callocs);
  EXPECT_EQ(4u, unexpected_frees);

  // Go once more after disabling everything, should have no effect.
  osrf_testing_tools_cpp::memory_tools::disable_monitoring();
  use_all_dynamic_memory_functions();
  EXPECT_EQ(2u, unexpected_mallocs);
  EXPECT_EQ(2u, unexpected_reallocs);
  EXPECT_EQ(2u, unexpected_callocs);
  EXPECT_EQ(4u, unexpected_frees);
}

void my_first_function(const std::string& str)
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
  int output = my_second_function(1, 2);
  osrf_testing_tools_cpp::memory_tools::expect_no_malloc_end();
  EXPECT_EQ(output, 3);

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

/**
 * Tests the static allocator used during dynamic library loading.
 */
TEST(TestMemoryTools, test_static_allocation_alignment) {
  static constexpr size_t allocator_size = 0x1000;
  // Arbitrarily chosen values (somewhat observed values from OpenSSL static initialization).
  // Not all aligned to std::max_align_t on most platforms.
  static const std::vector<size_t> request_sizes = {
    24, 24, 24, 7, 82, 2424
  };
  static constexpr size_t max_align = alignof(std::max_align_t);

  osrf_testing_tools_cpp::memory_tools::impl::StaticAllocator<allocator_size> allocator;

  // Check that all returned memory blocks are aligned with the max_align type.
  for (const size_t request : request_sizes) {
    void * memory = allocator.allocate(request);
    ASSERT_EQ(reinterpret_cast<size_t>(memory) % max_align, size_t(0));
    ASSERT_TRUE(allocator.deallocate(memory));
  }
}
