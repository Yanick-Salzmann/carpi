#ifndef CARPI_PLAYLIST_HPP
#define CARPI_PLAYLIST_HPP

#include <string>

namespace carpi::spotify::web {
    class playlist {
        std::string _access_token;

    public:
        explicit playlist(std::string token);
    };
};

#endif //CARPI_PLAYLIST_HPP
