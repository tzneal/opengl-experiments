project(glad)
set(SOURCE
        src/glad.c
        )

include_directories(${PROJECT_SOURCE_DIR}/include/)
add_library(${PROJECT_NAME} ${SOURCE})
target_link_libraries(${PROJECT_NAME} dl GL)
target_include_directories(${PROJECT_NAME} INTERFACE ${PROJECT_SOURCE_DIR}/include/)