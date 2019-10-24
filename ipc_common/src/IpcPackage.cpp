#include "ipc_common/IpcPackage.hpp"
#include <common_utils/network.hpp>

namespace carpi::ipc {
    LOGGER_IMPL(IpcPackage);

    IpcPackage IpcPackage::read_from_socket(int socket) {
        const auto size = utils::read<uint32_t>(socket);
        if(size < 4) {
            log->error("Received invalid IPC package. Size < 4");
            throw std::runtime_error{"Invalid IPC package"};
        }

        const auto opcode = utils::read<Opcodes>(socket);
        if(opcode > Opcodes::MAX_KNOWN_OPCODE) {
            log->error("Invalid IPC opcode received: {}", static_cast<uint32_t>(opcode));
            throw std::runtime_error{"Invalid IPC package"};
        }

        std::vector<uint8_t> content(size - 4);
        if(size > 4) {
            utils::read(socket, content.data(), content.size());
        }

        return IpcPackage{opcode, content};
    }

    void IpcPackage::read(void *buffer, std::size_t size) {
        if(_read_pos + size > _data.size()) {
            log->error("Attempted to read past the end of an IPC package: {} + {} > {}", _read_pos, size, _data.size());
            throw std::out_of_range{"Attempted to read past the end of a package"};
        }

        memcpy(buffer, &_data[_read_pos], size);
        _read_pos += size;
    }
}