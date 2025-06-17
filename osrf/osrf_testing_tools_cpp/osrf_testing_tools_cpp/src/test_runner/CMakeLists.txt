add_executable(test_runner main.cpp)

install(TARGETS test_runner
  EXPORT test_runner
  DESTINATION lib/${PROJECT_NAME}
)

install(EXPORT test_runner
  DESTINATION share/${PROJECT_NAME}/cmake
  NAMESPACE "${PROJECT_NAME}::"
  FILE "test_runnerExport.cmake"
)
