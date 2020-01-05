#ifndef CARPI_WIDEVINE_SESSION_HPP
#define CARPI_WIDEVINE_SESSION_HPP

#include <string>
#include <vector>
#include "../cdm/content_decryption_module.hpp"

namespace carpi::spotify::drm {
    class WidevineSession {
        std::string _session_id{};

    public:
        explicit WidevineSession(std::string session_id) : _session_id{std::move(session_id)} {

        }

        void handle_message(cdm::MessageType message_type, const std::vector<uint8_t>& data);
    };
}

#endif //CARPI_WIDEVINE_SESSION_HPP
