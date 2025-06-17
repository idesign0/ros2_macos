cmake_minimum_required(VERSION 3.5)

project(control_msgs)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(action_msgs REQUIRED)
find_package(ament_cmake REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(trajectory_msgs REQUIRED)

set(msg_files
  msg/AdmittanceControllerState.msg
  msg/DynamicInterfaceGroupValues.msg
  msg/DynamicJointState.msg
  msg/GripperCommand.msg
  msg/InterfaceValue.msg
  msg/JointComponentTolerance.msg
  msg/JointControllerState.msg
  msg/JointJog.msg
  msg/JointTolerance.msg
  msg/JointTrajectoryControllerState.msg
  msg/MecanumDriveControllerState.msg
  msg/MultiDOFCommand.msg
  msg/MultiDOFStateStamped.msg
  msg/PidState.msg
  msg/SingleDOFState.msg
  msg/SingleDOFStateStamped.msg
  msg/SteeringControllerStatus.msg
)

set(action_files
  action/ParallelGripperCommand.action
  action/FollowJointTrajectory.action
  action/GripperCommand.action
  action/JointTrajectory.action
  action/PointHead.action
  action/SingleJointPosition.action
)

set(srv_files
  srv/QueryCalibrationState.srv
  srv/QueryTrajectoryState.srv
)

rosidl_generate_interfaces(${PROJECT_NAME}
  ${action_files}
  ${msg_files}
  ${srv_files}
  DEPENDENCIES
    action_msgs
    builtin_interfaces
    geometry_msgs
    sensor_msgs
    std_msgs
    trajectory_msgs
  ADD_LINTER_TESTS
)
ament_export_dependencies(rosidl_default_runtime)
ament_package()
