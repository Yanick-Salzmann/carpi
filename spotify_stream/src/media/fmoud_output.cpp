#include "fmod_output.hpp"
#include "media_stream.hpp"

namespace carpi::spotify::media {
    LOGGER_IMPL(FmodSystem);

    FMOD_RESULT pcm_read_data(FMOD_SOUND *sound, void *data, unsigned int datalen) {
        FmodSystem *user_data = nullptr;
        FMOD_Sound_GetUserData(sound, (void **) &user_data);

        auto *read_buffer = (uint8_t *) data;

        auto num_read = std::size_t{0};
        while (num_read < datalen) {
            const auto read = user_data->_active_stream->read(read_buffer + num_read, datalen - num_read);
            if (!read) {
                return FMOD_ERR_FILE_EOF;
            }

            num_read += read;

        }

        return FMOD_OK;
    }

    FmodSystem::FmodSystem() : _system{nullptr} {
        auto status = FMOD::System_Create(&_system);
        if (status != FMOD_OK) {
            log_error(status, "FMOD::System_Create");
            throw std::runtime_error{"Error creating FMOD system"};
        }

        status = _system->init(64, FMOD_INIT_NORMAL, nullptr);
        if (status != FMOD_OK) {
            log_error(status, "FMOD::System::init");
            throw std::runtime_error{"Error initializing FMOD"};
        }

        _system_updater = std::thread{[=]() { main_loop(); }};
    }

    FmodSystem::~FmodSystem() {
        _is_running = false;
        if (_system_updater.joinable()) {
            _system_updater.join();
        }
    }

    void FmodSystem::log_error(FMOD_RESULT result, const std::string &prefix) {
        log->error("Error calling FMOD function '{}': {} (error={})", prefix, FMOD_ErrorString(result), result);
    }

    void FmodSystem::print_version() {
        uint32_t version = 0;
        _system->getVersion(&version);
        const auto minor = version & 0xFFu;
        const auto major = (version >> 8u) & 0xFFu;
        const auto product = (version >> 16u) & 0xFFFFu;
        log->info("FMOD version: {}.{}.{}", product, major, minor);
    }

    void FmodSystem::open_sound(std::shared_ptr<MediaStream> stream, std::size_t pcm_offset) {
        if (!stream->read_supported()) {
            log->warn("Stream is not readable");
            throw std::runtime_error{"Stream is not readable"};
        }

        _active_stream = stream;

        if (_active_channel != nullptr) {
            _active_channel->stop();
            _active_channel = nullptr;
        }

        if (_active_sound != nullptr) {
            _active_sound->release();
            _active_sound = nullptr;
        }

        FMOD_CREATESOUNDEXINFO sound_info{};
        sound_info.cbsize = sizeof sound_info;
        sound_info.format = FMOD_SOUND_FORMAT::FMOD_SOUND_FORMAT_PCM16;
        sound_info.userdata = this;
        sound_info.pcmreadcallback = pcm_read_data;
        sound_info.defaultfrequency = 44100;
        sound_info.decodebuffersize = 44100;
        sound_info.numchannels = 2;
        sound_info.initialseekposition = static_cast<unsigned int>(pcm_offset);
        sound_info.initialseekpostype = FMOD_TIMEUNIT_PCM;
        sound_info.length = static_cast<unsigned int>(-1);

        auto error = _system->createStream("file1", FMOD_OPENUSER | FMOD_LOOP_OFF | FMOD_CREATESTREAM, &sound_info, &_active_sound);
        if (error != FMOD_OK) {
            log_error(error, "FMOD::System::createSound");
            throw std::runtime_error{"Error creating sound"};
        }

        error = _system->playSound(_active_sound, nullptr, false, &_active_channel);
        if (error != FMOD_OK) {
            log_error(error, "FMOD::System::playSound");
            throw std::runtime_error{"Error playing sound"};
        }
    }

    void FmodSystem::main_loop() {
        while(_is_running) {
            _system->update();
            std::this_thread::sleep_for(std::chrono::milliseconds{20});
        }
    }
}