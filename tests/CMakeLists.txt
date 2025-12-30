# Test configuration for vision-infra
cmake_minimum_required(VERSION 3.25)

# Ensure we have access to GTest and GMock
if(NOT TARGET GTest::gtest)
    message(FATAL_ERROR "GTest not found. Please install Google Test or set BUILD_TESTING=OFF")
endif()

# Include test subdirectories
add_subdirectory(unit)
add_subdirectory(e2e)

# Create a convenience target to run all tests
add_custom_target(run_all_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS unit_tests e2e_tests
    COMMENT "Running all tests"
)