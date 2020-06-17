project(nanovg)

set(SOURCE nanovg/src/nanovg.c)

include_directories(${PROJECT_SOURCE_DIR}/include/)
add_library(${PROJECT_NAME} ${SOURCE})
target_link_libraries(${PROJECT_NAME} dl GL)
add_definitions(-DNANOVG_GL3_IMPLEMENTATION)
target_compile_definitions(${PROJECT_NAME} PUBLIC -DNANOVG_GL3_IMPLEMENTATION)
target_include_directories(${PROJECT_NAME} INTERFACE ${PROJECT_SOURCE_DIR}/nanovg/src)