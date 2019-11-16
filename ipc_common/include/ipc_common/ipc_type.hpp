#ifndef CARPI_IPC_COMMON_IPCTYPE_HPP
#define CARPI_IPC_COMMON_IPCTYPE_HPP

#include <cstdint>

namespace carpi::ipc {
    enum class IpcType : uint32_t {
        CAMERA_SOURCE = 0,
        FIRST_UNKNOWN_IPC_TYPE = 1
    };
}

#endif //CARPI_IPC_COMMON_IPCTYPE_HPP
