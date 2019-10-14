cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(carpi_master)

add_executable(carpi_master src/main.cpp)

add_dependencies(carpi_master bluetooth_utils)

target_include_directories(carpi_master PUBLIC ${CMAKE_SOURCE_DIR}/bluetooth_utils/include)

target_link_libraries(carpi_master bluetooth_utils)