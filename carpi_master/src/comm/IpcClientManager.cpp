#include "IpcClientManager.hpp"
#include "../../../ipc_common/include/ipc_common/IpcPackage.hpp"
#include "../../../ipc_common/include/ipc_common/IpcType.hpp"

namespace carpi::comm {
    LOGGER_IMPL(IpcClientManager);

    void IpcClientManager::accept_connection(int connection, sockaddr_un addr) {
        fd_set socket_set{};
        FD_SET(connection, &socket_set);
        struct timeval read_timeout{
            .tv_usec = 100000,
            .tv_sec = 0
        };

        const auto ret = select(connection, &socket_set, nullptr, nullptr, &read_timeout);
        if(ret <= 0) {
            log->warn("Discarding connection from {} because there was no response in 100 milliseconds", addr.sun_path);
            return;
        }

        auto init_package = ipc::IpcPackage::read_from_socket(connection);
        if(init_package.opcode() != ipc::Opcodes::MSG_IPC_TYPE) {
            log->warn("First package from IPC client must be of type MSG_IPC_TYPE(=0) but was {}", static_cast<uint32_t>(init_package.opcode()));
            return;
        }

        ipc::IpcType type;
        init_package >> type;

        if(type != ipc::IpcType::CAMERA_SOURCE) {
            log->warn("Invalid IPC init packet received");
            return;
        }
    }
}