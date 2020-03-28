#include <common_utils/string.hpp>
#include "web_api_accessor.hpp"
#include "../api_gateway.hpp"

namespace carpi::spotify::web {
    LOGGER_IMPL(web_api_accessor);

    web_api_accessor::web_api_accessor() : _web_root{std::move(sApiGateway->web_api_url())} {

    }

    std::vector<playlist> web_api_accessor::fetch_playlists() {
        //auto ret = fetch_endpoint("v1/search?q=Selena+Gomez&type=album,track,artist,playlist");
        auto ret = fetch_endpoint("v1/me/playlists");
        log->info(ret.dump(4, ' '));
        const auto itr_href = ret.find("href");
        if(itr_href == ret.end()) {
            return {};
        }

        ret = fetch_endpoint(*itr_href);
    }

    nlohmann::json web_api_accessor::fetch_endpoint(const std::string &endpoint) {
        std::string uri;
        if(!utils::starts_with(endpoint, "http")) {
            uri = fmt::format("{}{}", _web_root, endpoint);
        } else {
            uri = endpoint;
        }

        const auto response = _client.execute(
                net::http_request{"GET", uri}
                        .add_header("Authorization", fmt::format("Bearer {}", _access_token))
        );

        if(response.status_code() == 204) {
            return nlohmann::json{};
        }

        if(response.status_code() != 200) {
            log->error("Error fetching endpoint {}: {} {} ({})", endpoint, response.status_code(), response.status_text(), utils::bytes_to_utf8(response.body()));
            return {};
        }

        return nlohmann::json::parse(utils::bytes_to_utf8(response.body()));
    }

    void web_api_accessor::init(std::string access_token) {
        _access_token = std::move(access_token);
    }

    play_state web_api_accessor::playstate() {
        return {[this](const auto& endpoint) { return fetch_endpoint(endpoint); }};
    }
};