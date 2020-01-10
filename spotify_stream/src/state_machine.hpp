#ifndef CARPI_STATE_MACHINE_HPP
#define CARPI_STATE_MACHINE_HPP

#include <nlohmann/json.hpp>
#include <common_utils/log.hpp>
#include <net_utils/http_client.hpp>

namespace carpi::spotify {
    class SpotifyDevice;

    class WebsocketInterface;

    namespace media {
        class MediaPlayer;
    }

    class StateMachine {
        LOGGER;

        net::HttpClient _client;

        SpotifyDevice &_device;
        WebsocketInterface &_wss;

        std::shared_ptr<media::MediaPlayer> _media_player;

        std::string _id{};
        std::string _current_state{};
        std::string _current_track_id{};

        uint64_t _cur_position = 0;
        uint64_t _cur_duration = 0;

        void reject_state(const std::string &state_id, bool paused);

        void update_state(const std::string &state_id, bool paused);

        net::HttpRequest create_request(const std::string &method, const std::string &path);

    public:
        StateMachine(SpotifyDevice &_device, WebsocketInterface &_wss);

        void handle_state_replace(nlohmann::json payload);
    };
}

#endif //CARPI_STATE_MACHINE_HPP
