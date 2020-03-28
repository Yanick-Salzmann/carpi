#ifndef CARPI_COPYRIGHT_HPP
#define CARPI_COPYRIGHT_HPP

#include <string>
#include <nlohmann/json.hpp>

namespace carpi::spotify::web::model {
    struct copyright {
        std::string text;
        std::string type;
    };

    void from_json(const nlohmann::json& j, copyright& c);
}

#endif //CARPI_COPYRIGHT_HPP
