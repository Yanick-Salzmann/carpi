#include "widevine_session.hpp"
#include "widevine_adapter.hpp"

namespace carpi::spotify::drm {
    LOGGER_IMPL(WidevineSession);

    void WidevineSession::handle_message(cdm::MessageType message_type, const std::vector<uint8_t> &data) {
        log->info("Session message, type {}, data size: {}", message_type, data.size());

        switch (message_type) {
            case cdm::MessageType::kLicenseRequest: {
                log->info("Handling widevine license request");
                forward_license_request(data);
                break;
            }

            default: {
                log->warn("CDM message not yet implemented: {}", message_type);
                break;
            }
        }
    }

    void WidevineSession::forward_license_request(const std::vector<uint8_t> &data) {
        std::thread{
                [=]() {
                    std::this_thread::sleep_for(std::chrono::milliseconds{100});

                    net::http_request request{"POST", _license_server_url};
                    request.add_header("Authorization", fmt::format("Bearer {}", _access_token))
                            .byte_body(data);

                    const auto response = _client.execute(request);
                    if (response.status_code() != 200) {
                        log->error("Widevine license request not accepted by license server: {} {} ({})", response.status_code(), response.status_text(), utils::bytes_to_utf8(response.body()));
                        throw std::runtime_error{"Error requesting widevine license"};
                    }

                    log->info("Successfully requested widevine license from license server");
                    _adapter->update_session(this, _session_id, response.body());
                }
        }.detach();
    }

    void WidevineSession::on_license_updated() {
        {
            std::lock_guard<std::mutex> l{_license_lock};
            _has_license = true;
        }

        _license_event.notify_all();
    }

    void WidevineSession::wait_for_license() {
        std::unique_lock l{_license_lock};
        _license_event.wait(l, [=]() { return _has_license; });
    }

    void WidevineSession::decrypt_sample(uuid_t key_id, const void* iv, std::size_t iv_size, const void* encrypted, std::size_t encrypted_size, std::vector<uint8_t>& decrypted) {
        _adapter->decrypt(key_id, iv, iv_size, encrypted, encrypted_size, decrypted);
    }
}