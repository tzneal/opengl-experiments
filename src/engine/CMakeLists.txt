project(engine)

include_directories(${THIRDPARTY})
include_directories(${ROOTSRC})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${THIRDPARTY}/entt/single_include/)

find_package(SDL2 REQUIRED)
find_library(SDL2_image NAMES SDL2_image)
include_directories(${SDL2_INCLUDE_DIRS})

set(CPP_FILES
        engine.cpp
        assets.cpp
        shader/program.cpp
        shader/glerror.cpp
        shader/texture.cpp
        renderers/spriterenderer.cpp
        renderers/particlerenderer.cpp
        systems/particlesystem.cpp systems/particlesystem.h ../common/colormap.cpp ../common/colormap.h systems/system.cpp)
file(GLOB_RECURSE TEST_FILES *_test.cpp)

add_library(${PROJECT_NAME} ${CPP_FILES})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2_image} glad common spdlog imgui nanovg stb stdc++fs)
target_include_directories(${PROJECT_NAME} PUBLIC ${THIRDPARTY}/entt/single_include/)
