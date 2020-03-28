#ifndef CARPI_USER_PUBLIC_HPP
#define CARPI_USER_PUBLIC_HPP

#include <string>
#include <nlohmann/json.hpp>
#include <vector>
#include "followers.hpp"
#include "image.hpp"

namespace carpi::spotify::web::model {
    struct user {
        std::string display_name;
        std::vector<nlohmann::json> external_urls;
        followers fllwrs;
        std::string href;
        std::string id;
        std::vector<image> images;
        std::string type;
        std::string uri;
    };
}

#endif //CARPI_USER_PUBLIC_HPP
