#include <iostream>
#include "api_gateway.hpp"
#include "oauth/refresh_flow.hpp"
#include "websocket_interface.hpp"
#include "spotify_device.hpp"
#include "state_machine.hpp"
#include "drm/widevine_adapter.hpp"
#include <common_utils/log.hpp>

namespace carpi {
    int main(int argc, const char* argv[]) {
        using namespace spotify;

        utils::Logger log{"main"};


        sApiGateway->load_urls();
        oauth::RefreshFlow refresh_flow;

        drm::WidevineAdapter adapter{refresh_flow.access_token()};
        adapter.create_session(utils::base64_decode("AAAAU3Bzc2gAAAAA7e+LqXnWSs6jyCfc1R0h7QAAADMIARIQYw72lEd8cwk+AygyvJYhuxoHc3BvdGlmeSIUYw72lEd8cwk+AygyvJYhu+B3nV0="));

        WebsocketInterface wss_interface{refresh_flow.access_token()};
        wss_interface.wait_for_login();

        log->info("Spotify login complete, creating new device");

        SpotifyDevice device{refresh_flow.access_token(), wss_interface.connection_id()};
        StateMachine stateMachine{device, wss_interface};

        std::cin.get();
        return 0;
    }
}

int main(int argc, const char* argv[]) {
    return carpi::main(argc, argv);
}