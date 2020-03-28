#ifndef CARPI_SHOW_SIMPLIFIED_HPP
#define CARPI_SHOW_SIMPLIFIED_HPP

#include <string>
#include <nlohmann/json.hpp>
#include "copyright.hpp"
#include "image.hpp"

namespace carpi::spotify::web::model {
    struct show_simplified {
        std::vector<std::string> available_markets;
        std::vector<copyright> copyrights;
        std::string description;
        bool is_explicit;
        std::vector<nlohmann::json> external_urls;
        std::string href;
        std::string id;
        std::vector<image> images;
        bool is_externally_hosted;
        std::vector<std::string> languages;
        std::string media_type;
        std::string name;
        std::string publisher;
        std::string type;
        std::string uri;
    };
}

#endif //CARPI_SHOW_SIMPLIFIED_HPP
