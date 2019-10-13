set(CMAKE_VERSION 3.15)

cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(carpi)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(third_party)
add_subdirectory(common_utils)
add_subdirectory(bluetooth_utils)

add_subdirectory(camera_slave)