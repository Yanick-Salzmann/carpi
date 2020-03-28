#ifndef CARPI_API_GATEWAY_HPP
#define CARPI_API_GATEWAY_HPP

#include <common_utils/singleton.hpp>
#include <common_utils/log.hpp>
#include <toml.hpp>

namespace carpi::spotify {
    class ApiGateway : public utils::singleton<ApiGateway> {
        LOGGER;

        std::string _gew_dealer;
        std::string _auth_api;
        std::string _app_certificate_endpoint;
        std::string _license_endpoint;
        std::string _seektable_url;
        std::string _web_api_url;

        std::string _audio_cdn_prefix;
        std::string _audio_cdn_suffix;

        std::string _client_id{};
        std::string _client_secret{};

        toml::value _config;
        toml::value _spotify_cfg;
        toml::value _api_cfg;

        void load_dealer_url();
        void load_refresh_token_url();
        void load_app_cert_url();
        void load_license_url();
        void load_seektable_url();
        void load_audio_cdn();
        void load_web_api_url();

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

        [[nodiscard]] std::string app_certificate_endpoint() const {
            return _app_certificate_endpoint;
        }

        [[nodiscard]] std::string license_endpoint() const {
            return _license_endpoint;
        }

        [[nodiscard]] std::string seektable_endpoint() const {
            return _seektable_url;
        }

        [[nodiscard]] std::string resolve_audio_file_cdn(const std::string& file_id);

        [[nodiscard]] std::string web_api_url() const {
            return _web_api_url;
        }
    };
}

#define sApiGateway (carpi::spotify::ApiGateway::instance())

#endif //CARPI_API_GATEWAY_HPP
