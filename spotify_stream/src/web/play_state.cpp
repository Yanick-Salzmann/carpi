#include "play_state.hpp"

namespace carpi::spotify::web {
    play_state::play_state(std::function<nlohmann::json(const std::string &)> resolver) : _http_resolver{std::move(resolver)} {

    }

    model::track play_state::currently_playing() {
        const auto resp = _http_resolver("v1/me/player/currently-playing");
        if(resp.empty()) {
            return {};
        }

        return model::track::from_json(resp["item"]);
    }
}