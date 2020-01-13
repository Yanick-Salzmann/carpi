#include "fmod_output.hpp"

namespace carpi::spotify::media {
    LOGGER_IMPL(FmodSystem);

    FmodSystem::FmodSystem() : _system{nullptr} {
        const auto status = FMOD::System_Create(&_system);
        if(status != FMOD_OK) {
            log_error(status, "FMOD_SystemCreate");
            throw std::runtime_error{"Error initializing FMOD"};
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

    void FmodSystem::open_sound(std::shared_ptr<MediaStream> stream) {
        FMOD_CREATESOUNDEXINFO sound_info{};
        sound_info.format = FMOD_SOUND_FORMAT::FMOD_SOUND_FORMAT_BITSTREAM;

        //_system->createSound(nullptr, FMOD_OPENUSER, )
    }
}