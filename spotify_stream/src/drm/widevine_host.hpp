#ifndef CARPI_WIDEVINE_HOST_HPP
#define CARPI_WIDEVINE_HOST_HPP

#include "../cdm/content_decryption_module.hpp"
#include <common_utils/log.hpp>

namespace carpi::spotify::drm {
    class WidevineAdapter;

    class WidevineHost : public cdm::Host_10 {
        LOGGER;

        cdm::ContentDecryptionModule_10* _module = nullptr;
        WidevineAdapter* _adapter = nullptr;

    public:
        void module(cdm::ContentDecryptionModule_10* module) {
            _module = module;
        }

        void adapter(WidevineAdapter* adapter) {
            _adapter = adapter;
        }

        void free_buffer(cdm::Buffer* buffer);

        cdm::Buffer *Allocate(uint32_t capacity) override;

        void SetTimer(int64_t delay_ms, void *context) override;

        cdm::Time GetCurrentWallTime() override;

        void OnInitialized(bool success) override;

        void OnResolveKeyStatusPromise(uint32_t promise_id, cdm::KeyStatus key_status) override;

        void OnResolveNewSessionPromise(uint32_t promise_id, const char *session_id, uint32_t session_id_size) override;

        void OnResolvePromise(uint32_t promise_id) override;

        void OnRejectPromise(uint32_t promise_id, cdm::Exception exception, uint32_t system_code, const char *error_message, uint32_t error_message_size) override;

        void OnSessionMessage(const char *session_id, uint32_t session_id_size, cdm::MessageType message_type, const char *message, uint32_t message_size) override;

        void OnSessionKeysChange(const char *session_id, uint32_t session_id_size, bool has_additional_usable_key, const cdm::KeyInformation *keys_info, uint32_t keys_info_count) override;

        void OnExpirationChange(const char *session_id, uint32_t session_id_size, cdm::Time new_expiry_time) override;

        void OnSessionClosed(const char *session_id, uint32_t session_id_size) override;

        void SendPlatformChallenge(const char *service_id, uint32_t service_id_size, const char *challenge, uint32_t challenge_size) override;

        void EnableOutputProtection(uint32_t desired_protection_mask) override;

        void QueryOutputProtectionStatus() override;

        void OnDeferredInitializationDone(cdm::StreamType stream_type, cdm::Status decoder_status) override;

        cdm::FileIO *CreateFileIO(cdm::FileIOClient *client) override;

        void RequestStorageId(uint32_t version) override;
    };
}

#endif //CARPI_WIDEVINE_HOST_HPP
