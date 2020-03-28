#ifndef CARPI_USER_PRIVATE_HPP
#define CARPI_USER_PRIVATE_HPP

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "image.hpp"
#include "followers.hpp"

namespace carpi::spotify::web::model {
    struct user_private {
        std::string country;
        std::string display_name;
        std::string email;
        std::vector<nlohmann::json> external_urls;
        followers fllwrs;
        std::string href;
        std::string id;
        std::vector<image> images;
        std::string product;
        std::string type;
        std::string uri;
    };
}

#endif //CARPI_USER_PRIVATE_HPP
