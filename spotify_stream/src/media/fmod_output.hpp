#ifndef CARPI_FMOD_OUTPUT_HPP
#define CARPI_FMOD_OUTPUT_HPP

#include <thread>
#include <fmod.hpp>
#include <fmod_errors.h>
#include <common_utils/log.hpp>
#include <common_utils/singleton.hpp>
#include <functional>

namespace carpi::spotify::media {
    class MediaStream;

    class FmodSystem : public utils::singleton<FmodSystem> {
        LOGGER;

        friend FMOD_RESULT pcm_read_data(FMOD_SOUND *sound, void *data, unsigned int datalen);

        FMOD::System *_system;
        FMOD::Sound *_active_sound = nullptr;
        FMOD::Channel *_active_channel = nullptr;

        std::thread _system_updater;
        bool _is_running = true;

        std::function<void(std::size_t)> _media_callback{};

        std::shared_ptr<MediaStream> _active_stream = nullptr;

        void log_error(FMOD_RESULT result, const std::string &prefix);

        void main_loop();

    public:
        FmodSystem();

        ~FmodSystem();

        void print_version();

        void open_sound(std::shared_ptr<MediaStream> stream, std::size_t pcm_offset, bool paused);

        void paused(bool paused);

        void register_media_system_update_callback(std::function<void(std::size_t)> callback) {
            _media_callback = std::move(callback);
        }
    };
}

#define sFmodSystem (carpi::spotify::media::FmodSystem::instance())

#endif //CARPI_FMOD_OUTPUT_HPP
