cmake_minimum_required(VERSION 2.8.3)

project(kdl_parser_py)

find_package(catkin REQUIRED
  COMPONENTS urdf
)

catkin_package(
  LIBRARIES ${PROJECT_NAME}
  CATKIN_DEPENDS urdf
)

catkin_python_setup()

catkin_install_python(PROGRAMS ${PROJECT_NAME}/urdf.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})

if(CATKIN_ENABLE_TESTING)
  find_package(catkin REQUIRED COMPONENTS rostest)
  add_rostest(test/test_kdl_parser.launch)
endif()
