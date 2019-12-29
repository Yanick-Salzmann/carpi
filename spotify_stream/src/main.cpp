#include <iostream>
#include <net_utils/ssl_socket.hpp>
#include <net_utils/url_parser.hpp>
#include <net_utils/websocket_client.hpp>

namespace carpi {
    int main(int argc, const char* argv[]) {
        net::WebsocketClient websocket{"wss://gew-dealer.spotify.com/"
                                       "?access_token=BQCYQuGrmhvbsTANTd3hcMsTk_3F_a61kSy0JhHiQwFw-IoTMH0kZs7sD816emuhsoIwYKBsVGz0qgJmcucYAIu_-cmTFZO0aGfIR57dta7TGT3hobClbRNUSrL9ce01Dwg1l6gVi1o0Ab9S9kF82oXthMXkAmB6Q8ga87B7ceqiN_dB0yO-nTI"};

        return 0;
    }
}

int main(int argc, const char* argv[]) {
    return carpi::main(argc, argv);
}