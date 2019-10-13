#include <iostream>
#include <bluetooth_utils/BluetoothManager.hpp>

namespace carpi {
    int main(int argc, char* argv[]) {
        bluetooth::BluetoothManager mgr{};
        return 0;
    }
}

int main(int argc, char* argv[]) {
    return carpi::main(argc, argv);
}