#ifndef CARPI_REFRESH_FLOW_HPP
#define CARPI_REFRESH_FLOW_HPP

#include <string>
#include <toml.hpp>
#include <common_utils/log.hpp>

namespace carpi::spotify::oauth {
    class RefreshFlow {
        LOGGER;

        std::string _refresh_token{};
        std::string _access_token{};

        toml::value parse_config();
        void update_config();

        void generate_token();
    public:
        RefreshFlow();

        std::string access_token() const {
            return _access_token;
        }
    };
}

#endif //CARPI_REFRESH_FLOW_HPP
