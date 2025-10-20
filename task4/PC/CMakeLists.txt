cmake_minimum_required(VERSION 3.15)
project(Task4PC CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(task4
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

add_library(serial_lib STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/serial/src/serial.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/serial/src/impl/win.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/serial/src/impl/list_ports/list_ports_win.cc
)

target_include_directories(serial_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/serial/include
)

target_include_directories(task4 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(task4 PRIVATE serial_lib)

if (WIN32)
    target_link_libraries(task4 PRIVATE ws2_32)
endif()
