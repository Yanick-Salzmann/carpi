#include <iostream>
#include <bluetooth_utils/BluetoothServer.hpp>
#include <bluetooth_utils/BluetoothConnection.hpp>

namespace carpi {
    int main(int argc, char* argv[]) {
        utils::log::Logger log{"main"};
        bluetooth::BluetoothServer server{1};
        auto connection = server.accept_connection();
        std::string payload{};
        (*connection) >> payload;
        log->info("Received: {}", payload);
        return 0;
    }
}

int main(int argc, char* argv[]) {
    return carpi::main(argc, argv);
}