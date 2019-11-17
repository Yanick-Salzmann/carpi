#include <iostream>
#include <gps/gps_listener.hpp>

namespace carpi {
    int main(int argc, char* argv[]) {
        gps::GpsListener listener{};
        return 0;
    }
}

int main(int argc, char* argv[]) {
    return carpi::main(argc, argv);
}