add_subdirectory(integration)

configure_file (test_config.h.in ${PROJECT_BINARY_DIR}/log/include/ignition/transport/log/test_config.h)
configure_file (test_config.h.in ${PROJECT_BINARY_DIR}/log/include/gz/transport/log/test_config.h)
