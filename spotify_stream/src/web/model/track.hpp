#ifndef CARPI_TRACK_HPP
#define CARPI_TRACK_HPP

#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <common_utils/string.hpp>
#include <iostream>

namespace carpi::spotify::web::model {
    struct track {
        nlohmann::json album;
        nlohmann::json artists;
        std::vector<std::string> available_markets;
        uint32_t disc_number;
        uint32_t duration_ms;
        bool is_explicit;
        nlohmann::json external_ids;
        nlohmann::json external_urls;
        std::string href;
        std::string id;
        bool is_playable;
        nlohmann::json linked_from;
        nlohmann::json restrictions;
        std::string name;
        uint32_t popularity;
        std::string preview_url;
        uint32_t track_number;
        std::string type;
        std::string uri;
        bool is_local;

        [[nodiscard]] std::string to_string() const {
            std::stringstream stream;
            stream << "[Track "
                   << "album=" << album.dump()
                   << ", artists=" << artists.dump()
                   << ", available_markets=[" << utils::join(available_markets, ", ") << "]"
                   << ", disc_number=" << disc_number
                   << ", duration_ms=" << duration_ms
                   << ", explicit=" << is_explicit
                   << ", external_ids=" << external_ids.dump()
                   << ", external_urls=" << external_urls.dump()
                   << ", href=" << href
                   << ", id=" << id
                   << ", is_playable=" << is_playable
                   << ", linked_from=" << linked_from.dump()
                   << ", restrictions=" << restrictions.dump()
                   << ", name=" << name
                   << ", preview_url=" << preview_url
                   << ", track_number=" << track_number
                   << ", type=" << type
                   << ", uri=" << uri
                   << ", is_local=" << is_local
                   << "]";

            return stream.str();
        }

        static track from_json(const nlohmann::json &json) {
            return track{
                    .album = read_if_found<nlohmann::json>(json, "album"),
                    .artists = read_if_found<nlohmann::json>(json, "artists"),
                    .available_markets = read_if_found<std::vector<std::string>>(json, "available_markets"),
                    .disc_number = read_if_found<uint32_t>(json, "disc_number"),
                    .duration_ms = read_if_found<uint32_t>(json, "duration_ms"),
                    .is_explicit = read_if_found<bool>(json, "explicit"),
                    .external_ids = read_if_found<nlohmann::json>(json, "external_ids"),
                    .external_urls = read_if_found<nlohmann::json>(json, "external_urls"),
                    .href = read_if_found<std::string>(json, "href"),
                    .id = read_if_found<std::string>(json, "id"),
                    .is_playable = read_if_found<bool>(json, "is_playable"),
                    .linked_from = read_if_found<nlohmann::json>(json, "linked_from"),
                    .restrictions = read_if_found<nlohmann::json>(json, "restrictions"),
                    .name = read_if_found<std::string>(json, "name"),
                    .popularity = read_if_found<uint32_t>(json, "popularity"),
                    .preview_url = read_if_found<std::string>(json, "preview_url"),
                    .track_number = read_if_found<uint32_t>(json, "track_number"),
                    .type = read_if_found<std::string>(json, "type"),
                    .uri = read_if_found<std::string>(json, "uri"),
                    .is_local = read_if_found<bool>(json, "is_local")
            };
        }

    private:
        template<typename T>
        static T read_if_found(const nlohmann::json &json, const std::string &key, const T &default_value = T{}) {
            const auto itr = json.find(key);
            if (itr == json.end()) {
                return default_value;
            }

            if((*itr).is_null()) {
                return default_value;
            }

            return (*itr);
        }
    };
}

#endif //CARPI_TRACK_HPP
