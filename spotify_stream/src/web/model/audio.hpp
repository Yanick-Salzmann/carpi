#ifndef CARPI_AUDIO_HPP
#define CARPI_AUDIO_HPP

#include <string>
#include <nlohmann/json.hpp>

namespace carpi::spotify::web::model {
    struct audio {
        float acousticness;
        std::string analysis_url;
        float danceability;
        int32_t duration_ms;
        float energy;
        std::string id;
        float instrumentalness;
        int32_t key;
        float liveness;
        float loudness;
        int32_t mode;
        float speechiness;
        float tempo;
        int32_t time_signature;
        std::string track_href;
        std::string type;
        std::string uri;
        float valence;
    };
}

#endif //CARPI_AUDIO_HPP
