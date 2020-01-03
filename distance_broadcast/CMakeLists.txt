cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(distance_broadcast)

add_executable(distance_broadcast src/main.cpp)

find_package(Threads)

configure_file(resources/config.toml resources/config.toml)

add_dependencies(distance_broadcast common_utils net_utils wiring_utils bluetooth_utils toml ipc_common)

target_include_directories(distance_broadcast PUBLIC
        ${CMAKE_SOURCE_DIR}/common_utils/include
        ${CMAKE_SOURCE_DIR}/net_utils/include
        ${CMAKE_SOURCE_DIR}/wiring_utils/include
        ${CMAKE_SOURCE_DIR}/bluetooth_utils/include
        ${CMAKE_SOURCE_DIR}/ipc_common/include
        ${CMAKE_BINARY_DIR}/third_party/toml/include
)
target_link_libraries(distance_broadcast net_utils wiring_utils bluetooth_utils ipc_common Threads::Threads)