cmake_minimum_required(VERSION 3.6)

message("${PROJECT_SOURCE_DIR}")

set(PROJECT_NAME "Graph")
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfatal-errors")

set(EXT_PROJECTS_DIR ${CMAKE_SOURCE_DIR}/external)

include_directories(
        ${COMMON_INCLUDES}
        ${EXT_PROJECTS_DIR}/include
        ${CMAKE_SOURCE_DIR}/include
)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/tests)
set(SRC_FILES
    ${SRC_DIR}/test_base.cpp
    ${SRC_DIR}/basic_operations.cpp
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

enable_testing(true)
add_test(test1 ${PROJECT_NAME})
