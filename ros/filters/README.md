# Filters

## Usage with ament_cmake

Here is recommended approach on how to link `filters` to your project, using the `filters::realtime_circular_buffer` target.

```cmake
find_package(filters CONFIG REQUIRED)

add_library(my_library)
# Other library stuff here

target_link_libraries(my_library PUBLIC filters::realtime_circular_buffer)
```

For more information on using ament_cmake, 
see the [ament_cmake](https://docs.ros.org/en/rolling/How-To-Guides/Ament-CMake-Documentation.html)
tutorial.

Filters creates all of the following CMake targets, including:
* filters::realtime_circular_buffer
* filters::filter_chain
* filters::mean
* filters::params
* filters::increment
* filters::median
* filters::transfer_function

It is recommended to only link to the libraries needed.
Linking to `filters::filter_base` pulls in all necessary libraries and include directories for targets with classes that extend `FilterBase`. This is useful if you are writing your own filter.
