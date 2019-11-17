cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(gps_broadcast)

add_executable(gps_broadcast src/main.cpp)

add_dependencies(gps_broadcast gps_utils)

target_include_directories(gps_utils PUBLIC ${CMAKE_SOURCE_DIR}/gps/include)
target_link_libraries(gps_broadcast gps_utils)