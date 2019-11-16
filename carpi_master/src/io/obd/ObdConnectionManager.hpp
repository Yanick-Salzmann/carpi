#ifndef CARPI_OBDCONNECTIONMANAGER_HPP
#define CARPI_OBDCONNECTIONMANAGER_HPP

#include <common_utils/singleton.hpp>

namespace carpi::io::obd {
    class ObdConnectionManager : public utils::Singleton<ObdConnectionManager> {
    public:
        bool connect_to_device(const std::string& address);
    };
}

#define sObdConnMgr (carpi::io::obd::ObdConnectionManager::instance())

#endif //CARPI_OBDCONNECTIONMANAGER_HPP
