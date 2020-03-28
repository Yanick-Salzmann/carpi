#ifndef CARPI_TRACK_SIMPLIFIED_HPP
#define CARPI_TRACK_SIMPLIFIED_HPP

#include "track_link.hpp"
#include "artist_simplified.hpp"
#include <vector>
#include <string>
#include <cstdint>

namespace carpi::spotify::web::model {
    struct track_simplified {
        std::vector<artist_simplified> artist;
        std::vector<std::string> available_markets;
        int32_t disc_number;
        int32_t duration_ms;
        bool is_explicit;
        std::vector<nlohmann::json> external_urls;
        std::string href;
        std::string id;
        bool is_playable;
        track_link linked_from;
        nlohmann::json restrictions;
        std::string name;
        std::string preview_url;
        int32_t track_number;
        std::string type;
        std::string url;
        bool is_locale;
    };
}

#endif //CARPI_TRACK_SIMPLIFIED_HPP
