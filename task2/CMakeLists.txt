cmake_minimum_required(VERSION 3.15)

project("Task2")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/rapidjson/include/
)

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(task2 
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${SOURCES}
)