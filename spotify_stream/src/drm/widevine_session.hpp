#ifndef CARPI_WIDEVINE_SESSION_HPP
#define CARPI_WIDEVINE_SESSION_HPP

#include <string>
#include <vector>
#include "../cdm/content_decryption_module.hpp"
#include <common_utils/log.hpp>
#include <net_utils/http_client.hpp>
#include <cstdint>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <uuid/uuid.h>

namespace carpi::spotify::drm {
    class WidevineAdapter;

    class WidevineSession {
        LOGGER;

        net::http_client _client{};

        std::string _session_id{};
        std::string _license_server_url{};
        std::string _access_token{};

        std::condition_variable _license_event{};
        std::mutex _license_lock;
        bool _has_license = false;

        WidevineAdapter *_adapter = nullptr;

        void forward_license_request(const std::vector<uint8_t> &data);

    public:
        explicit WidevineSession(WidevineAdapter *adapter, std::string session_id, std::string license_server_url, std::string access_token) :
                _adapter{adapter},
                _session_id{std::move(session_id)},
                _license_server_url{std::move(license_server_url)},
                _access_token{std::move(access_token)} {

        }

        void on_license_updated();

        void handle_message(cdm::MessageType message_type, const std::vector<uint8_t> &data);

        void wait_for_license();

        void decrypt_sample(uuid_t key_id, const void* iv, std::size_t iv_size, const void* encrypted, std::size_t encrypted_size, std::vector<uint8_t>& decrypted);
    };
}

#endif //CARPI_WIDEVINE_SESSION_HPP
