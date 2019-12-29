cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(spotify_stream)

set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(spotify_stream src/main.cpp)

add_dependencies(spotify_stream net_utils)

target_include_directories(spotify_stream PUBLIC ${CMAKE_SOURCE_DIR}/net_utils/include)
target_link_libraries(spotify_stream net_utils)