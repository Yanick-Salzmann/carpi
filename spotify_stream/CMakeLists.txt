cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(spotify_stream)

set(CMAKE_VERBOSE_MAKEFILE ON)

if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/resources/config.toml)
    configure_file(resources/config.toml resources/config.toml)
endif ()

add_executable(spotify_stream src/main.cpp src/websocket_interface.hpp src/websocket_interface.cpp src/oauth/refresh_flow.hpp src/oauth/refresh_flow.cpp)

add_dependencies(spotify_stream net_utils toml)

target_include_directories(
        spotify_stream PUBLIC
        ${CMAKE_SOURCE_DIR}/net_utils/include
        ${CMAKE_BINARY_DIR}/third_party/toml/include
)
target_link_libraries(spotify_stream net_utils)