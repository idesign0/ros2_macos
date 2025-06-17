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
#include <stdio.h>
#include <sched.h>

#include <rttest/rttest.h>

int i = 0;

void * my_loop_callback(void * args)
{
  ++i;
  return 0;
}

int main(int argc, char ** argv)
{
  rttest_set_sched_priority(98, SCHED_RR);

  if (rttest_read_args(argc, argv) != 0) {
    perror("Couldn't read arguments for rttest");
    return -1;
  }
  if (rttest_lock_memory() != 0) {
    perror("Couldn't lock memory");
    return -1;
  }
  rttest_lock_and_prefault_dynamic();

  rttest_spin(my_loop_callback, NULL);

  rttest_write_results();
  rttest_finish();

  return 0;
}
