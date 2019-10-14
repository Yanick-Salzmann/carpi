cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(video_stream)

find_package(FFmpeg REQUIRED)

add_library(video_stream SHARED src/ConversionContext.cpp include/video_stream/ConversionContext.hpp)

target_include_directories(video_stream PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/common_utils/include ${FFMPEG_INCLUDE_DIRS})
target_compile_definitions(video_stream PUBLIC ${FFMPEG_DEFINITIONS})
target_link_libraries(video_stream common_utils ${FFMPEG_LIBRARIES})