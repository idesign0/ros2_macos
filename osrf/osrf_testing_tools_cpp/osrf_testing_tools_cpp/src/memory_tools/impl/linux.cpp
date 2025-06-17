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

#if defined(__linux__)

#include <cstdio>
#include <cstdlib>
#include <dlfcn.h>

#include "./static_allocator.hpp"
#include "./unix_common.hpp"

template<typename FunctionPointerT>
FunctionPointerT
find_original_function(const char * name)
{
  FunctionPointerT original_function = reinterpret_cast<FunctionPointerT>(dlsym(RTLD_NEXT, name));
  if (!original_function) {
    fprintf(stderr, "failed to get original function '%s' with dlsym() and RTLD_NEXT\n", name);
    exit(1);  // cannot throw, next best thing
  }
  // check to make sure we got one we want, see:
  //   http://optumsoft.com/dangers-of-using-dlsym-with-rtld_next/
  Dl_info dl_info;
  if (!dladdr(reinterpret_cast<void *>(original_function), &dl_info)) {
    fprintf(stderr,
      "failed to get information about function '%p' with dladdr()\n",
      reinterpret_cast<void *>(original_function));
    exit(1);  // cannot throw, next best thing
  }
  return original_function;
}

// An amount of memory that is greater than what is needed for static initialization
// for any test we run. It was found experimentally on Ubuntu Linux 16.04 x86_64.
static constexpr size_t STATIC_ALLOCATOR_SIZE = 0x800000;
using osrf_testing_tools_cpp::memory_tools::impl::StaticAllocator;
using StaticAllocatorT = StaticAllocator<STATIC_ALLOCATOR_SIZE>;
static uint8_t g_static_allocator_storage[sizeof(StaticAllocatorT)];

// Contains global allocator to make 100% sure to avoid Static Initialization Order Fiasco.
// "Construct on first use" idiom
static StaticAllocatorT *
get_static_allocator()
{
  // placement-new the static allocator in preallocated storage
  // which is used while finding the original memory functions
  static StaticAllocatorT * alloc = new (g_static_allocator_storage) StaticAllocatorT;
  return alloc;
}

// storage for original malloc/realloc/calloc/free
using MallocSignature = void * (*)(size_t);
static MallocSignature g_original_malloc = nullptr;
using ReallocSignature = void *(*)(void *, size_t);
static ReallocSignature g_original_realloc = nullptr;
using CallocSignature = void *(*)(size_t, size_t);
static CallocSignature g_original_calloc = nullptr;
using FreeSignature = void (*)(void *);
static FreeSignature g_original_free = nullptr;

// on shared library load, find and store the original memory function locations
static __attribute__((constructor)) void __linux_memory_tools_init(void)
{
  g_original_malloc = find_original_function<MallocSignature>("malloc");
  g_original_realloc = find_original_function<ReallocSignature>("realloc");
  g_original_calloc = find_original_function<CallocSignature>("calloc");
  g_original_free = find_original_function<FreeSignature>("free");

  complete_static_initialization();
}

extern "C"
{

void *
malloc(size_t size) noexcept
{
  if (!get_static_initialization_complete()) {
    return get_static_allocator()->allocate(size);
  }
  return unix_replacement_malloc(size, g_original_malloc);
}

void *
realloc(void * pointer, size_t size) noexcept
{
  if (!get_static_initialization_complete()) {
    return get_static_allocator()->reallocate(pointer, size);
  }
  return unix_replacement_realloc(pointer, size, g_original_realloc);
}

void *
calloc(size_t count, size_t size) noexcept
{
  if (!get_static_initialization_complete()) {
    return get_static_allocator()->zero_allocate(count, size);
  }
  return unix_replacement_calloc(count, size, g_original_calloc);
}

void
free(void * pointer) noexcept
{
  if (nullptr == pointer || get_static_allocator()->deallocate(pointer)) {
    // free of nullptr or,
    // memory was originally allocated by static allocator, no need to pass to "real" free
    return;
  }
  unix_replacement_free(pointer, g_original_free);
}

}  // extern "C"

#endif  // defined(__linux__)
