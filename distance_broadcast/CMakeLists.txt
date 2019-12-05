cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(distance_broadcast)

add_executable(distance_broadcast src/main.cpp)

find_package(Threads)

add_dependencies(distance_broadcast common_utils net_utils wiring_utils)

target_include_directories(distance_broadcast PUBLIC ${CMAKE_SOURCE_DIR}/common_utils/include ${CMAKE_SOURCE_DIR}/net_utils/include ${CMAKE_SOURCE_DIR}/wiring_utils/include)
target_link_libraries(distance_broadcast common_utils net_utils wiring_utils Threads::Threads)