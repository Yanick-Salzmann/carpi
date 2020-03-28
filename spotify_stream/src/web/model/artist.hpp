#ifndef CARPI_ARTIST_HPP
#define CARPI_ARTIST_HPP

#include <string>
#include <nlohmann/json.hpp>

namespace carpi::spotify::web::model {
    struct artist {
        nlohmann::json external_urls;
        nlohmann::json followers;
        std::vector<std::string> genres;
        std::string href;
        std::string id;
        nlohmann::json images;
        std::string gname;
        int32_t popularity;
        std::string type;
        std::string uri;
    };
}

#endif //CARPI_ARTIST_HPP
