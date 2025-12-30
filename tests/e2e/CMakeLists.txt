# End-to-end tests for vision-infra
set(E2E_TEST_TARGET e2e_tests)

# Find all e2e test source files
file(GLOB_RECURSE E2E_TEST_SOURCES 
    "test_*.cpp"
    "*_test.cpp"
)

# Only create the target if there are test files
if(E2E_TEST_SOURCES)
    # Create e2e test executable
    add_executable(${E2E_TEST_TARGET} ${E2E_TEST_SOURCES})

    # Link required libraries
    target_link_libraries(${E2E_TEST_TARGET}
        PRIVATE
            vision-infra::vision-infra
            GTest::gtest
            GTest::gtest_main
    )

    # Link GMock if available
    if(GMOCK_TARGET)
        target_link_libraries(${E2E_TEST_TARGET} PRIVATE ${GMOCK_TARGET})
    endif()

    if(GMOCK_MAIN_TARGET)
        target_link_libraries(${E2E_TEST_TARGET} PRIVATE ${GMOCK_MAIN_TARGET})
    endif()

    # Set C++ standard and compile options
    target_compile_features(${E2E_TEST_TARGET} PRIVATE cxx_std_20)

    # Link warning and sanitizer targets if available
    if(TARGET vision_infra_warnings)
        target_link_libraries(${E2E_TEST_TARGET} PRIVATE vision_infra_warnings)
    endif()

    if(TARGET vision_infra_sanitizers)
        target_link_libraries(${E2E_TEST_TARGET} PRIVATE vision_infra_sanitizers)
    endif()

    # Include directories
    target_include_directories(${E2E_TEST_TARGET}
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_SOURCE_DIR}/include
    )

    # Discover tests for CTest
    gtest_discover_tests(${E2E_TEST_TARGET}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

    # Add custom target for running e2e tests only
    add_custom_target(run_e2e_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --verbose -R "e2e"
        DEPENDS ${E2E_TEST_TARGET}
        COMMENT "Running end-to-end tests"
    )
else()
    # Create a dummy target if no test files exist
    add_custom_target(${E2E_TEST_TARGET}
        COMMAND ${CMAKE_COMMAND} -E echo "No e2e tests found"
        COMMENT "No e2e tests to run"
    )
    
    add_custom_target(run_e2e_tests
        COMMAND ${CMAKE_COMMAND} -E echo "No e2e tests found"
        COMMENT "No e2e tests to run"
    )
endif()