#include "playlist.hpp"

namespace carpi::spotify::web {
    playlist::playlist(std::string token) : _access_token{std::move(token)} {

    }
}
