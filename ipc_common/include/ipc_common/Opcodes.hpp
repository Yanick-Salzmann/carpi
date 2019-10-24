#ifndef CARPI_IPC_COMMON_OPCODES_HPP
#define CARPI_IPC_COMMON_OPCODES_HPP

#include <cstdint>

namespace carpi::ipc {
    enum class Opcodes : uint32_t {
        MSG_IPC_TYPE = 0,
        FIRST_UNKNOWN_OPCODE
    };
}

#endif //CARPI_IPC_COMMON_OPCODES_HPP
