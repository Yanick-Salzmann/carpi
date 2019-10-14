#include <iostream>
#include <bluetooth_utils/BluetoothServer.hpp>

namespace carpi {
    int main(int argc, char* argv[]) {
        bluetooth::BluetoothServer server{0x1001};
        server.accept_connection();
        return 0;
    }
}

int main(int argc, char* argv[]) {
    return carpi::main(argc, argv);
}