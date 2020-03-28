cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(spotify_stream)

set(CMAKE_VERBOSE_MAKEFILE ON)

configure_file(resources/api.toml resources/api.toml)
configure_file(resources/config.toml resources/config.toml)

find_package(FFmpeg REQUIRED)

add_executable(spotify_stream src/main.cpp src/websocket_interface.hpp src/websocket_interface.cpp src/oauth/refresh_flow.hpp src/oauth/refresh_flow.cpp src/api_gateway.hpp src/api_gateway.cpp src/spotify_device.hpp src/spotify_device.cpp src/state_machine.hpp src/state_machine.cpp src/media/media_player.hpp src/media/media_player.cpp src/media/media_source.hpp src/media/media_source.cpp src/cdm/content_decryption_module.hpp src/cdm/content_decryption_module_export.hpp src/cdm/content_decryption_module_proxy.hpp src/drm/widevine_adapter.hpp src/drm/widevine_adapter.cpp src/drm/widevine_host.hpp src/drm/widevine_host.cpp src/drm/widevine_session.hpp src/drm/widevine_session.cpp src/media/mpeg_dash.hpp src/media/mpeg_dash.cpp src/media/media_stream.hpp src/media/media_stream.cpp src/media/fmod_output.hpp src/media/fmoud_output.cpp src/media/ffmpeg_converter.hpp src/media/ffmpeg_converter.cpp src/media/spotify_media_stream.hpp src/media/spotify_media_stream.cpp src/media/ffmpeg_converter_stream.hpp src/media/ffmpeg_converter_stream.cpp src/media/track_downloader.cpp src/media/track_downloader.hpp src/web/playlist.hpp src/web/playlist.cpp src/web/web_api_accessor.hpp src/web/web_api_accessor.cpp
        src/web/play_state.cpp src/web/play_state.hpp src/web/model/track.cpp src/web/model/track.hpp src/web/model/album.hpp src/web/model/album.cpp src/web/model/copyright.hpp src/web/model/copyright.cpp
        src/web/model/artist.hpp src/web/model/artist.cpp src/web/model/audio.hpp src/web/model/audio.cpp src/web/model/category.hpp src/web/model/category.cpp src/web/model/context.hpp src/web/model/context.cpp src/web/model/followers.hpp src/web/model/followers.cpp src/web/model/image.hpp src/web/model/image.cpp)

add_dependencies(spotify_stream net_utils toml json)

target_include_directories(
        spotify_stream PUBLIC
        ${FMOD_ROOT_DIR}/inc
        ${CMAKE_SOURCE_DIR}/net_utils/include
        ${CMAKE_BINARY_DIR}/third_party/toml/include
        ${CMAKE_BINARY_DIR}/third_party/json/include
        ${FFMPEG_INCLUDE_DIRS}
)

find_package(Threads)

target_link_libraries(
        spotify_stream
        net_utils
        Threads::Threads
        ${CMAKE_DL_LIBS}
        -lstdc++fs
        ${FFMPEG_LIBRARIES}
)

IF (${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
    target_link_libraries(
            spotify_stream
            ${FMOD_ROOT_DIR}/lib/armhf/libfmod.so
    )
else ()
    target_link_libraries(
            spotify_stream
            ${FMOD_ROOT_DIR}/lib/x86_64/libfmod.so
    )
endif ()