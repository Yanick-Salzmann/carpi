#include <dlfcn.h>
#include <nlohmann/json.hpp>
#include "widevine_adapter.hpp"
#include "widevine_host.hpp"
#include "widevine_session.hpp"
#include "../api_gateway.hpp"

namespace carpi::spotify::drm {
    using nlohmann::json;

    LOGGER_IMPL(WidevineAdapter);

    const std::string WIDEVINE_KEY_SYSTEM{"com.widevine.alpha"};

    WidevineAdapter::WidevineAdapter(std::string access_token) : _access_token{std::move(access_token)} {
        const auto widevine_module = toml::find<std::string>(toml::find(toml::parse("resources/config.toml"), "widevine"), "library");
        log->info("Loading widevine from {}", widevine_module);

        _cdm_library = dlopen(widevine_module.c_str(), RTLD_NOW);
        _create_cdm_instance = (decltype(_create_cdm_instance)) dlsym(_cdm_library, "CreateCdmInstance");
        auto get_cdm_version = (decltype(GetCdmVersion) *) dlsym(_cdm_library, "GetCdmVersion");
        log->info("Widevine Version: {}", get_cdm_version());
        const auto cdm = _create_cdm_instance(cdm::ContentDecryptionModule_10::kVersion, WIDEVINE_KEY_SYSTEM.c_str(), static_cast<uint32_t>(WIDEVINE_KEY_SYSTEM.size()), WidevineAdapter::create_cdm_host, this);
        _cdm = (cdm::ContentDecryptionModule_10 *) cdm;

        if (_host != nullptr) {
            _host->module(_cdm);
        }

        _cdm->Initialize(false, false, false);

        fetch_server_certificate();
        fetch_license_server_url();
    }

    void *WidevineAdapter::create_cdm_host(int interface_version, void *user_data) {
        if (interface_version != cdm::Host_10::kVersion) {
            throw std::runtime_error{"Only host version 10 supported"};
        }

        auto adapter = (WidevineAdapter *) user_data;
        auto host = new WidevineHost();
        adapter->_host = host;
        host->adapter(adapter);
        return static_cast<cdm::Host_10 *>(host);
    }

    void WidevineAdapter::session_promise_resolved(uint32_t promise_id, const char *session_id, uint32_t session_id_size) {
        std::packaged_task<std::string(const char *, uint32_t)> *session_task = nullptr;
        {
            std::lock_guard<std::mutex> l{_promise_lock};
            const auto itr = _session_promises.find(promise_id);
            if (itr == _session_promises.end()) {
                return;
            }

            session_task = itr->second;
            _session_promises.erase(itr);
        }

        if (session_task) {
            (*session_task)(session_id, session_id_size);
        }
    }

    void WidevineAdapter::promise_failed(uint32_t promise_id, cdm::Exception exception, uint32_t system_code, const char *error_message, uint32_t error_message_size) {
        std::packaged_task<void(bool)> *generic_task = nullptr;
        std::packaged_task<std::string(const char *, uint32_t)> *session_task = nullptr;

        {
            std::lock_guard<std::mutex> l{_promise_lock};
            const auto itr = _session_promises.find(promise_id);
            if (itr != _session_promises.end()) {
                session_task = itr->second;
                _session_promises.erase(itr);
            }

            const auto generic_itr = _generic_promises.find(promise_id);
            if (generic_itr != _generic_promises.end()) {
                generic_task = generic_itr->second;
                _generic_promises.erase(generic_itr);
            }
        }

        if (session_task) {
            (*session_task)(nullptr, 0);
        }

        if (generic_task) {
            (*generic_task)(false);
        }
    }

    std::shared_ptr<WidevineSession> WidevineAdapter::create_session(const std::vector<uint8_t> &pssh_box) {
        update_license_server_if_expired();

        std::packaged_task<std::string(const char *, uint32_t)> task{
                [this](const char *session_id, uint32_t size) {
                    if (session_id == nullptr) {
                        return std::string{};
                    }

                    std::string id{session_id, session_id + size};
                    {
                        std::lock_guard<std::mutex> l{_session_lock};
                        _active_sessions.emplace(id, std::make_shared<WidevineSession>(this, id, _license_server, _access_token));
                    }

                    return id;
                }
        };

        std::future<std::string> promise{};
        uint32_t promise_id;
        {
            std::lock_guard<std::mutex> l{_promise_lock};
            promise_id = _promise_id_generator++;

            promise = task.get_future();
            _session_promises.insert(std::make_pair<>(promise_id, &task));
        }

        _cdm->CreateSessionAndGenerateRequest(promise_id, cdm::SessionType::kTemporary, cdm::InitDataType::kCenc, pssh_box.data(), static_cast<uint32_t>(pssh_box.size()));

        const auto session_id = promise.get();
        if (session_id.empty()) {
            return nullptr;
        }

        return _active_sessions[session_id];
    }

    void WidevineAdapter::session_message(const char *session_id, uint32_t session_id_size, cdm::MessageType message_type, const char *message, uint32_t message_size) {
        std::string sess_id{session_id, session_id + session_id_size};
        std::vector<uint8_t> buffer{message, message + message_size};

        std::shared_ptr<WidevineSession> session{};
        {
            std::lock_guard<std::mutex> l{_session_lock};
            const auto itr = _active_sessions.find(sess_id);
            if (itr == _active_sessions.end()) {
                return;
            }

            session = itr->second;
        }

        session->handle_message(message_type, buffer);
    }

    void WidevineAdapter::fetch_server_certificate() {
        net::HttpRequest req{"GET", sApiGateway->app_certificate_endpoint()};
        req.add_header("Authorization", fmt::format("Bearer {}", _access_token))
                .add_header("cache-control", "no-cache")
                .add_header("referer", "https://sdk.scdn.co/embedded/index.html")
                .add_header("pragma", "no-cache")
                .add_header("user-agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.88 Safari/537.36");


        const auto response = _client.execute(req);
        if (response.status_code() != 200) {
            log->error("Error fetching widevine application certificate: {} {} ({})", response.status_code(), response.status_text(), utils::bytes_to_utf8(response.body()));
            throw std::runtime_error{"Error getting widevine license certificate"};
        }

        const auto cert = response.body();
        const auto hdr = response.header("content-length");

        uint32_t promise_id = 0;
        auto success = false;
        std::packaged_task<void(bool)> task{
                [&success](bool result) {
                    success = result;
                }
        };

        {
            std::lock_guard<std::mutex> l{_promise_lock};
            promise_id = _promise_id_generator++;
            _generic_promises.emplace(promise_id, &task);
        }

        _cdm->SetServerCertificate(promise_id, cert.data(), static_cast<uint32_t>(cert.size()));

        task.get_future().get();
        if (!success) {
            log->error("There was an error setting widevine server certificate");
            throw std::runtime_error{"Error setting widevine certificate"};
        }

        log->info("Successfully set up application certificate for widevine license server");
    }

    void WidevineAdapter::promise_resolved(uint32_t promise_id) {
        std::packaged_task<void(bool)> *task = nullptr;
        {
            std::lock_guard<std::mutex> l{_promise_lock};

            const auto itr = _generic_promises.find(promise_id);
            if (itr != _generic_promises.end()) {
                task = itr->second;
                _generic_promises.erase(itr);
            } else {
                return;
            }
        }

        (*task)(true);
    }

    void WidevineAdapter::fetch_license_server_url() {
        net::HttpRequest req{"GET", sApiGateway->license_endpoint()};
        req.add_header("Authorization", fmt::format("Bearer {}", _access_token));

        const auto response = _client.execute(req);
        const auto content = utils::bytes_to_utf8(response.body());
        if (response.status_code() != 200) {
            log->error("Error fetching widevine license server URL: {} {} ({})", response.status_code(), response.status_text(), content);
            throw std::runtime_error{"Error fetching widevine license server"};
        }

        const auto payload = json::parse(content);
        _license_server = std::string{"https://api.spotify.com/v1/"} + (std::string) payload["uri"];
        _license_server_expiration = (time_t) payload["expires"];

        tm local_time;
        localtime_r(&_license_server_expiration, &local_time);

        char time_string[1024]{};
        strftime(time_string, (sizeof time_string) - 1, "%F %T", &local_time);

        log->info("Widevine license server URL: {}, expires at {}", _license_server, time_string);
    }

    void WidevineAdapter::update_license_server_if_expired() {
        std::lock_guard<std::mutex> l{_license_server_lock};
        time_t cur_time{};
        time(&cur_time);
        if (cur_time < _license_server_expiration) {
            return;
        }

        fetch_license_server_url();
    }

    void WidevineAdapter::update_session(const std::string &session_id, const std::vector<uint8_t> &license_response) {
        uint32_t promise_id = 0;
        auto success = false;
        std::packaged_task<void(bool)> task{
                [&success](bool result) {
                    success = result;
                }
        };

        {
            std::lock_guard<std::mutex> l{_promise_lock};
            promise_id = _promise_id_generator++;
            _generic_promises.emplace(promise_id, &task);
        }

        log->info("Updating session with {} bytes", license_response.size());
        _cdm->UpdateSession(promise_id, session_id.data(), static_cast<uint32_t>(session_id.size()),
                            license_response.data(), static_cast<uint32_t>(license_response.size()));

        task.get_future().get();

        log->info("Updating widevine session with license data was {}", success ? "successful" : "not successful");
    }
}