cmake_minimum_required(VERSION 3.22)
project(rsl-install-test CXX)

add_executable(test-rsl-install install.cpp)

if(PROJECT_IS_TOP_LEVEL)
    # Test consumption via find_package interface and ament_target_dependencies
    find_package(ament_cmake_ros REQUIRED)
    find_package(rsl 1.1.0 EXACT REQUIRED)

    ament_target_dependencies(test-rsl-install rsl)
else()
    # Test consumption via in-tree build
    target_link_libraries(test-rsl-install PRIVATE rsl::rsl)
endif()
