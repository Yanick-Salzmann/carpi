#include "obd/response/BitArrayCodec.hpp"
#include <sstream>
#include <iomanip>

namespace carpi::obd::response {

    std::ostream &operator<<(std::ostream &stream, const BitArray &array) {
        std::stringstream sstream;
        sstream << "<BitArray _data=[";
        auto is_first = true;
        for(const auto& elem : array._data) {
            if(is_first) {
                is_first = false;
            } else {
                sstream << ", ";
            }

            sstream << std::hex << std::setw(2) << std::setfill('0') << elem;
        }

        sstream << "]>";
        stream << sstream.str();
        return stream;
    }
}