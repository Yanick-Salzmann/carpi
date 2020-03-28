#ifndef CARPI_CONTEXT_HPP
#define CARPI_CONTEXT_HPP

#include <string>
#include <nlohmann/json.hpp>

namespace carpi::spotify::web::model {
    struct context {
        std::string type;
        std::string href;
        /**
         * values of type: { key: value } i.e. { "spotify": "https://...." }
         */
        std::vector<nlohmann::json> external_urls;
        std::string uri;
    };
}

#endif //CARPI_CONTEXT_HPP
