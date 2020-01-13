#ifndef CARPI_FMOD_OUTPUT_HPP
#define CARPI_FMOD_OUTPUT_HPP

#include <fmod.hpp>
#include <fmod_errors.h>
#include <common_utils/log.hpp>
#include <common_utils/singleton.hpp>

namespace carpi::spotify::media {
    class MediaStream;

    class FmodSystem : public utils::Singleton<FmodSystem> {
        LOGGER;

        FMOD::System *_system;

        void log_error(FMOD_RESULT result, const std::string &prefix);

    public:
        FmodSystem();

        void print_version();

        void open_sound(std::shared_ptr<MediaStream> stream);
    };
}

#define sFmodSystem (carpi::spotify::media::FmodSystem::instance())

#endif //CARPI_FMOD_OUTPUT_HPP
