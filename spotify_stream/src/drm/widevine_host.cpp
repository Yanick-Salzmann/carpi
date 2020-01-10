#include <sys/time.h>
#include "widevine_host.hpp"
#include "widevine_adapter.hpp"
#include <vector>
#include <thread>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

namespace carpi::spotify::drm {
    LOGGER_IMPL(WidevineHost);

    class FixedBuffer : public cdm::Buffer {
        std::vector<uint8_t> _buffer;
        std::size_t _buffer_size = 0;

    public:
        explicit FixedBuffer(std::size_t capacity) {
            _buffer.resize(capacity);
        }

        void Destroy() override {

        }

        [[nodiscard]] uint32_t Capacity() const override {
            return static_cast<uint32_t>(_buffer.size());
        }

        uint8_t *Data() override {
            return _buffer.data();
        }

        void SetSize(uint32_t size) override {
            _buffer_size = size;
            if (_buffer_size > _buffer.size()) {
                _buffer.resize(_buffer_size);
            }
        }

        [[nodiscard]] uint32_t Size() const override {
            return static_cast<uint32_t>(_buffer_size);
        }

    protected:
        ~FixedBuffer() override = default;
    };

    class SimpleFileIO : public cdm::FileIO {
        int _file = -1;
        cdm::FileIOClient *_file_client;
        std::mutex _io_lock;

    public:
        explicit SimpleFileIO(cdm::FileIOClient *file_client) : _file_client{file_client} {

        }

        void Open(const char *file_name, uint32_t file_name_size) override {
            std::vector<char> name_buffer(file_name_size + 1);
            memcpy(name_buffer.data(), file_name, file_name_size);
            name_buffer.back() = '\0';
            _file = open(name_buffer.data(),  O_RDWR, S_IRUSR | S_IWUSR);
            if (_file < 0) {
                _file_client->OnOpenComplete(cdm::FileIOClient::Status::kError);
                return;
            }

            lseek(_file, 0, SEEK_SET);
            const auto ret = lockf(_file, F_TLOCK, 0);
            if (ret == EACCES || ret == EAGAIN) {
                _file_client->OnOpenComplete(cdm::FileIOClient::Status::kInUse);
            } else {
                _file_client->OnOpenComplete(cdm::FileIOClient::Status::kSuccess);
            }
        }

        void Read() override {
            if (!_io_lock.try_lock()) {
                _file_client->OnReadComplete(cdm::FileIOClient::Status::kInUse, nullptr, 0);
                return;
            }

            std::lock_guard<std::mutex> l{_io_lock, std::adopt_lock};
            const auto size = lseek(_file, 0, SEEK_END);
            lseek(_file, 0, SEEK_SET);
            if (!size) {
                _file_client->OnReadComplete(cdm::FileIOClient::Status::kSuccess, nullptr, 0);
                return;
            }

            std::vector<uint8_t> buffer((unsigned long) size);
            if (read(_file, buffer.data(), buffer.size()) != buffer.size()) {
                _file_client->OnReadComplete(cdm::FileIOClient::Status::kError, nullptr, 0);
                return;
            }

            _file_client->OnReadComplete(cdm::FileIOClient::Status::kSuccess, buffer.data(), static_cast<uint32_t>(buffer.size()));
        }

        void Write(const uint8_t *data, uint32_t data_size) override {
            if (!_io_lock.try_lock()) {
                _file_client->OnWriteComplete(cdm::FileIOClient::Status::kInUse);
                return;
            }

            std::lock_guard<std::mutex> l{_io_lock, std::adopt_lock};
            ftruncate(_file, 0);
            lseek(_file, 0, SEEK_SET);
            if (data && data_size) {
                if (write(_file, data, data_size) != data_size) {
                    _file_client->OnWriteComplete(cdm::FileIOClient::Status::kError);
                    return;
                }
            }

            _file_client->OnWriteComplete(cdm::FileIOClient::Status::kSuccess);
        }

        void Close() override {
            if (_file < 0) {
                close(_file);
                _file = -1;
            }
        }

    protected:
        ~SimpleFileIO() override {
            Close();
        }
    };


    cdm::Buffer *WidevineHost::Allocate(uint32_t capacity) {
        log->info("Allocating buffer of size {}", capacity);
        return new FixedBuffer{capacity};
    }

    void WidevineHost::SetTimer(int64_t delay_ms, void *context) {
        std::thread{
                [this, delay_ms, context]() {
                    std::this_thread::sleep_for(std::chrono::milliseconds{delay_ms});
                    _module->TimerExpired(context);
                }
        }.detach();
    }

    cdm::Time WidevineHost::GetCurrentWallTime() {
        time_t time_buffer;
        time(&time_buffer);

        return static_cast<cdm::Time>(time_buffer);
    }

    void WidevineHost::OnInitialized(bool success) {

    }

    void WidevineHost::OnResolveKeyStatusPromise(uint32_t promise_id, cdm::KeyStatus key_status) {

    }

    void WidevineHost::OnResolveNewSessionPromise(uint32_t promise_id, const char *session_id, uint32_t session_id_size) {
        if (_adapter != nullptr) {
            _adapter->session_promise_resolved(promise_id, session_id, session_id_size);
        }
    }

    void WidevineHost::OnResolvePromise(uint32_t promise_id) {
        if (_adapter != nullptr) {
            _adapter->promise_resolved(promise_id);
        }
    }

    void WidevineHost::OnRejectPromise(uint32_t promise_id, cdm::Exception exception, uint32_t system_code, const char *error_message, uint32_t error_message_size) {
        if (_adapter != nullptr) {
            _adapter->promise_failed(promise_id, exception, system_code, error_message, error_message_size);
        }
    }

    void WidevineHost::OnSessionMessage(const char *session_id, uint32_t session_id_size, cdm::MessageType message_type, const char *message, uint32_t message_size) {
        if (_adapter != nullptr) {
            _adapter->session_message(session_id, session_id_size, message_type, message, message_size);
        }
    }

    void WidevineHost::OnSessionKeysChange(const char *session_id, uint32_t session_id_size, bool has_additional_usable_key, const cdm::KeyInformation *keys_info, uint32_t keys_info_count) {

    }

    void WidevineHost::OnExpirationChange(const char *session_id, uint32_t session_id_size, cdm::Time new_expiry_time) {
        time_t t = (time_t) new_expiry_time;
        tm tmz;
        localtime_r(&t, &tmz);

        char buffer[1024]{};
        strftime(buffer, 1024, "%F %T", &tmz);
        log->info("Session expiration changed: {}", buffer);
    }

    void WidevineHost::OnSessionClosed(const char *session_id, uint32_t session_id_size) {

    }

    void WidevineHost::SendPlatformChallenge(const char *service_id, uint32_t service_id_size, const char *challenge, uint32_t challenge_size) {
        cdm::PlatformChallengeResponse response{};
        _module->OnPlatformChallengeResponse(response);
    }

    void WidevineHost::EnableOutputProtection(uint32_t desired_protection_mask) {

    }

    void WidevineHost::QueryOutputProtectionStatus() {
        _module->OnQueryOutputProtectionStatus(cdm::QueryResult::kQueryFailed, 0, 0);
    }

    void WidevineHost::OnDeferredInitializationDone(cdm::StreamType stream_type, cdm::Status decoder_status) {

    }

    cdm::FileIO *WidevineHost::CreateFileIO(cdm::FileIOClient *client) {
        // return new SimpleFileIO{client};
        return nullptr;
    }

    void WidevineHost::RequestStorageId(uint32_t version) {

    }
}