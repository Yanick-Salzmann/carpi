cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(carpi_master)

add_executable(carpi_master src/main.cpp src/comm/CommServer.hpp src/comm/CommServer.cpp)

add_dependencies(carpi_master bluetooth_utils video_stream obd)

find_package(Threads)

target_include_directories(carpi_master PUBLIC
        ${CMAKE_SOURCE_DIR}/bluetooth_utils/include
        ${CMAKE_SOURCE_DIR}/video_stream/include
        ${CMAKE_SOURCE_DIR}/obd/include
)

target_link_libraries(carpi_master bluetooth_utils video_stream obd Threads::Threads bcm_host)