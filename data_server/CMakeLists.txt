cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(data_server)

add_library(data_server SHARED src/http_server.cpp include/data_server/http_server.hpp include/data_server/http_connection.hpp src/http_connection.cpp include/data_server/http_request.hpp include/data_server/http_response.hpp src/http_request.cpp src/http_response.cpp include/data_server/http_headers.hpp src/http_headers.cpp src/camera_handler.hpp src/camera_handler.cpp src/cookie_helper.hpp src/cookie_helper.cpp)

add_dependencies(data_server common_utils video_stream)

target_include_directories(data_server PUBLIC ${CMAKE_SOURCE_DIR}/common_utils/include ${CMAKE_SOURCE_DIR}/video_stream/include ${CMAKE_CURRENT_SOURCE_DIR}/include /usr/local/include/cef)
target_link_libraries(data_server common_utils video_stream /usr/local/lib/libcef_dll_wrapper.a /usr/local/bin/libcef.so)