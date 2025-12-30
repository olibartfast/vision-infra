cmake_minimum_required(VERSION 3.25)

project(vision-infra 
    VERSION 1.0.0
    DESCRIPTION "C++ infrastructure library for computer vision and machine learning applications"
    LANGUAGES CXX
)

# Enforce modern CMake practices
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.")
endif()

# Build options
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_TESTING "Build tests" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)
option(ENABLE_SANITIZERS "Enable address and undefined behavior sanitizers" OFF)
option(ENABLE_WARNINGS "Enable compiler warnings" ON)

# Include helper modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Create interface library for compiler warnings
add_library(vision_infra_warnings INTERFACE)

if(ENABLE_WARNINGS)
    if(MSVC)
        target_compile_options(vision_infra_warnings INTERFACE 
            /W4 /WX /permissive-
            /w14640 # Enable warning on thread-safe static member initialization
        )
    else()
        target_compile_options(vision_infra_warnings INTERFACE
            -Wall -Wextra -Wpedantic -Werror
            -Wshadow -Wnon-virtual-dtor -Wold-style-cast
            -Wcast-align -Wunused -Woverloaded-virtual
            -Wconversion -Wsign-conversion -Wnull-dereference
            -Wdouble-promotion -Wformat=2 -Wimplicit-fallthrough
        )
        
        # GCC specific warnings
        if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            target_compile_options(vision_infra_warnings INTERFACE
                -Wmisleading-indentation -Wduplicated-cond -Wlogical-op
            )
        endif()
    endif()
endif()

# Create interface library for sanitizers  
add_library(vision_infra_sanitizers INTERFACE)

if(ENABLE_SANITIZERS AND NOT MSVC)
    target_compile_options(vision_infra_sanitizers INTERFACE
        -fsanitize=address,undefined -fno-omit-frame-pointer
    )
    target_link_options(vision_infra_sanitizers INTERFACE
        -fsanitize=address,undefined
    )
endif()

# Find required packages
find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs)

# Testing dependencies
if(BUILD_TESTING)
    find_package(GTest REQUIRED)
    
    # Handle different GTest/GMock target names across versions
    if(TARGET GTest::gmock)
        set(GMOCK_TARGET GTest::gmock)
        set(GMOCK_MAIN_TARGET GTest::gmock_main)
    elseif(TARGET gmock)
        set(GMOCK_TARGET gmock)
        set(GMOCK_MAIN_TARGET gmock_main)
    else()
        # Try to find GMock separately
        find_library(GMOCK_LIBRARY gmock)
        find_library(GMOCK_MAIN_LIBRARY gmock_main)
        if(GMOCK_LIBRARY AND GMOCK_MAIN_LIBRARY)
            add_library(gmock UNKNOWN IMPORTED)
            set_target_properties(gmock PROPERTIES IMPORTED_LOCATION ${GMOCK_LIBRARY})
            add_library(gmock_main UNKNOWN IMPORTED)
            set_target_properties(gmock_main PROPERTIES IMPORTED_LOCATION ${GMOCK_MAIN_LIBRARY})
            set(GMOCK_TARGET gmock)
            set(GMOCK_MAIN_TARGET gmock_main)
        else()
            message(WARNING "GMock not found. Some tests may not build properly.")
            set(GMOCK_TARGET "")
            set(GMOCK_MAIN_TARGET "")
        endif()
    endif()
    
    include(GoogleTest)
    enable_testing()
endif()

# Create modular library structure
add_subdirectory(src/config)
add_subdirectory(src/core) 
add_subdirectory(src/utils)

# Create main library target that aggregates all modules
add_library(${PROJECT_NAME} INTERFACE)
add_library(vision-infra::vision-infra ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} INTERFACE
    vision-infra::config
    vision-infra::core
    vision-infra::utils
)

# Main library properties
target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

# Export configuration
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# Install all module targets
install(TARGETS ${PROJECT_NAME} vision_infra_config vision_infra_core vision_infra_utils vision_infra_warnings vision_infra_sanitizers
    EXPORT vision-infra-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Export targets
install(EXPORT vision-infra-targets
    FILE vision-infra-targets.cmake
    NAMESPACE vision-infra::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vision-infra
)

# Create config file
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/vision-infra-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/vision-infra-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vision-infra
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/vision-infra-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/vision-infra-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/vision-infra-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vision-infra
)

# Optional components
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()