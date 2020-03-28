#ifndef CARPI_ALBUM_SIMPLIFIED_HPP
#define CARPI_ALBUM_SIMPLIFIED_HPP

#include <string>
#include <nlohmann/json.hpp>
#include "image.hpp"

namespace carpi::spotify::web::model {
    struct album_simplified {
        std::string album_group;
        std::string album_type;
        nlohmann::json artists;
        std::vector<std::string> available_markets;
        std::vector<nlohmann::json> external_urls;
        std::string href;
        std::string id;
        std::vector<image> images;
        std::string name;
        std::string release_date;
        std::string release_date_precision;
        nlohmann::json restrictions;
        std::string type;
        std::string uri;
    };
}

#endif //CARPI_ALBUM_SIMPLIFIED_HPP
