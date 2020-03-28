#ifndef CARPI_CATEGORY_HPP
#define CARPI_CATEGORY_HPP

#include <nlohmann/json.hpp>
#include <string>

namespace carpi::spotify::web::model {
    struct category {
        std::string href;
        nlohmann::json icons;
        std::string id;
        std::string name;
    };
}

#endif //CARPI_CATEGORY_HPP
