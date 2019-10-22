set(CMAKE_VERSION 3.14)

cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(carpi)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(ENV{PKG_CONFIG_PATH} "${CMAKE_SOURCE_DIR}/pkgconfig:$ENV{PKG_CONFIG_PATH}")

add_definitions("-Wno-psabi")

find_package(PkgConfig)

add_subdirectory(third_party)

add_subdirectory(common_utils)
add_subdirectory(bluetooth_utils)
add_subdirectory(video_stream)

add_subdirectory(obd)

add_subdirectory(carpi_browser_subprocess)
add_subdirectory(carpi_master)
add_subdirectory(camera_slave)