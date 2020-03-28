#ifndef CARPI_TRACK_LINK_HPP
#define CARPI_TRACK_LINK_HPP

#include <vector>
#include <string>
#include <nlohmann/json.hpp>

namespace carpi::spotify::web::model {
    struct track_link {
        std::vector<nlohmann::json> external_urls;
        std::string href;
        std::string id;
        std::string type;
        std::string uri;
    };
}

#endif //CARPI_TRACK_LINK_HPP
