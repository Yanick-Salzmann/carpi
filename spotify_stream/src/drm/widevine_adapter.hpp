#ifndef CARPI_WIDEVINE_ADAPTER_HPP
#define CARPI_WIDEVINE_ADAPTER_HPP

#include "../cdm/content_decryption_module.hpp"
#include <memory>
#include <vector>
#include <atomic>
#include <thread>
#include <future>
#include <map>
#include <deque>
#include <net_utils/http_client.hpp>
#include <common_utils/log.hpp>

namespace carpi::spotify::drm {
    class WidevineHost;
    class WidevineSession;

    class WidevineAdapter {
        LOGGER;

        net::HttpClient _client{};
        std::string _access_token{};

        std::mutex _license_server_lock;
        std::string _license_server{};
        time_t _license_server_expiration = 0;

        WidevineHost* _host;

        std::mutex _promise_lock;
        uint32_t _promise_id_generator{1};
        std::map<uint32_t, std::packaged_task<std::string (const char*, uint32_t)>*> _session_promises;
        std::map<uint32_t, std::packaged_task<void(bool)>*> _generic_promises;

        std::mutex _session_lock;
        std::map<std::string, std::shared_ptr<WidevineSession>> _active_sessions{};

        void* _cdm_library = nullptr;
        decltype(::CreateCdmInstance)* _create_cdm_instance;

        cdm::ContentDecryptionModule_10* _cdm;

        void fetch_server_certificate();

        void update_license_server_if_expired();
        void fetch_license_server_url();

        static void* create_cdm_host(int interface_version, void* user_data);

    public:
        explicit WidevineAdapter(std::string access_token);

        void session_promise_resolved(uint32_t promise_id, const char *session_id, uint32_t session_id_size);
        void promise_resolved(uint32_t promise_id);
        void promise_failed(uint32_t promise_id, cdm::Exception exception, uint32_t system_code, const char *error_message, uint32_t error_message_size);
        void session_message(const char *session_id, uint32_t session_id_size, cdm::MessageType message_type, const char *message, uint32_t message_size);

        std::shared_ptr<WidevineSession> create_session(const std::vector<uint8_t>& pssh_box);
    };
}

#endif //CARPI_WIDEVINE_ADAPTER_HPP
