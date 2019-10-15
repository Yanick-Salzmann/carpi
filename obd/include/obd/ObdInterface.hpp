#ifndef CARPI_OBD_OBDINTERFACE_HPP
#define CARPI_OBD_OBDINTERFACE_HPP

#include <memory>

namespace carpi::bluetooth {
    class BluetoothConnection;
}

namespace carpi::obd {
    class ObdInterface {
        std::shared_ptr<bluetooth::BluetoothConnection> _connection;

    public:
        explicit ObdInterface(std::shared_ptr<bluetooth::BluetoothConnection> connection);
    };
}

#endif //CARPI_OBD_OBDINTERFACE_HPP
