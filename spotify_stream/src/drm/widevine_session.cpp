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
                net::HttpRequest request{"POST", _license_server_url};
                request.add_header("Authorization", fmt::format("Bearer {}", _access_token))
                        .byte_body(data);

                const auto response = _client.execute(request);
                if(response.status_code() != 200) {
                    log->error("Widevine license request not accepted by license server: {} {} ({})", response.status_code(), response.status_text(), utils::bytes_to_utf8(response.body()));
                    throw std::runtime_error{"Error requesting widevine license"};
                }

                log->info("Successfully requested widevine license from license server");
                _adapter->update_session(_session_id, response.body());
            }
        }.detach();

    }
}