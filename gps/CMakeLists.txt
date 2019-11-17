cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(gps)

pkg_search_module(LIBGPS REQUIRED gpsd)

add_library(gps_utils SHARED src/gps_listener.cpp)

target_include_directories(gps_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${LIBGPS_INCLUDE_DIRS})
target_link_libraries(gps_utils PUBLIC ${LIBGPS_LIBRARIES})