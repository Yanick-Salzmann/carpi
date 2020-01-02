#include "api_gateway.hpp"
#include <nlohmann/json.hpp>
#include <net_utils/http_client.hpp>

namespace carpi::spotify {
    LOGGER_IMPL(ApiGateway);

    void ApiGateway::load_urls() {
        _config = toml::parse("resources/api.toml");
        _spotify_cfg = toml::find(_config, "spotify");
        _api_cfg = toml::find(_spotify_cfg, "api");

        load_dealer_url();
        load_refresh_token_url();
        load_application_key();
    }

    void ApiGateway::load_dealer_url() {
        using nlohmann::json;

        net::HttpClient client{};
        net::HttpRequest request{"GET", "https://apresolve.spotify.com/?type=dealer"};

        const auto response = client.execute(request);
        auto body = response.body();
        body.emplace_back('\0');

        std::string json_resp{body.begin(), body.end()};
        const auto val = json::parse(json_resp);
        const auto dealer_itr = val.find("dealer");
        if(dealer_itr == val.end()) {
            log->error("Error loading spotify API gateway, response '{}' expected to contain an element named 'dealer'", json_resp);
            throw std::runtime_error{"Invalid API gateway response"};
        }

        const auto arr = (*dealer_itr);
        for(const auto& elem : arr) {
            log->debug("Available dealer URL: {}", (std::string) elem);
            if(_gew_dealer.empty()) {
                _gew_dealer = elem;
            }
        }

        if(_gew_dealer.empty()) {
            log->error("No spotify API gateway found");
            throw std::runtime_error{"Invalid API gateway response"};
        }

        log->info("Using spotify dealer URL: {}", _gew_dealer);
    }

    void ApiGateway::load_refresh_token_url() {
        _auth_api = toml::find<std::string>(_api_cfg, "refresh_token");
        log->info("Using accounts URL: {}", _auth_api);
    }

    void ApiGateway::load_application_key() {
        const auto env_client_id = getenv(toml::find<std::string>(_api_cfg, "env_key_client_id").c_str());
        const auto env_client_secret = getenv(toml::find<std::string>(_api_cfg, "env_key_client_secret").c_str());

        if(!env_client_id) {
            log->error("No client id found in environment variables");
            throw std::runtime_error{"Missing client id environment variable"};
        }

        if(!env_client_secret) {
            log->error("No client secret found in environment variables");
            throw std::runtime_error{"Missing client secret environment variable"};
        }

        _client_id = env_client_id;
        _client_secret = env_client_secret;

        log->info("Spotify client ID: {}", _client_id);
    }
}