cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(gps_broadcast)

add_executable(gps_broadcast src/main.cpp src/net_broadcast.hpp src/net_broadcast.cpp)

add_dependencies(gps_broadcast gps_utils net_utils)

target_include_directories(gps_utils PUBLIC ${CMAKE_SOURCE_DIR}/gps/include ${CMAKE_SOURCE_DIR}/net_utils/include)
target_link_libraries(gps_broadcast gps_utils)