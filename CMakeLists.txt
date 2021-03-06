set(CMAKE_VERSION 3.14)

cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(carpi)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(ENV{PKG_CONFIG_PATH} "${CMAKE_SOURCE_DIR}/pkgconfig:$ENV{PKG_CONFIG_PATH}")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_definitions("-Wno-psabi")
endif ()

find_package(PkgConfig)

add_subdirectory(third_party)

add_subdirectory(common_utils)
add_subdirectory(ipc_common)
add_subdirectory(bluetooth_utils)
add_subdirectory(video_stream)
add_subdirectory(wiring_utils)
add_subdirectory(data_server)
add_subdirectory(gps)
add_subdirectory(gps_broadcast)
add_subdirectory(net_utils)
add_subdirectory(distance_broadcast)

add_subdirectory(obd)

add_subdirectory(carpi_browser_subprocess)
add_subdirectory(carpi_master)
add_subdirectory(camera_slave)

add_subdirectory(spotify_stream)