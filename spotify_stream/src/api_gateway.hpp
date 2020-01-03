#ifndef CARPI_API_GATEWAY_HPP
#define CARPI_API_GATEWAY_HPP

#include <common_utils/singleton.hpp>
#include <common_utils/log.hpp>
#include <toml.hpp>

namespace carpi::spotify {
    class ApiGateway : public utils::Singleton<ApiGateway> {
        LOGGER;

        std::string _gew_dealer;
        std::string _auth_api;

        std::string _client_id{};
        std::string _client_secret{};

        toml::value _config;
        toml::value _spotify_cfg;
        toml::value _api_cfg;

        void load_dealer_url();
        void load_refresh_token_url();

        void load_application_key();

    public:
        void load_urls();

        [[nodiscard]] std::string gew_dealer() const {
            return _gew_dealer;
        }

        [[nodiscard]] std::string accounts_api() const {
            return _auth_api;
        }

        [[nodiscard]] std::string client_id() const {
            return _client_id;
        }

        [[nodiscard]] std::string client_secret() const {
            return _client_secret;
        }
    };
}

#define sApiGateway (carpi::spotify::ApiGateway::instance())

#endif //CARPI_API_GATEWAY_HPP