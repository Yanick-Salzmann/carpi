cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(bluetooth_utils)

add_library(bluetooth_utils SHARED src/BluetoothManager.cpp include/bluetooth_utils/BluetoothDevice.hpp src/BluetoothDevice.cpp include/bluetooth_utils/BluetoothServer.hpp include/bluetooth_utils/BluetoothConnection.hpp src/BluetoothServer.cpp src/BluetoothConnection.cpp ../data_server/src/http_server.cpp)
add_dependencies(bluetooth_utils common_utils)

target_include_directories(bluetooth_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${SPDLOG_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/common_utils/include)

target_link_libraries(bluetooth_utils -lbluetooth common_utils)