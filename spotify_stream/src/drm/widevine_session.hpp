#ifndef CARPI_WIDEVINE_SESSION_HPP
#define CARPI_WIDEVINE_SESSION_HPP

#include <string>
#include <vector>
#include "../cdm/content_decryption_module.hpp"
#include <common_utils/log.hpp>
#include <net_utils/http_client.hpp>

namespace carpi::spotify::drm {
    class WidevineAdapter;

    class WidevineSession {
        LOGGER;

        net::HttpClient _client{};

        std::string _session_id{};
        std::string _license_server_url{};
        std::string _access_token{};

        WidevineAdapter *_adapter = nullptr;

        void forward_license_request(const std::vector<uint8_t> &data);

    public:
        explicit WidevineSession(WidevineAdapter *adapter, std::string session_id, std::string license_server_url, std::string access_token) :
                _adapter{adapter},
                _session_id{std::move(session_id)},
                _license_server_url{std::move(license_server_url)},
                _access_token{std::move(access_token)} {

        }

        void handle_message(cdm::MessageType message_type, const std::vector<uint8_t> &data);
    };
}

#endif //CARPI_WIDEVINE_SESSION_HPP
