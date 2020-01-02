#ifndef CARPI_SPOTIFY_DEVICE_HPP
#define CARPI_SPOTIFY_DEVICE_HPP

#include <string>
#include <net_utils/http_client.hpp>
#include <common_utils/log.hpp>

namespace carpi::spotify {
    class SpotifyDevice {
        LOGGER;

        net::HttpClient _client;
        std::string _connection_id;
        std::string _access_token;

        void enable_notifications();
        void create_device();

    public:
        explicit SpotifyDevice(std::string access_token, std::string connection_id);
    };
}

#endif //CARPI_SPOTIFY_DEVICE_HPP
