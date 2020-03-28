#ifndef CARPI_RECOMMENDATIONS_HPP
#define CARPI_RECOMMENDATIONS_HPP

#include <cstdint>
#include <string>
#include <vector>
#include "track_simplified.hpp"

namespace carpi::spotify::web::model {
    struct recommendations_seed {
        int32_t afterFilteringSize;
        int32_t afterRelinkingSize;
        std::string href;
        std::string id;
        int32_t initialPoolSize;
        std::string type;
    };

    struct recommendations {
        std::vector<recommendations_seed> seeds;
        std::vector<track_simplified> tracks;
    };
}

#endif //CARPI_RECOMMENDATIONS_HPP
