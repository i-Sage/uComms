# Enable testing
enable_testing()

# Create the test executable
add_executable(parse_tests parse_tests.c)

# Link against the header-only library
target_link_libraries(parse_tests PRIVATE uComms::Headers)

# Optional: Add compiler flags for better debugging/testing
target_compile_options(parse_tests PRIVATE
    $<$<COMPILE_LANGUAGE:C>:-Wall -Wextra -g -O0>
)

# Optional: Add test-specific definitions
target_compile_definitions(parse_tests PRIVATE
    TESTING=1
)

# Register the test with CTest
add_test(NAME ParseFunctionTests COMMAND parse_tests)

# Optional: Set test properties
set_tests_properties(ParseFunctionTests PROPERTIES
    TIMEOUT 30
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Optional: Add a custom target to run tests easily
add_custom_target(run_parse_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS parse_tests
    COMMENT "Running parse function tests"
)

# Optional: If you want to use a testing framework like Unity or just simple asserts
# You can add additional test executables here:
# add_executable(integration_tests integration_tests.c)
# target_link_libraries(integration_tests PRIVATE uComms::Headers)
# add_test(NAME IntegrationTests COMMAND integration_tests)
