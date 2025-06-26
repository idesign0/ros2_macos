# ===================================================================================
#  The OctoMap CMake configuration file
#
#             ** File generated automatically, do not modify **
#
#  Usage from an external project:
#    In your CMakeLists.txt, add these lines:
#
#    FIND_PACKAGE(OCTOMAP REQUIRED )
#    INCLUDE_DIRECTORIES(${OCTOMAP_INCLUDE_DIRS})
#    TARGET_LINK_LIBRARIES(MY_TARGET_NAME ${OCTOMAP_LIBRARIES})
#
#
#    This file will define the following variables:
#      - OCTOMAP_LIBRARIES      : The list of libraries to links against.
#      - OCTOMAP_LIBRARY_DIRS   : The directory where lib files are. Calling
#                                 LINK_DIRECTORIES with this path is NOT needed.
#      - OCTOMAP_INCLUDE_DIRS   : The OctoMap include directories.
#      - OCTOMAP_MAJOR_VERSION  : Major version.
#      - OCTOMAP_MINOR_VERSION  : Minor version.
#      - OCTOMAP_PATCH_VERSION  : Patch version.
#      - OCTOMAP_VERSION        : Major.Minor.Patch version.
#
# ===================================================================================


####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was octomap-config.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

set(OCTOMAP_MAJOR_VERSION "1")
set(OCTOMAP_MINOR_VERSION "9")
set(OCTOMAP_PATCH_VERSION "8")
set(OCTOMAP_VERSION "1.9.8")

set_and_check(OCTOMAP_INCLUDE_DIRS "/Users/dhruvpatel29/humble-ros2/src/moveit/octomap-release/octomap/include")
set_and_check(OCTOMAP_LIBRARY_DIRS "/Users/dhruvpatel29/humble-ros2/src/moveit/octomap-release/octomap/lib")

# Set library names
set(OCTOMAP_LIBRARIES
  "/Users/dhruvpatel29/humble-ros2/src/moveit/octomap-release/octomap/lib/liboctomap.dylib"
  "/Users/dhruvpatel29/humble-ros2/src/moveit/octomap-release/octomap/lib/liboctomath.dylib"
)

# Additionally set the variables using a lower-case project name.
# This fixes discovery for downstream packages that search for lower-case octomap, which has been common in downstream code:
set(octomap_MAJOR_VERSION "${OCTOMAP_MAJOR_VERSION}")
set(octomap_MINOR_VERSION "${OCTOMAP_MINOR_VERSION}")
set(octomap_PATCH_VERSION "${OCTOMAP_PATCH_VERSION}")
set(octomap_VERSION "${OCTOMAP_VERSION}")
set(octomap_INCLUDE_DIRS "${OCTOMAP_INCLUDE_DIRS}")
set(octomap_LIBRARY_DIRS "${OCTOMAP_LIBRARY_DIRS}")
set(octomap_LIBRARIES "${OCTOMAP_LIBRARIES}")



