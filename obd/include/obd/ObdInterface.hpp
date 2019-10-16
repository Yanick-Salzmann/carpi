#ifndef CARPI_OBD_OBDINTERFACE_HPP
#define CARPI_OBD_OBDINTERFACE_HPP

#include <memory>
#include <vector>

namespace carpi::bluetooth {
    class BluetoothConnection;
}

namespace carpi::obd {
    class ObdCommand;

    class ObdInterface {
        std::shared_ptr<bluetooth::BluetoothConnection> _connection;

        bool _is_in_lower_power_mode = true;

        void send_raw(const std::string& payload);

        void trigger_normal_power();

        std::vector<std::string> read_raw();

    public:
        explicit ObdInterface(std::shared_ptr<bluetooth::BluetoothConnection> connection);

        void send_command(ObdCommand& command);
    };
}

#endif //CARPI_OBD_OBDINTERFACE_HPP
