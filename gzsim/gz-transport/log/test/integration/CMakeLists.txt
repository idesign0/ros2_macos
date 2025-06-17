# Integration tests

ign_build_tests(
  TYPE "INTEGRATION"
  TEST_LIST logging_tests
  SOURCES
    recorder.cc
    playback.cc
    query.cc
  LIB_DEPS
    ${PROJECT_LIBRARY_TARGET_NAME}-log
    ${EXTRA_TEST_LIB_DEPS}
  INCLUDE_DIRS
    ${CMAKE_BINARY_DIR}/test/
)

# Test symbols having the right name on linux only.
if (UNIX AND NOT APPLE)
  add_test(NAME INTEGRATION_log_versioned_symbols
    COMMAND bash ${PROJECT_BINARY_DIR}/test/integration/all_symbols_have_version.bash $<TARGET_FILE:${PROJECT_LIBRARY_TARGET_NAME}-log>)
endif()

foreach(test_target ${logging_tests})
  set(_env_vars)
  list(APPEND _env_vars "IGN_IP=127.0.0.1")
  list(APPEND _env_vars "IGN_TRANSPORT_LOG_SQL_PATH=${PROJECT_SOURCE_DIR}/log/sql")
  set_tests_properties(${test_target} PROPERTIES
    ENVIRONMENT "${_env_vars}")
  target_compile_definitions(${test_target}
    PRIVATE IGN_TRANSPORT_LOG_SQL_PATH="${PROJECT_SOURCE_DIR}/log/sql")
  target_compile_definitions(${test_target}
    PRIVATE IGN_TRANSPORT_LOG_BUILD_PATH="$<TARGET_FILE_DIR:${test_target}>")

endforeach()

set (aux
  topicChirp_aux.cc
)

foreach(source_file ${aux})
  string(REGEX REPLACE ".cc" "" AUX_EXECUTABLE ${source_file})
  set(BINARY_NAME ${TEST_TYPE}_${AUX_EXECUTABLE})

  ign_add_executable(${BINARY_NAME} ${AUX_EXECUTABLE}.cc)

  # Include the interface directories that we always need.
  ign_target_interface_include_directories(${BINARY_NAME}
    ${PROJECT_LIBRARY_TARGET_NAME})

  # Link the libraries that we always need.
  target_link_libraries(${BINARY_NAME}
    PRIVATE
      ${PROJECT_LIBRARY_TARGET_NAME}
      ${log_lib_target}
      gtest
      ${EXTRA_TEST_LIB_DEPS}
  )

  if(UNIX)
    # pthread is only available on Unix machines
    target_link_libraries(${BINARY_NAME}
      PRIVATE pthread)
  endif()

  target_compile_definitions(${BINARY_NAME}
    PRIVATE IGN_TRANSPORT_LOG_BUILD_PATH="$<TARGET_FILE_DIR:${BINARY_NAME}>")
endforeach()

# CLI test
if (HAVE_IGN_TOOLS)
  set(IGN_CONFIG_PATH "${CMAKE_BINARY_DIR}/log/test/lib/ruby/ignition")

  add_test(ign_log_record_no_overwrite
    ruby ${CMAKE_CURRENT_SOURCE_DIR}/ign_log_record_no_overwrite.rb
  )

  # Test disabled on Citadel
  #add_test(ign_log_record_force
  #  ruby ${CMAKE_CURRENT_SOURCE_DIR}/ign_log_record_force.rb
  #)

  set_tests_properties(
    ign_log_record_no_overwrite
    # Test disabled on Citadel
    # ign_log_record_force
    PROPERTIES
    ENVIRONMENT
      "IGN_CONFIG_PATH=${IGN_CONFIG_PATH};IGN_TRANSPORT_LOG_SQL_PATH=${PROJECT_SOURCE_DIR}/log/sql"
    TIMEOUT 10
  )
endif ()
