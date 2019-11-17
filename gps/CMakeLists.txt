cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(gps)

pkg_search_module(LIBGPS REQUIRED gpsd)

add_library(gps_utils SHARED src/gps_listener.cpp)
add_dependencies(gps_utils common_utils ipc_common)

target_include_directories(gps_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/common_utils/include ${CMAKE_SOURCE_DIR}/ipc_common/include ${LIBGPS_INCLUDE_DIRS})
target_link_libraries(gps_utils common_utils ipc_common ${LIBGPS_LIBRARIES})