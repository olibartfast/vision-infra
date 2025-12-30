# Utils module
add_library(vision_infra_utils STATIC
    VisionUtils.cpp
)

add_library(vision-infra::utils ALIAS vision_infra_utils)

target_include_directories(vision_infra_utils
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(vision_infra_utils
    PUBLIC
        ${OpenCV_LIBS}
    PRIVATE
        vision_infra_warnings
        $<$<BOOL:${ENABLE_SANITIZERS}>:vision_infra_sanitizers>
)

target_compile_features(vision_infra_utils PUBLIC cxx_std_20)

set_target_properties(vision_infra_utils PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    POSITION_INDEPENDENT_CODE ON
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)