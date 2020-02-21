#ifndef CARPI_OBDCONNECTIONMANAGER_HPP
#define CARPI_OBDCONNECTIONMANAGER_HPP

#include <common_utils/singleton.hpp>
#include <obd/ObdInterface.hpp>

namespace carpi::io::obd {
    class ObdConnectionManager : public utils::singleton<ObdConnectionManager> {
        std::shared_ptr<carpi::obd::ObdInterface> _connection;

    public:
        bool connect_to_device(const std::string& address);

        uint32_t fetch_speed();
        float fetch_rpm();
    };
}

#define sObdConnMgr (carpi::io::obd::ObdConnectionManager::instance())

#endif //CARPI_OBDCONNECTIONMANAGER_HPP
