cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(bluetooth_utils)

add_library(bluetooth_utils SHARED src/bluetooth_manager.cpp include/bluetooth_utils/bluetooth_device.hpp src/bluetooth_device.cpp include/bluetooth_utils/bluetooth_server.hpp include/bluetooth_utils/bluetooth_connection.hpp src/bluetooth_server.cpp src/bluetooth_connection.cpp)
add_dependencies(bluetooth_utils common_utils)

target_include_directories(bluetooth_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${SPDLOG_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/common_utils/include)

target_link_libraries(bluetooth_utils -lbluetooth common_utils)