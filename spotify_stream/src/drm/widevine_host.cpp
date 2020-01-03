#include "widevine_host.hpp"

namespace carpi::spotify::drm {

    cdm::Buffer *WidevineHost::Allocate(uint32_t capacity) {
        return nullptr;
    }

    void WidevineHost::SetTimer(int64_t delay_ms, void *context) {

    }

    cdm::Time WidevineHost::GetCurrentWallTime() {
        return 0;
    }

    void WidevineHost::OnInitialized(bool success) {

    }

    void WidevineHost::OnResolveKeyStatusPromise(uint32_t promise_id, cdm::KeyStatus key_status) {

    }

    void WidevineHost::OnResolveNewSessionPromise(uint32_t promise_id, const char *session_id, uint32_t session_id_size) {

    }

    void WidevineHost::OnResolvePromise(uint32_t promise_id) {

    }

    void WidevineHost::OnRejectPromise(uint32_t promise_id, cdm::Exception exception, uint32_t system_code, const char *error_message, uint32_t error_message_size) {

    }

    void WidevineHost::OnSessionMessage(const char *session_id, uint32_t session_id_size, cdm::MessageType message_type, const char *message, uint32_t message_size) {

    }

    void WidevineHost::OnSessionKeysChange(const char *session_id, uint32_t session_id_size, bool has_additional_usable_key, const cdm::KeyInformation *keys_info, uint32_t keys_info_count) {

    }

    void WidevineHost::OnExpirationChange(const char *session_id, uint32_t session_id_size, cdm::Time new_expiry_time) {

    }

    void WidevineHost::OnSessionClosed(const char *session_id, uint32_t session_id_size) {

    }

    void WidevineHost::SendPlatformChallenge(const char *service_id, uint32_t service_id_size, const char *challenge, uint32_t challenge_size) {

    }

    void WidevineHost::EnableOutputProtection(uint32_t desired_protection_mask) {

    }

    void WidevineHost::QueryOutputProtectionStatus() {

    }

    void WidevineHost::OnDeferredInitializationDone(cdm::StreamType stream_type, cdm::Status decoder_status) {

    }

    cdm::FileIO *WidevineHost::CreateFileIO(cdm::FileIOClient *client) {
        return nullptr;
    }

    void WidevineHost::RequestStorageId(uint32_t version) {

    }
}