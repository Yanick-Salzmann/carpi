#ifndef CARPI_IPC_COMMON_OPCODES_HPP
#define CARPI_IPC_COMMON_OPCODES_HPP

#include <cstdint>

namespace carpi::ipc {
    enum class Opcodes : uint32_t {
        MSG_IPC_TYPE = 0,
        MAX_KNOWN_OPCODE = 0
    };
}

#endif //CARPI_IPC_COMMON_OPCODES_HPP
