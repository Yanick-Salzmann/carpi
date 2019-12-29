cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(net_utils)

find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

add_library(net_utils SHARED src/iface_utils.cpp include/net_utils/udp_broadcast.hpp src/udp_broadcast.cpp include/net_utils/udp_multicast.hpp src/udp_multicast.cpp src/ssl_socket.cpp include/net_utils/ssl_socket.hpp include/net_utils/websocket_client.hpp src/websocket_client.cpp include/net_utils/url_parser.hpp src/url_parser.cpp include/net_utils/http_request.hpp src/http_request.cpp)

add_dependencies(net_utils common_utils)

target_include_directories(
        net_utils PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/common_utils/include
        ${OPENSSL_INCLUDE_DIR}
        ${CURL_INCLUDE_DIR}
)

target_link_directories(
        net_utils PUBLIC
        ${libcurl_LIBRARY_DIRS}
)

target_link_libraries(
        net_utils
        common_utils
        ${OPENSSL_LIBRARIES}
        ${CURL_LIBRARIES}
)