#include "IpcClientManager.hpp"
#include "../../../ipc_common/include/ipc_common/IpcPackage.hpp"

namespace carpi::comm {
    LOGGER_IMPL(IpcClientManager);

    void IpcClientManager::accept_connection(int connection, sockaddr_un addr) {
        fd_set socket_set{};
        FD_SET(connection, &socket_set);
        // vpython src/build/linux/sysroot_scripts/install-sysroot.py --arch=x86
        struct timeval read_timeout{
            .tv_usec = 100000,
            .tv_sec = 0
        };

        const auto ret = select(connection, &socket_set, nullptr, nullptr, &read_timeout);
        if(ret <= 0) {
            log->warn("Discarding connection from {} because there was no response in 100 milliseconds", addr.sun_path);
            return;
        }

        const auto init_package = ipc::IpcPackage::read_from_socket(connection);
    }
}