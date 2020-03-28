#ifndef CARPI_ARTIST_SIMPLIFIED_HPP
#define CARPI_ARTIST_SIMPLIFIED_HPP

#include <string>
#include <nlohmann/json.hpp>

namespace carpi::spotify::web::model {
    struct artist_simplified {
        nlohmann::json external_urls;
        std::string href;
        std::string id;
        std::string name;
        std::string type;
        std::string uri;
    };
}

#endif //CARPI_ARTIST_SIMPLIFIED_HPP
