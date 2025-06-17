#include "./count_function_occurrences_in_backtrace.hpp"

void func()
{
  printf("entering func()\n");
  static size_t recurse_count = 0;
  if (
    recurse_count++ < 10 &&
    osrf_testing_tools_cpp::memory_tools::count_function_occurrences_in_backtrace(func) <= 2)
  {
    func();
  }
  printf("exiting func(%zu)\n", recurse_count);
}
