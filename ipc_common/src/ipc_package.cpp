#include "ipc_common/ipc_package.hpp"
#include <common_utils/network.hpp>
#include <mqueue.h>
#include <common_utils/error.hpp>
#include <ipc_common/ipc_mq.hpp>

namespace carpi::ipc {
    LOGGER_IMPL(IpcPackage);

    IpcPackage IpcPackage::read_from_socket(int socket) {
        const auto size = utils::read<uint32_t>(socket);
        if (size < 4) {
            log->error("Received invalid IPC package. Size < 4");
            throw std::runtime_error{"Invalid IPC package"};
        }

        const auto opcode = utils::read<Opcodes>(socket);
        if (opcode >= Opcodes::FIRST_UNKNOWN_OPCODE) {
            log->error("Invalid IPC opcode received: {}", static_cast<uint32_t>(opcode));
            throw std::runtime_error{"Invalid IPC package"};
        }

        std::vector<uint8_t> content(size - 4);
        if (size > 4) {
            utils::read(socket, content.data(), content.size());
        }

        return IpcPackage{opcode, content};
    }

    void IpcPackage::read(void *buffer, std::size_t size) {
        if (_read_pos + size > _data.size()) {
            log->error("Attempted to read past the end of an IPC package: {} + {} > {}", _read_pos, size, _data.size());
            throw std::out_of_range{"Attempted to read past the end of a package"};
        }

        memcpy(buffer, &_data[_read_pos], size);
        _read_pos += size;
    }

    IpcPackage IpcPackage::read_from_mq(int mq) {
        std::array<std::uint8_t, MessageQueue::MAX_MESSAGE_SIZE> message_data{};
        const auto msg_size = mq_receive(mq, (char *) message_data.data(), MessageQueue::MAX_MESSAGE_SIZE, nullptr);
        if (msg_size < 0) {
            log->warn("Error receiving MQ message: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error receiving MQ message"};
        }

        if (msg_size < 4) {
            log->error("Received invalid MQ message. Size < 4");
            throw std::runtime_error{"Invalid MQ message"};
        }

        const auto opcode = (Opcodes) *(uint32_t *) message_data.data();
        std::vector<uint8_t> actual_data{message_data.begin() + 4, message_data.begin() + msg_size - 4};

        return IpcPackage{opcode, actual_data};

    }

    void IpcPackage::write(const void *buffer, std::size_t size) {
        const auto num_bytes = _write_pos + size;
        if(_data.size() < num_bytes) {
            _data.resize(num_bytes);
        }

        memcpy(_data.data() + _write_pos, buffer, size);
        _write_pos += size;
    }

    void IpcPackage::send_to_mq(int mq) const {
        std::vector<uint8_t> full_data(4 + _data.size());
        *(uint32_t *) full_data.data() = (uint32_t) _opcode;
        memcpy(full_data.data() + 4, _data.data(), _data.size());
        if (mq_send(mq, (const char *) full_data.data(), full_data.size(), 1) < 0) {
            log->error("Error sending MQ message: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error sending MQ message"};
        }
    }

    void IpcPackage::to_buffer(std::vector<uint8_t> &buffer) const {
        buffer.resize(size());
        *(uint32_t*) buffer.data() = size() - 4;
        *(uint32_t*)(buffer.data() + 4) = (uint32_t) _opcode;
        memcpy(buffer.data() + 8, _data.data(), _data.size());
    }
}