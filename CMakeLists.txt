cmake_minimum_required(VERSION 3.16)

project(N8ROHumanMotionController VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(N8RO_ROOT "C:/N8RO" CACHE PATH "N8RO installation root")

add_library(n8ro_human_motion SHARED
    src/n8ro_motion_controller.cpp
)

target_include_directories(n8ro_human_motion
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(n8ro_human_motion
    PRIVATE
        N8RO_MOTION_BUILD_DLL
)

add_executable(n8ro_motion_demo
    examples/demo.cpp
)

target_link_libraries(n8ro_motion_demo PRIVATE n8ro_human_motion)

add_library(n8ro_human_motion_plugin SHARED
    src/N8ROHumanMotionPlugin.cpp
)

target_include_directories(n8ro_human_motion_plugin
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${N8RO_ROOT}/include/astlib
        ${N8RO_ROOT}/include/astsim
        ${N8RO_ROOT}/include/astdb
        ${N8RO_ROOT}/include/n8ro-db
)

target_compile_definitions(n8ro_human_motion_plugin
    PRIVATE
        NDEBUG
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        ARKHEON_ASTLIB_EXPORTS
)

target_link_directories(n8ro_human_motion_plugin
    PRIVATE
        ${N8RO_ROOT}/lib
)

target_link_libraries(n8ro_human_motion_plugin
    PRIVATE
        arkheon-astlib
        arkheon-astdb
        n8ro-db
        arkheon-astsim
)

if(MSVC)
    target_compile_options(n8ro_human_motion PRIVATE /W4 /permissive-)
    target_compile_options(n8ro_motion_demo PRIVATE /W4 /permissive-)
    target_compile_options(n8ro_human_motion_plugin PRIVATE /W4 /permissive-)
else()
    target_compile_options(n8ro_human_motion PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(n8ro_motion_demo PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(n8ro_human_motion_plugin PRIVATE -Wall -Wextra -Wpedantic)
endif()
