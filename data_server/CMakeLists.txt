cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(data_server)

add_library(data_server SHARED src/http_server.cpp include/data_server/http_server.hpp include/data_server/http_connection.hpp src/http_connection.cpp include/data_server/http_request.hpp include/data_server/http_response.hpp src/http_request.cpp src/http_response.cpp include/data_server/http_headers.hpp src/http_headers.cpp)

add_dependencies(data_server common_utils)

target_include_directories(data_server PUBLIC ${CMAKE_SOURCE_DIR}/common_utils/include ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(data_server  common_utils)