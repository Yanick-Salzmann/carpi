#ifndef CARPI_IPC_COMMON_OPCODES_HPP
#define CARPI_IPC_COMMON_OPCODES_HPP

#include <cstdint>

namespace carpi::ipc {
    enum class Opcodes : uint32_t {
        MSG_IPC_TYPE = 0,
        MSG_OBD_BLUETOOTH_CONNECT = 1,
        MSG_OBD_FETCH_RPM = 2,
        MSG_OBD_FETCH_SPEED = 3,
        MSG_GPS_FETCH = 4,
        MSG_GPS_UPDATE = 5,
        MSG_JSNSR04T_UPDATE = 6,
        FIRST_UNKNOWN_OPCODE
    };
}

#endif //CARPI_IPC_COMMON_OPCODES_HPP
