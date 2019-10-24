cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(ipc_common)

add_library(ipc_common SHARED include/ipc_common/Opcodes.hpp src/Opcodes.cpp include/ipc_common/IpcType.hpp src/IpcType.cpp include/ipc_common/IpcPackage.hpp src/IpcPackage.cpp)

add_dependencies(ipc_common spdlog)

target_include_directories(ipc_common PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/common_utils/include
        ${CMAKE_BINARY_DIR}/third_party/spdlog/include
        )