# eigen3_cmake_module

On Ubuntu Bionic, the `Eigen3` CMake package offers exported targets and non-standard CMake variables.
This is a problem for packages using [ament_cmake](https://github.com/ament/ament_cmake).

Targets using `ament_target_dependencies(my_target Eigen3)` will fail to find `Eigen3` headers at compile time.
Downstream packages will also fail to find `Eigen3` headers at compile time, even if your package uses `ament_export_dependencies(Eigen3)`.

This package adds a [CMake find module](https://cmake.org/cmake/help/v3.14/manual/cmake-developer.7.html#find-modulesjj) for [Eigen3](https://eigen.tuxfamily.org/dox/) that sets [standard CMake variables](https://cmake.org/cmake/help/v3.5/manual/cmake-developer.7.html#standard-variable-names).
ROS 2 packages using `Eigen3` should use this package to avoid these problems.

## Using this package

### Edit your CMakeLists.txt
In your `CMakeLists.txt`, call `find_package()` on this package using `REQUIRED`.
Afterwards, find `Eigen3` with or without `REQUIRED` as appropriate for your package.

```CMake
find_package(eigen3_cmake_module REQUIRED)
find_package(Eigen3)
```

#### Using with ament_cmake
If your package uses [ament_cmake](https://github.com/ament/ament_cmake), then use `ament_target_dependencies()` to give your library or executable targets access to the `Eigen3` headers.

```CMake
# add_library(my_thing ...)
# # OR
# add_executable(my_thing ...)
# ...
ament_target_dependencies(my_thing Eigen3)
```

If `Eigen3` appears in headers installed by your package, then downstream packages will need the `Eigen3` headers too.
Call `ament_export_dependencies()` on this package, and afterwards do the same for `Eigen3`.

```CMake
ament_export_dependencies(eigen3_cmake_module)
ament_export_dependencies(Eigen3)
```

#### Using without ament_cmake

If your package does not use `ament_cmake`, then use `target_include_directories()` to give your targets access to the `Eigen3` headers.

```CMake
target_include_directories(my_target PUBLIC "${Eigen3_INCLUDE_DIRS}")
```

If `Eigen3` is used in headers installed by your package, then your `<project>-config.cmake` must find both this package and `Eigen3`.
Afterwards the `Eigen3` headers should be added to your package's include variable.

```CMake
find_package(eigen3_cmake_module)
if (NOT eigen3_cmake_module_FOUND)
  set(your_package_name_FOUND 0)
  return()
endif()

find_package(Eigen3)
if (NOT Eigen3_FOUND)
  set(your_package_name_FOUND 0)
  return()
endif()

list(APPEND your_package_name_INCLUDE_DIRS ${Eigen3_INCLUDE_DIRS})
```

### Edit your package.xml

Add a buildtool dependency to this package, and a build dependency to `Eigen3` to your `package.xml`.

```xml
<buildtool_depend>eigen3_cmake_module</buildtool_depend>
<build_depend>eigen</build_depend>
```

If your package uses `Eigen3` in public headers, then **also add** these tags so downstream packages also depend on this package and `Eigen3`.

```xml
<buildtool_export_depend>eigen3_cmake_module</buildtool_export_depend>
<build_export_depend>eigen</build_export_depend>
```

`<exec_depend>` is not necessary because `Eigen3` is a header only library.
