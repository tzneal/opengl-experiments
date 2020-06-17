project(boids)

include_directories(${THIRDPARTY})
include_directories(${ROOTSRC})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

set(CPP_FILES main.cpp
        systems/backgroundrenderer.cpp systems/boidrenderer.cpp systems/boidrenderer.h)
file(GLOB_RECURSE TEST_FILES *_test.cpp)

add_executable(${PROJECT_NAME} ${CPP_FILES} ${version_file})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} common engine spdlog c++fs)