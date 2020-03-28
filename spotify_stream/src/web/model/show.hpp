#ifndef CARPI_SHOW_HPP
#define CARPI_SHOW_HPP

#include <vector>
#include <string>
#include "copyright.hpp"
#include "paging.hpp"
#include "episode_simplified.hpp"
#include "image.hpp"

namespace carpi::spotify::web::model {
    struct show {
        std::vector<std::string> available_markets;
        std::vector<copyright> copyrights;
        std::string description;
        bool is_explicit;
        paging<episode_simplified> episodes;
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

#endif //CARPI_SHOW_HPP
