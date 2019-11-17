#include "ipc_common/ipc_mq.hpp"
#include <sys/stat.h>
#include <common_utils/error.hpp>
#include <map>
#include <iostream>

namespace carpi::ipc {
    LOGGER_IMPL(MessageQueue);
    static std::map<IpcMessageQueue, std::string> queue_names{
            {IpcMessageQueue::CHROMIUM_EVENTS, "/chromiumevents"},
            { IpcMessageQueue::GPS_BROADCAST, "/gpsbroadcast"}
    };

    MessageQueue::MessageQueue(IpcMessageQueue queue) : MessageQueue(queue_names[queue]) {
    }

    MessageQueue::MessageQueue(const std::string &name) {
        std::cout << "Opening MQ: " << name << std::endl;

        mq_attr attributes{
                .mq_maxmsg = 5,
                .mq_msgsize = MAX_MESSAGE_SIZE
        };
        _queue = mq_open(name.c_str(), O_RDWR | O_CREAT | O_CLOEXEC, 0777, &attributes);
        if (_queue == -1) {
            log->warn("Error creating message queue named '{}': {} (errno={})", name, utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error creating message queue"};
        }

        log->info("Opened message queue named '{}' -> id={}", name, (std::size_t) _queue);
    }

    MessageQueue::~MessageQueue() {
        mq_close(_queue);
    }

    IpcPackage MessageQueue::receive() {
        return IpcPackage::read_from_mq(_queue);
    }

    void MessageQueue::send(const IpcPackage &package) {
        package.send_to_mq(_queue);
    }
}