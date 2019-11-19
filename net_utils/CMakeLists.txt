cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(net_utils)

add_library(net_utils SHARED src/iface_utils.cpp include/net_utils/udp_broadcast.hpp src/udp_broadcast.cpp include/net_utils/udp_multicast.hpp src/udp_multicast.cpp)

add_dependencies(net_utils common_utils)

target_include_directories(net_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/common_utils/include)

target_link_libraries(net_utils common_utils)