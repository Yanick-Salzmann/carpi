cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(common_utils)

add_library(common_utils SHARED src/log.cpp include/common_utils/error.hpp src/error.cpp include/common_utils/any.hpp src/any.cpp include/common_utils/conversion.hpp src/conversion.cpp include/common_utils/byte_stream.hpp src/byte_stream.cpp include/common_utils/singleton.hpp include/common_utils/network.hpp src/network.cpp include/common_utils/string.hpp src/string.cpp include/common_utils/process.hpp src/process.cpp include/common_utils/random.hpp src/random.cpp src/helper/basen.hpp include/common_utils/io.hpp src/io.cpp include/common_utils/time.hpp src/time.cpp)

add_dependencies(common_utils spdlog)

target_include_directories(common_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/third_party/spdlog/include)