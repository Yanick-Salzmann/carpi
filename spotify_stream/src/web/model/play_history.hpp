#ifndef CARPI_PLAY_HISTORY_HPP
#define CARPI_PLAY_HISTORY_HPP

#include <vector>
#include "context.hpp"
#include "track_simplified.hpp"

namespace carpi::spotify::web::model {
    struct play_history {
        track_simplified track;
        std::string timestamp;
        context ctx;
    };
}

#endif //CARPI_PLAY_HISTORY_HPP
