# Unit tests for vision-infra
set(UNIT_TEST_TARGET unit_tests)

# Find all unit test source files
file(GLOB_RECURSE UNIT_TEST_SOURCES 
    "test_*.cpp"
    "*_test.cpp"
)

# Create unit test executable
add_executable(${UNIT_TEST_TARGET} ${UNIT_TEST_SOURCES})

# Link required libraries
target_link_libraries(${UNIT_TEST_TARGET}
    PRIVATE
        vision-infra::vision-infra
        GTest::gtest
        GTest::gtest_main
)

# Link GMock if available
if(GMOCK_TARGET)
    target_link_libraries(${UNIT_TEST_TARGET} PRIVATE ${GMOCK_TARGET})
endif()

if(GMOCK_MAIN_TARGET)
    target_link_libraries(${UNIT_TEST_TARGET} PRIVATE ${GMOCK_MAIN_TARGET})
endif()

# Set C++ standard and compile options
target_compile_features(${UNIT_TEST_TARGET} PRIVATE cxx_std_20)

# Link warning and sanitizer targets if available
if(TARGET vision_infra_warnings)
    target_link_libraries(${UNIT_TEST_TARGET} PRIVATE vision_infra_warnings)
endif()

if(TARGET vision_infra_sanitizers)
    target_link_libraries(${UNIT_TEST_TARGET} PRIVATE vision_infra_sanitizers)
endif()

# Include directories
target_include_directories(${UNIT_TEST_TARGET}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
)

# Discover tests for CTest
gtest_discover_tests(${UNIT_TEST_TARGET}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# Add custom target for running unit tests only
add_custom_target(run_unit_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose -R "unit"
    DEPENDS ${UNIT_TEST_TARGET}
    COMMENT "Running unit tests"
)