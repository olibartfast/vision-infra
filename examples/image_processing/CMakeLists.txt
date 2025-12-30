# Image Processing Example
set(EXAMPLE_NAME image_processing_example)

add_executable(${EXAMPLE_NAME} main.cpp)

target_link_libraries(${EXAMPLE_NAME}
    PRIVATE
        vision-infra::vision-infra
        ${OpenCV_LIBS}
)

# Set C++ standard and compile options
target_compile_features(${EXAMPLE_NAME} PRIVATE cxx_std_20)

# Link warning and sanitizer targets if available
if(TARGET vision_infra_warnings)
    target_link_libraries(${EXAMPLE_NAME} PRIVATE vision_infra_warnings)
endif()

if(TARGET vision_infra_sanitizers)
    target_link_libraries(${EXAMPLE_NAME} PRIVATE vision_infra_sanitizers)
endif()

# Include directories
target_include_directories(${EXAMPLE_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${OpenCV_INCLUDE_DIRS}
)