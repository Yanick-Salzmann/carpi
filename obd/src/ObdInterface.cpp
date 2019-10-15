#include "obd/ObdInterface.hpp"
#include "bluetooth_utils/BluetoothConnection.hpp"

namespace carpi::obd {

    ObdInterface::ObdInterface(std::shared_ptr<bluetooth::BluetoothConnection> connection) :
            _connection(std::move(connection)) {

    }
}