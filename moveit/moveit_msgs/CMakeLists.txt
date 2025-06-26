cmake_minimum_required(VERSION 3.5)
project(moveit_msgs)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

# Disable Wredundant-decls warnings since rosidl generates redundant function
#  declarations
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS 
    "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wno-redundant-decls")
endif()

find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(action_msgs REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(shape_msgs REQUIRED)
find_package(object_recognition_msgs REQUIRED)
find_package(octomap_msgs REQUIRED)
find_package(trajectory_msgs REQUIRED)


set(msg_files
  "msg/AllowedCollisionEntry.msg"
  "msg/AllowedCollisionMatrix.msg"
  "msg/AttachedCollisionObject.msg"
  "msg/BoundingVolume.msg"
  "msg/CartesianPoint.msg"
  "msg/CartesianTrajectory.msg"
  "msg/CartesianTrajectoryPoint.msg"
  "msg/CollisionObject.msg"
  "msg/ConstraintEvalResult.msg"
  "msg/Constraints.msg"
  "msg/CostSource.msg"
  "msg/ContactInformation.msg"
  "msg/DisplayTrajectory.msg"
  "msg/DisplayRobotState.msg"
  "msg/GenericTrajectory.msg"
  "msg/Grasp.msg"
  "msg/GripperTranslation.msg"
  "msg/JointConstraint.msg"
  "msg/JointLimits.msg"
  "msg/LinkPadding.msg"
  "msg/LinkScale.msg"
  "msg/MotionPlanRequest.msg"
  "msg/MotionPlanResponse.msg"
  "msg/MotionPlanDetailedResponse.msg"
  "msg/MotionSequenceItem.msg"
  "msg/MotionSequenceRequest.msg"
  "msg/MotionSequenceResponse.msg"
  "msg/MoveItErrorCodes.msg"
  "msg/TrajectoryConstraints.msg"
  "msg/ObjectColor.msg"
  "msg/OrientationConstraint.msg"
  "msg/OrientedBoundingBox.msg"
  "msg/PlaceLocation.msg"
  "msg/PlannerInterfaceDescription.msg"
  "msg/PlannerParams.msg"
  "msg/PlanningScene.msg"
  "msg/PlanningSceneComponents.msg"
  "msg/PlanningSceneWorld.msg"
  "msg/PlanningOptions.msg"
  "msg/PositionConstraint.msg"
  "msg/RobotState.msg"
  "msg/RobotTrajectory.msg"
  "msg/VisibilityConstraint.msg"
  "msg/WorkspaceParameters.msg"
  "msg/KinematicSolverInfo.msg"
  "msg/PositionIKRequest.msg"
)

set(srv_files
  "srv/GetMotionPlan.srv"
  "srv/GetStateValidity.srv"
  "srv/GetCartesianPath.srv"
  "srv/GetPlanningScene.srv"
  "srv/GraspPlanning.srv"
  "srv/ApplyPlanningScene.srv"
  "srv/QueryPlannerInterfaces.srv"
  "srv/GetMotionSequence.srv"
  "srv/GetPositionFK.srv"
  "srv/GetPositionIK.srv"
  "srv/GetPlannerParams.srv"
  "srv/SetPlannerParams.srv"
  "srv/UpdatePointcloudOctomap.srv"
  "srv/SaveMap.srv"
  "srv/LoadMap.srv"
  "srv/SaveRobotStateToWarehouse.srv"
  "srv/ListRobotStatesInWarehouse.srv"
  "srv/GetRobotStateFromWarehouse.srv"
  "srv/CheckIfRobotStateExistsInWarehouse.srv"
  "srv/RenameRobotStateInWarehouse.srv"
  "srv/DeleteRobotStateFromWarehouse.srv"
  "srv/ChangeControlDimensions.srv"
  "srv/ChangeDriftDimensions.srv"
)

set(action_files
  "action/ExecuteTrajectory.action"
  "action/MoveGroup.action"
  "action/MoveGroupSequence.action"
  "action/Pickup.action"
  "action/Place.action"
  "action/LocalPlanner.action"
  "action/GlobalPlanner.action"
  "action/HybridPlanner.action"
)

rosidl_generate_interfaces(${PROJECT_NAME}
  ${msg_files}
  ${srv_files}
  ${action_files}
  DEPENDENCIES
    action_msgs
    builtin_interfaces
    std_msgs
    geometry_msgs
    sensor_msgs
    shape_msgs
    object_recognition_msgs
    octomap_msgs
    trajectory_msgs
)

ament_export_dependencies(rosidl_default_runtime)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
