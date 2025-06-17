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

#ifndef MEMORY_TOOLS__SAFE_FWRITE_HPP_
#define MEMORY_TOOLS__SAFE_FWRITE_HPP_

#include <cstdio>
#include <cstring>

#if defined(_WIN32)
// Limit the buffer size in the `fwrite` call to give an upper bound to buffer overrun in the case
// of non-null terminated `msg`.
#define SAFE_FWRITE(out, msg) fwrite(msg, sizeof(char), strnlen_s(msg, 4096), out)
#else
#define SAFE_FWRITE(out, msg) fwrite(msg, sizeof(char), strlen(msg), out)
#endif

#endif  // MEMORY_TOOLS__SAFE_FWRITE_HPP_
