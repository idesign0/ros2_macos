find_package(Boost REQUIRED COMPONENTS filesystem)
find_package(ament_cmake_gtest REQUIRED)

file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/resources" TEST_RESOURCES_DIR)
if(WIN32)
    # Correct directory separator for Windows
    string(REPLACE "\\" "\\\\" TEST_RESOURCES_DIR "${TEST_RESOURCES_DIR}")
endif()
configure_file(resources/config.h.in "${CMAKE_CURRENT_BINARY_DIR}/resources/config.h")
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ament_add_gtest(test_basics test_basics.cpp)
target_link_libraries(test_basics ${PROJECT_NAME})

ament_add_gtest(test_point_inclusion test_point_inclusion.cpp)
target_link_libraries(test_point_inclusion ${PROJECT_NAME} Boost::filesystem)

ament_add_gtest(test_bounding_sphere test_bounding_sphere.cpp)
target_link_libraries(test_bounding_sphere ${PROJECT_NAME} Boost::filesystem)

ament_add_gtest(test_bounding_box test_bounding_box.cpp)
target_link_libraries(test_bounding_box ${PROJECT_NAME})

ament_add_gtest(test_bounding_cylinder test_bounding_cylinder.cpp)
target_link_libraries(test_bounding_cylinder ${PROJECT_NAME})

ament_add_gtest(test_create_mesh test_create_mesh.cpp)
target_link_libraries(test_create_mesh ${PROJECT_NAME})

ament_add_gtest(test_loaded_meshes test_loaded_meshes.cpp)
target_link_libraries(test_loaded_meshes ${PROJECT_NAME} Boost::filesystem)

ament_add_gtest(test_shapes test_shapes.cpp)
target_link_libraries(test_shapes ${PROJECT_NAME})

ament_add_gtest(test_ray_intersection test_ray_intersection.cpp)
target_link_libraries(test_ray_intersection ${PROJECT_NAME} Boost::filesystem)

ament_add_gtest(test_body_operations test_body_operations.cpp)
target_link_libraries(test_body_operations ${PROJECT_NAME} Boost::filesystem)
