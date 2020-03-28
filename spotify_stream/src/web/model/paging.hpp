#ifndef CARPI_PAGING_HPP
#define CARPI_PAGING_HPP

#include <string>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <vector>

namespace carpi::spotify::web::model {
    template<typename T>
    struct paging {
        std::string href;
        std::vector<T> items;
        int32_t limit;
        std::string next;
        int32_t offset;
        std::string previous;
        int32_t total;
    };
}

#endif //CARPI_PAGING_HPP
