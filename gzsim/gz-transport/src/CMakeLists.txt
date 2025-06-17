# Collect source files into the "sources" variable and unit test files into the
# "gtest_sources" variable.
ign_get_libsources_and_unittests(sources gtest_sources)

if (MSVC)
  # Warning #4251 is the "dll-interface" warning that tells you when types used
  # by a class are not being exported. These generated source files have private
  # members that don't get exported, so they trigger this warning. However, the
  # warning is not important since those members do not need to be interfaced
  # with.
  set_source_files_properties(${sources} ${gtest_sources} COMPILE_FLAGS "/wd4251 /wd4146")
endif()

# Create the library target.
ign_create_core_library(SOURCES ${sources} CXX_STANDARD 17)

# Link the libraries that we always need.
target_link_libraries(${PROJECT_LIBRARY_TARGET_NAME}
  PUBLIC
    ignition-cmake${IGN_CMAKE_VER}::utilities
    ignition-msgs${IGN_MSGS_VER}::ignition-msgs${IGN_MSGS_VER}
    CPPZMQ::CPPZMQ
  PRIVATE
    ${ZeroMQ_TARGET}
)

# Windows system library provides UUID
if (NOT MSVC)
  target_link_libraries(${PROJECT_LIBRARY_TARGET_NAME}
    PUBLIC
      UUID::UUID
  )
endif()

# Build the unit tests.
ign_build_tests(TYPE UNIT SOURCES ${gtest_sources}
  TEST_LIST test_list
  LIB_DEPS ${EXTRA_TEST_LIB_DEPS})

foreach(test ${test_list})
  set(_env_vars)
  list(APPEND _env_vars "IGN_IP=127.0.0.1")
  list(APPEND _env_vars "IGN_VERBOSE=1")
  set_property(TEST ${test} PROPERTY ENVIRONMENT "${_env_vars}")

  # Inform each test of its output directory so it knows where to call the
  # auxiliary files from. Using a generator expression here is useful for
  # multi-configuration generators, like Visual Studio.
  target_compile_definitions(${test} PRIVATE
    "DETAIL_IGN_TRANSPORT_TEST_DIR=\"$<TARGET_FILE_DIR:${test}>\""
    "IGN_TEST_LIBRARY_PATH=\"$<TARGET_FILE_DIR:${PROJECT_LIBRARY_TARGET_NAME}>\"")
endforeach()

if(MSVC)
  # On Windows, UNIT_Discovery_TEST uses some socket functions and therefore
  # needs to link to the Windows socket library. An easy, maintainable way to
  # do that is to tell it to link to the ZeroMQ target, and let it get the
  # socket library dependency transitively.
  if(TARGET UNIT_Discovery_TEST)
    target_link_libraries(UNIT_Discovery_TEST
      ${ZeroMQ_TARGET})
  endif()
endif()

# Command line support.
add_subdirectory(cmd)
