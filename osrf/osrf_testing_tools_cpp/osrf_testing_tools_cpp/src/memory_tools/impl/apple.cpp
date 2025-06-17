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

#if defined(__APPLE__)

#include <cstdlib>

#include "./unix_common.hpp"

// Pulled from:
//  https://github.com/emeryberger/Heap-Layers/blob/
//    076e9e7ef53b66380b159e40473b930f25cc353b/wrappers/macinterpose.h

// The interposition data structure (just pairs of function pointers),
// used an interposition table like the following:
//

typedef struct interpose_s
{
  void * new_func;
  void * orig_func;
} interpose_t;

#define OSX_INTERPOSE(newf, oldf) \
  __attribute__((used)) static const interpose_t \
  macinterpose__ ## newf ## __ ## oldf __attribute__ ((section("__DATA, __interpose"))) = { \
    reinterpret_cast<void *>(newf), \
    reinterpret_cast<void *>(oldf), \
  }

extern "C"
{

void *
apple_replacement_malloc(size_t size)
{
  return unix_replacement_malloc(size, malloc);
}

void *
apple_replacement_realloc(void * memory_in, size_t size)
{
  return unix_replacement_realloc(memory_in, size, realloc);
}

void *
apple_replacement_calloc(size_t count, size_t size)
{
  return unix_replacement_calloc(count, size, calloc);
}

void
apple_replacement_free(void * memory)
{
  return unix_replacement_free(memory, free);
}

OSX_INTERPOSE(apple_replacement_malloc, malloc);
OSX_INTERPOSE(apple_replacement_realloc, realloc);
OSX_INTERPOSE(apple_replacement_calloc, calloc);
OSX_INTERPOSE(apple_replacement_free, free);

}  // extern "C"

// End Interpose.

// on shared library load, find and store the original memory function locations
static __attribute__((constructor,used)) void __apple_memory_tools_init(void)
{
  complete_static_initialization();
}

#endif  // defined(__APPLE__)
