set(TEST_TYPE "REGRESSION")

ign_get_sources(tests)

ign_build_tests(TYPE REGRESSION SOURCES ${tests}
  LIB_DEPS ${EXTRA_TEST_LIB_DEPS})
