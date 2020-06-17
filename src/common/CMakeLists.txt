project(common)

include_directories(${THIRDPARTY})

file(GLOB TEST_FILES *_test.cpp)
file(GLOB SOURCE_FILES colormap.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

# libuuid

enable_testing()
add_executable(${PROJECT_NAME}_test ${TEST_FILES} ${THIRDPARTY}/catch2/main.cpp)
target_link_libraries(${PROJECT_NAME} spdlog glm)
target_link_libraries(${PROJECT_NAME}_test common)
add_test(NAME ${PROJECT_NAME}_test COMMAND ${PROJECT_NAME}_test WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
# add_dependencies(check ${PROJECT_NAME}_test)
