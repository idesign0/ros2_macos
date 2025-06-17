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

#ifndef MEMORY_TOOLS__IMPL__UNIX_COMMON_HPP_
#define MEMORY_TOOLS__IMPL__UNIX_COMMON_HPP_

#include <cstddef>

void
complete_static_initialization();

bool &
get_static_initialization_complete();

extern "C"
{

void *
unix_replacement_malloc(size_t size, void *(*original_malloc)(size_t));

void *
unix_replacement_realloc(void * memory_in, size_t size, void *(*original_realloc)(void *, size_t));

void *
unix_replacement_calloc(size_t count, size_t size, void *(*original_calloc)(size_t, size_t));

void
unix_replacement_free(void * memory, void (*original_free)(void *));

}  // extern "C"

#endif  // MEMORY_TOOLS__IMPL__UNIX_COMMON_HPP_
