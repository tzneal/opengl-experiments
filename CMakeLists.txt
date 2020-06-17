cmake_minimum_required(VERSION 3.12)

project(SDL2Test
        VERSION 1.0
        LANGUAGES CXX
        )

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)

add_compile_options("-Wall")

SET(THIRDPARTY ${CMAKE_SOURCE_DIR}/src/thirdparty)
SET(ROOTSRC ${CMAKE_SOURCE_DIR}/src)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)


# extract version info
execute_process(
        COMMAND git describe --tags --dirty
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        OUTPUT_VARIABLE VERSION_LONG
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(
        COMMAND git log -1 --format=%ad --date=local
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE VERSION_DATE
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)


configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/version/version.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/version.cpp)
set(version_file "${CMAKE_CURRENT_BINARY_DIR}/version.cpp")


add_subdirectory(${THIRDPARTY}/spdlog)
add_subdirectory(${THIRDPARTY}/imgui)
add_subdirectory(${THIRDPARTY}/glad)
add_subdirectory(${THIRDPARTY}/glm)
add_subdirectory(${THIRDPARTY}/stb)
add_subdirectory(${THIRDPARTY}/nanovg)

add_subdirectory(src/common)
add_subdirectory(src/engine)
add_subdirectory(src/simulations/boids)
