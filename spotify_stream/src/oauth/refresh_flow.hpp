#ifndef CARPI_REFRESH_FLOW_HPP
#define CARPI_REFRESH_FLOW_HPP

#include <string>
#include <toml.hpp>

namespace carpi::spotify::oauth {
    class RefreshFlow {
        std::string _refresh_token{};

        toml::value parse_config();
        void update_config();

        void generate_token();
    public:
        RefreshFlow();
    };
}

#endif //CARPI_REFRESH_FLOW_HPP
