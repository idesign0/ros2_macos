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

#ifndef RTTEST__UTILS_HPP_
#define RTTEST__UTILS_HPP_

#include <stdint.h>
#include <time.h>

#define NSEC_PER_SEC 1000000000

static inline bool timespec_gt(
  const struct timespec * t1,
  const struct timespec * t2)
{
  if (t1->tv_sec > t2->tv_sec) {
    return true;
  }
  if (t1->tv_sec < t2->tv_sec) {
    return false;
  }
  return t1->tv_nsec > t2->tv_nsec;
}

static inline void normalize_timespec(struct timespec * t)
{
  // TODO(jacquelinekay) improve
  while (t->tv_nsec >= NSEC_PER_SEC) {
    t->tv_nsec -= NSEC_PER_SEC;
    t->tv_sec++;
  }
}

static inline void add_timespecs(
  const struct timespec * t1,
  const struct timespec * t2,
  struct timespec * dst)
{
  dst->tv_sec = t1->tv_sec + t2->tv_sec;
  dst->tv_nsec = t1->tv_nsec + t2->tv_nsec;
  normalize_timespec(dst);
}

static inline bool subtract_timespecs(
  const struct timespec * t1,
  const struct timespec * t2,
  struct timespec * dst)
{
  if (timespec_gt(t2, t1)) {
    return subtract_timespecs(t2, t1, dst);
  }

  dst->tv_sec = t1->tv_sec - t2->tv_sec;
  dst->tv_nsec = t1->tv_nsec - t2->tv_nsec;

  normalize_timespec(dst);
  return true;
}

static inline uint64_t timespec_to_uint64(const struct timespec * t)
{
  return static_cast<uint64_t>(t->tv_sec) * NSEC_PER_SEC + static_cast<uint64_t>(t->tv_nsec);
}

static inline void uint64_to_timespec(const uint64_t input, struct timespec * t)
{
  uint64_t nsecs = input % NSEC_PER_SEC;
  uint64_t secs = (input - nsecs) / NSEC_PER_SEC;
  t->tv_sec = static_cast<time_t>(secs);
  t->tv_nsec = static_cast<long>(nsecs); // NOLINT for C type long
}

static inline void multiply_timespec(
  const struct timespec * t, const uint32_t i,
  struct timespec * result)
{
  uint64_t result_nsec = i * timespec_to_uint64(t);
  uint64_to_timespec(result_nsec, result);
}

#endif  // RTTEST__UTILS_HPP_
