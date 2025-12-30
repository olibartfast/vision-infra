# Examples for vision-infra library
cmake_minimum_required(VERSION 3.25)

# Add example subdirectories
add_subdirectory(basic_config)
add_subdirectory(logging_demo)
add_subdirectory(image_processing)
add_subdirectory(file_operations)

# Create a convenience target to build all examples
add_custom_target(examples
    DEPENDS 
        basic_config_example 
        logging_demo_example 
        image_processing_example 
        file_operations_example
    COMMENT "Building all examples"
)