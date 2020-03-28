#ifndef CARPI_PLAYLIST_SIMPLE_HPP
#define CARPI_PLAYLIST_SIMPLE_HPP

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "image.hpp"

namespace carpi::spotify::web::model {
    struct playlist_simple {
        bool collaborative;
        std::string description;
        std::vector<nlohmann::json> external_urls;
        std::string href;
        std::string id;
        std::vector<image> images;
        std::string name;
        nlohmann::json owner;
        bool is_public;
        std::string snapshot_id;
        nlohmann::json tracks;
        std::string type;
        std::string uri;
    };
}

#endif //CARPI_PLAYLIST_SIMPLE_HPP
