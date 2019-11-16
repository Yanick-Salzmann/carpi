cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(ipc_common)

add_library(ipc_common SHARED include/ipc_common/opcodes.hpp src/opcodes.cpp include/ipc_common/ipc_type.hpp src/ipc_type.cpp include/ipc_common/ipc_package.hpp src/ipc_package.cpp include/ipc_common/ipc_mq.hpp src/ipc_mq.cpp)

add_dependencies(ipc_common spdlog)

target_include_directories(ipc_common PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/common_utils/include
        ${CMAKE_BINARY_DIR}/third_party/spdlog/include
        )

target_link_libraries(ipc_common rt)