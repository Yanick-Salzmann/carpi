cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(camera_slave)

add_executable(camera_slave src/main.cpp)

add_dependencies(camera_slave bluetooth_utils)

target_include_directories(camera_slave PUBLIC ${CMAKE_SOURCE_DIR}/bluetooth_utils/include)

target_link_libraries(camera_slave bluetooth_utils)