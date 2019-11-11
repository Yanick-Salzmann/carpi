#include "ipc_client_manager.hpp"
#include "ipc_common/IpcPackage.hpp"
#include "ipc_common/IpcType.hpp"
#include "ipc_connection.hpp"

namespace carpi::comm {
    LOGGER_IMPL(IpcClientManager);

    void IpcClientManager::accept_connection(int connection, sockaddr_un addr) {
        ipc::IpcType type;
        if(!read_type_from_socket(connection, type)) {
            // read_type_from_socket is supposed to print the information
            return;
        }

        _connections.emplace(type, std::make_shared<IpcConnection>());
    }

    void IpcClientManager::accept_bluetooth(std::shared_ptr<bluetooth::BluetoothConnection> connection) {
        ipc::IpcType type;
        if(!read_type_from_socket(connection->fd(), type)) {
            // read_type_from_socket is supposed to print enough information
            return;
        }

        _connections.emplace(type, std::make_shared<IpcConnection>());
    }

    bool IpcClientManager::read_type_from_socket(int socket, ipc::IpcType &out_type) {
        fd_set socket_set{};
        FD_SET(socket, &socket_set);
        struct timeval read_timeout{
                .tv_sec = 0,
                .tv_usec = 100000
        };

        const auto ret = select(socket + 1, &socket_set, nullptr, nullptr, &read_timeout);
        if(ret <= 0) {
            sockaddr addr{};
            socklen_t sock_len;
            getsockname(socket, &addr, &sock_len);
            std::string name{};
            if(addr.sa_family == AF_BLUETOOTH) {
                char bt_addr[18]{};
                ba2str(&((sockaddr_rc*) &addr)->rc_bdaddr, bt_addr);
                bt_addr[17] = '\0';
                name = bt_addr;
            } else if(addr.sa_family == AF_UNIX) {
                name = ((sockaddr_un*) &addr)->sun_path;
            } else {
                name = std::to_string(socket);
            }

            log->warn("Discarding connection from {} because there was no response in 100 milliseconds", name);
            return false;
        }

        auto init_package = ipc::IpcPackage::read_from_socket(socket);
        if(init_package.opcode() != ipc::Opcodes::MSG_IPC_TYPE) {
            log->warn("First package from IPC client must be of type MSG_IPC_TYPE(=0) but was {}", static_cast<uint32_t>(init_package.opcode()));
            return false;
        }

        ipc::IpcType type;
        init_package >> type;

        if(type >= ipc::IpcType::FIRST_UNKNOWN_IPC_TYPE) {
            log->warn("Invalid IPC init packet received");
            return false;
        }

        out_type = type;
        return true;
    }
}