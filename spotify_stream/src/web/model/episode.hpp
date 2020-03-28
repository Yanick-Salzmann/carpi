#ifndef CARPI_EPISODE_HPP
#define CARPI_EPISODE_HPP

#include <string>
#include <cstdint>
#include <vector>
#include <nlohmann/json.hpp>
#include "image.hpp"
#include "resume_point.hpp"
#include "show_simplified.hpp"

namespace carpi::spotify::web::model {
    struct episode {
        std::string audio_preview_url;
        std::string description;
        int32_t duration_ms;
        bool is_explicit;
        std::vector<nlohmann::json> external_urls;
        std::string href;
        std::string id;
        std::vector<image> images;
        bool is_externally_hosted;
        bool is_playable;
        std::string language;
        std::vector<std::string> languages;
        std::string name;
        std::string release_date;
        std::string release_date_precision;
        resume_point rsm_pt;
        show_simplified show;
        std::string type;
        std::string uri;
    };
}

#endif //CARPI_EPISODE_HPP
