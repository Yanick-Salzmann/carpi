#ifndef CARPI_IPC_COMMON_IPC_MQ_HPP
#define CARPI_IPC_COMMON_IPC_MQ_HPP

#include <string>
#include <fcntl.h>
#include <mqueue.h>

#include <common_utils/log.hpp>
#include "ipc_package.hpp"

namespace carpi::ipc {
    enum class IpcMessageQueue {
        CHROMIUM_EVENTS,
        GPS_BROADCAST
    };

    class MessageQueue {
    public:
        static const std::size_t MAX_MESSAGE_SIZE = 512;

    private:
        LOGGER;

        mqd_t _queue{};
    public:
        explicit MessageQueue(IpcMessageQueue queue);
        explicit MessageQueue(const std::string& name);
        ~MessageQueue();

        IpcPackage receive();
        void send(const IpcPackage& package);
    };
}

#endif //CARPI_IPC_COMMON_IPC_MQ_HPP
