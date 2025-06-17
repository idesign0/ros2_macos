set(TEST_TYPE "INTEGRATION")

set(tests
  authPubSub.cc
  scopedTopic.cc
  statistics.cc
  twoProcsPubSub.cc
  twoProcsSrvCall.cc
  twoProcsSrvCallStress.cc
  twoProcsSrvCallSync1.cc
  twoProcsSrvCallWithoutInput.cc
  twoProcsSrvCallWithoutInputStress.cc
  twoProcsSrvCallWithoutInputSync1.cc
  twoProcsSrvCallWithoutOutput.cc
  twoProcsSrvCallWithoutOutputStress.cc
)

# Test symbols having the right name on linux only.
if (UNIX AND NOT APPLE)
  configure_file(all_symbols_have_version.bash.in
    ${CMAKE_CURRENT_BINARY_DIR}/all_symbols_have_version.bash @ONLY)
  add_test(NAME INTEGRATION_versioned_symbols
    COMMAND bash ${CMAKE_CURRENT_BINARY_DIR}/all_symbols_have_version.bash $<TARGET_FILE:${PROJECT_LIBRARY_TARGET_NAME}>)
endif()

ign_build_tests(TYPE INTEGRATION SOURCES ${tests}
  TEST_LIST test_list
  LIB_DEPS ${EXTRA_TEST_LIB_DEPS})

foreach(test ${test_list})

  # Inform each test of its output directory so it knows where to call the
  # auxiliary files from. Using a generator expression here is useful for
  # multi-configuration generators, like Visual Studio.
  target_compile_definitions(${test} PRIVATE
    "DETAIL_IGN_TRANSPORT_TEST_DIR=\"$<TARGET_FILE_DIR:${test}>\"")

endforeach()

foreach(test ${test_list})
  set(_env_vars)
  list(APPEND _env_vars "IGN_IP=127.0.0.1")
  set_property(TEST ${test} PROPERTY ENVIRONMENT "${_env_vars}")
endforeach()

set(auxiliary_files
  authPubSubSubscriberInvalid_aux
  fastPub_aux
  pub_aux
  pub_aux_throttled
  scopedTopicSubscriber_aux
  twoProcsPublisher_aux
  twoProcsPubSubSingleSubscriber_aux
  twoProcsPubSubSubscriber_aux
  twoProcsSrvCallReplier_aux
  twoProcsSrvCallReplierInc_aux
  twoProcsSrvCallWithoutInputReplier_aux
  twoProcsSrvCallWithoutInputReplierInc_aux
  twoProcsSrvCallWithoutOutputReplier_aux
  twoProcsSrvCallWithoutOutputReplierInc_aux
)

# Build the auxiliary files.
foreach(AUX_EXECUTABLE ${auxiliary_files})
  ign_add_executable(INTEGRATION_${AUX_EXECUTABLE} ${AUX_EXECUTABLE}.cc)

  # Link the libraries that we always need.
  target_link_libraries(INTEGRATION_${AUX_EXECUTABLE}
    PRIVATE
      ${PROJECT_LIBRARY_TARGET_NAME}
      gtest
      ${EXTRA_TEST_LIB_DEPS}
  )

  if(UNIX)
    # pthread is only available on Unix machines
    target_link_libraries(INTEGRATION_${AUX_EXECUTABLE}
      PRIVATE pthread)
  endif()

endforeach(AUX_EXECUTABLE)
