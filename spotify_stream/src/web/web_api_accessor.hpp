#ifndef CARPI_WEB_API_ACCESSOR_HPP
#define CARPI_WEB_API_ACCESSOR_HPP


#include <common_utils/singleton.hpp>
#include <vector>
#include <net_utils/http_client.hpp>
#include "playlist.hpp"
#include <nlohmann/json.hpp>
#include "common_utils/log.hpp"
#include "play_state.hpp"

namespace carpi::spotify::web {
    class web_api_accessor : public utils::singleton<web_api_accessor> {
        LOGGER;

        net::http_client _client{};
        std::string _access_token;

        std::string _web_root{};

        nlohmann::json fetch_endpoint(const std::string& endpoint);

    public:
        web_api_accessor();

        void init(std::string access_token);
        std::vector<playlist> fetch_playlists();

        play_state playstate();
    };
}

#define spotify_web_api (carpi::spotify::web::web_api_accessor::instance())

#endif //CARPI_WEB_API_ACCESSOR_HPP
