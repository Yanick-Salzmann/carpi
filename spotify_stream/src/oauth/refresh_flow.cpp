#include "refresh_flow.hpp"
#include "../api_gateway.hpp"
#include <ctime>
#include <fstream>
#include <net_utils/http_client.hpp>
#include <common_utils/conversion.hpp>
#include <nlohmann/json.hpp>

namespace carpi::spotify::oauth {
    LOGGER_IMPL(RefreshFlow);

    RefreshFlow::RefreshFlow() {
        auto cfg = parse_config();
        auto token_cfg = toml::find<toml::value>(toml::find<toml::value>(cfg, "spotify"), "token");
        _refresh_token = toml::find<std::string>(token_cfg, "refresh");

        generate_token();
    }

    toml::value RefreshFlow::parse_config() {
        const auto root_config = toml::parse("resources/config.toml");
        const auto spotify_config = toml::find(root_config, "spotify");
        const auto path = toml::find<std::string>(spotify_config, "credentials");
        return toml::parse(path);
    }

    void RefreshFlow::update_config() {
        auto cfg = parse_config();
        toml::value &spotify = toml::find(cfg, "spotify");
        toml::value &token = toml::find(spotify, "token");
        toml::find(token, "refresh") = _refresh_token;
        toml::find(token, "creation") = (uint64_t) time(nullptr);

        std::ofstream os{"resources/spotify.toml"};
        os << cfg;
    }

    void RefreshFlow::generate_token() {
        net::HttpClient client{};
        net::HttpRequest req{"POST", sApiGateway->accounts_api()};
        req.add_header("Authorization", fmt::format("Basic {}", utils::base64_encode(fmt::format("{}:{}", sApiGateway->client_id(), sApiGateway->client_secret()))))
                .add_header("Accept", "application/json")
                .add_header("Content-Type", "application/x-www-form-urlencoded")
                .string_body(fmt::format("grant_type=refresh_token&refresh_token={}", _refresh_token));

        const auto response = client.execute(req);
        if(response.status_code() != 200) {
            log->error("Error refreshing access token, unexpected return code {} {}", response.status_code(), response.status_text());
            throw std::runtime_error{"Error refreshing access token"};
        }

        using nlohmann::json;

        const auto body = json::parse(utils::bytes_to_utf8(response.body()));
        const auto access_token_itr = body.find("access_token");
        if(access_token_itr == body.end()) {
            log->error("Body '{}' does not contain a value for key 'access_token'", body.dump());
            throw std::runtime_error{"Error refreshing access token"};
        }

        _access_token = *access_token_itr;

        const auto refresh_token_itr = body.find("refresh_token");
        if(refresh_token_itr != body.end()) {
            _refresh_token = *refresh_token_itr;
            update_config();
        }

        log->info("Access token: {}", _access_token);
    }
}