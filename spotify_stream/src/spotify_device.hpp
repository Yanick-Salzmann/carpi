#ifndef CARPI_SPOTIFY_DEVICE_HPP
#define CARPI_SPOTIFY_DEVICE_HPP

#include <string>
#include <net_utils/http_client.hpp>
#include <common_utils/log.hpp>
#include "drm/widevine_adapter.hpp"

namespace carpi::spotify {
    class SpotifyDevice {
        LOGGER;

        net::http_client _client;
        std::string _connection_id;
        std::string _access_token;
        std::string _device_id;

        std::atomic_uint64_t _sequence_number;

        drm::WidevineAdapter _drm_frontend;

        void verify_feature_flags();
        void enable_notifications();
        void create_device();
        void setup_volume();

    public:
        explicit SpotifyDevice(std::string access_token, std::string connection_id);

        drm::WidevineAdapter& drm_frontend() {
            return _drm_frontend;
        }

        uint64_t next_sequence_number() {
            return ++_sequence_number;
        }

        [[nodiscard]] std::string device_id() const {
            return _device_id;
        }

        [[nodiscard]] std::string access_token() const {
            return _access_token;
        }
    };
}

#endif //CARPI_SPOTIFY_DEVICE_HPP
