cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(gps_broadcast)

configure_file(resources/config.toml resources/config.toml)

add_executable(gps_broadcast src/main.cpp src/net_broadcast.hpp src/net_broadcast.cpp src/bluetooth_broadcast.hpp src/bluetooth_broadcast.cpp)

add_dependencies(gps_broadcast gps_utils net_utils toml)

target_include_directories(gps_utils PUBLIC ${CMAKE_SOURCE_DIR}/gps/include ${CMAKE_SOURCE_DIR}/net_utils/include ${CMAKE_BINARY_DIR}/third_party/toml/include)
target_link_libraries(gps_broadcast gps_utils net_utils)