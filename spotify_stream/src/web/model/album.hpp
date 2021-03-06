#ifndef CARPI_ALBUM_HPP
#define CARPI_ALBUM_HPP

#include <string>
#include <nlohmann/json.hpp>
#include <vector>
#include "copyright.hpp"
#include "image.hpp"

namespace carpi::spotify::web::model {
    struct album {
        std::string album_type;
        nlohmann::json artists;
        std::vector<std::string> available_markets;
        std::vector<copyright> copyrights;
        nlohmann::json external_ids;
        nlohmann::json external_urls;
        std::vector<std::string> genres;
        std::string href;
        std::string id;
        std::vector<image> images;
        std::string name;
        uint32_t popularity;
        std::string release_date;
        std::string release_date_precision;
        nlohmann::json restrictions;
        nlohmann::json tracks;
        std::string type;
        std::string uri;
    };
}

#endif //CARPI_ALBUM_HPP
