#ifndef CARPI_PLAY_STATE_HPP
#define CARPI_PLAY_STATE_HPP

#include <nlohmann/json.hpp>
#include <functional>
#include "model/track.hpp"

namespace carpi::spotify::web {
    class play_state {
        std::function<nlohmann::json (const std::string&)> _http_resolver;

    public:
        play_state(std::function<nlohmann::json (const std::string&)> resolver);

        model::track currently_playing();
    };
}

#endif //CARPI_PLAY_STATE_HPP
