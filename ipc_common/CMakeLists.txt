cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(ipc_common)

add_library(ipc_common SHARED include/ipc_common/opcodes.hpp src/opcodes.cpp include/ipc_common/ipc_type.hpp src/ipc_type.cpp include/ipc_common/ipc_package.hpp src/ipc_package.cpp include/ipc_common/ipc_mq.hpp src/ipc_mq.cpp include/ipc_common/net_broadcast.hpp src/net_broadcast.cpp include/ipc_common/bluetooth_broadcast.hpp src/bluetooth_broadcast.cpp)

add_dependencies(ipc_common spdlog common_utils bluetooth_utils net_utils)

target_include_directories(ipc_common PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/common_utils/include
        ${CMAKE_BINARY_DIR}/third_party/spdlog/include
        ${CMAKE_SOURCE_DIR}/net_utils/include
        ${CMAKE_SOURCE_DIR}/bluetooth_utils/include
        )

target_link_libraries(ipc_common net_utils bluetooth_utils rt)