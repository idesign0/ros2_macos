list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Catch2 3.6.0 REQUIRED)
find_package(range-v3 REQUIRED)

# Test library
add_executable(test-rsl
    algorithm.cpp
    monad.cpp
    no_discard.cpp
    overload.cpp
    parameter_validators.cpp
    queue.cpp
    random.cpp
    static_string.cpp
    static_vector.cpp
    strong_type.cpp)
if(CMAKE_CXX_COMPILER_ID MATCHES "(GNU|Clang)")
    target_sources(test-rsl PRIVATE try.cpp) # Requires GCC extensions
endif()
target_link_libraries(test-rsl PRIVATE
    rsl::rsl
    Catch2::Catch2WithMain
    range-v3::range-v3
)
catch_discover_tests(test-rsl)

# Test install interface
add_test(NAME "Install RSL" COMMAND
    ${CMAKE_COMMAND}
    --install ${PROJECT_BINARY_DIR}
    --prefix ${CMAKE_CURRENT_BINARY_DIR}/install/RSL
    --config $<CONFIG>)
set_tests_properties("Install RSL" PROPERTIES FIXTURES_SETUP install-rsl)

add_test(NAME "Configure installation test" COMMAND
    ${CMAKE_COMMAND}
    -S ${CMAKE_CURRENT_SOURCE_DIR}/install
    -B ${CMAKE_CURRENT_BINARY_DIR}/install/build
    -G ${CMAKE_GENERATOR}
    -DCMAKE_BUILD_TYPE=$<CONFIG>
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -Drsl_ROOT=${CMAKE_CURRENT_BINARY_DIR}/install/RSL)
set_tests_properties("Configure installation test" PROPERTIES FIXTURES_SETUP configure-test-project FIXTURES_REQUIRED install-rsl)

add_test(NAME "Build installation test" COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR}/install/build --config $<CONFIG>)
set_tests_properties("Build installation test" PROPERTIES FIXTURES_REQUIRED configure-test-project)

add_test(NAME "Delete installation" COMMAND ${CMAKE_COMMAND} -E rm -r ${CMAKE_CURRENT_BINARY_DIR}/install/RSL ${CMAKE_CURRENT_BINARY_DIR}/install/build)
set_tests_properties("Delete installation" PROPERTIES FIXTURES_CLEANUP "install-rsl;configure-test-project")

# Build install test project
add_subdirectory(install)
