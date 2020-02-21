#ifndef CARPI_WEB_API_ACCESSOR_HPP
#define CARPI_WEB_API_ACCESSOR_HPP


#include <common_utils/singleton.hpp>
#include <vector>
#include <net_utils/http_client.hpp>
#include "playlist.hpp"

namespace carpi::spotify::web {
    class web_api_accessor : public utils::singleton<web_api_accessor> {
        net::http_client _client{};

    public:
        std::vector<playlist> fetch_playlists();
    };
}

#define spotify_web_api (carpi::spotify::web::web_api_accessor::instance())

#endif //CARPI_WEB_API_ACCESSOR_HPP
