#include "refresh_flow.hpp"
#include <ctime>
#include <fstream>

namespace carpi::spotify::oauth {

    RefreshFlow::RefreshFlow() {
        auto cfg = parse_config();
        auto token_cfg = toml::find<toml::value>(toml::find<toml::value>(cfg, "spotify"), "token");
        _refresh_token = toml::find<std::string>(token_cfg, "refresh");
    }

    toml::value RefreshFlow::parse_config() {
        return toml::parse("resources/config.toml");
    }

    void RefreshFlow::update_config() {
        auto cfg = parse_config();
        toml::value& spotify = toml::find(cfg, "spotify");
        toml::value& token = toml::find(spotify, "token");
        toml::find(token, "refresh") = _refresh_token;
        toml::find(token, "creation") = (uint64_t) time(nullptr);

        std::ofstream os{"resources/config.toml"};
        os << cfg;
    }
}