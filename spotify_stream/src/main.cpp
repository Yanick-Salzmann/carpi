#include <iostream>
#include "api_gateway.hpp"
#include "oauth/refresh_flow.hpp"
#include "websocket_interface.hpp"
#include "spotify_device.hpp"
#include "state_machine.hpp"
#include "drm/widevine_adapter.hpp"
#include "media/fmod_output.hpp"
#include "web/web_api_accessor.hpp"
#include <common_utils/log.hpp>

namespace carpi {
    int main(int argc, const char* argv[]) {
        using namespace spotify;

        utils::Logger log{"main"};

        sFmodSystem->print_version();

        sApiGateway->load_urls();
        oauth::RefreshFlow refresh_flow;

        spotify_web_api->init(refresh_flow.access_token());
        auto state = spotify_web_api->playstate();
        log->info(state.currently_playing().to_string());

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