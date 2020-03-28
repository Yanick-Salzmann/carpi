#include "copyright.hpp"

namespace carpi::spotify::web::model {
    void from_json(const nlohmann::json &j, copyright &c) {
        c.type = j["type"];
        c.text = j["text"];
    }
}