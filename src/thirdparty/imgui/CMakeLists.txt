project(imgui)

set(SOURCE imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_widgets.cpp
        imgui/examples/imgui_impl_sdl.cpp
        imgui/examples/imgui_impl_opengl3.cpp
        )

find_package(SDL2 REQUIRED)
find_library(SDL2_image NAMES SDL2_image)
include_directories(${PROJECT_SOURCE_DIR}/imgui ${SDL2_INCLUDE_DIRS})
add_library(${PROJECT_NAME} ${SOURCE})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} glad dl GL)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/imgui)