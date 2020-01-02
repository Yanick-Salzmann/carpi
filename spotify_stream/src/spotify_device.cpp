#include "spotify_device.hpp"
#include <utility>
#include <nlohmann/json.hpp>

namespace carpi::spotify {
    using nlohmann::json;

    LOGGER_IMPL(SpotifyDevice);

    SpotifyDevice::SpotifyDevice(std::string access_token, std::string connection_id) :
            _connection_id{std::move(connection_id)},
            _access_token{std::move(access_token)} {
        enable_notifications();
        create_device();
    }

    void SpotifyDevice::create_device() {
        const auto device_desc = json::parse(std::string{
                R"(
{
    "device_id": "569ddafef1502867d1982b92e226337cc2039530",
    "device_type": "computer",
    "brand": "public_js-sdk",
    "model": "harmony-chrome.79-windows",
    "name": "CarPI",
    "metadata": {},
    "capabilities": {
        "change_volume": true,
        "audio_podcasts": true,
        "enable_play_token": true,
        "play_token_lost_behavior": "pause",
        "disable_connect": false,
        "manifest_formats": [ "file_urls_mp3", "file_urls_external" ]
    }
}
)"});
        json req_payload{};
        req_payload["device"] = device_desc;
        req_payload["connection_id"] = _connection_id;
        req_payload["previous_session_state"] = json{};
        req_payload["volume"] = 65535;
        req_payload["client_version"] = "harmony:3.19.1-441cc8f";

        log->info(req_payload.dump());

        net::HttpRequest req{"POST", "https://api.spotify.com/v1/track-playback/v1/devices"};
        req.add_header("Authorization", fmt::format("Bearer {}", _access_token))
                .add_header("Content-Type", "application/json")
                .add_header("Referer", "https://sdk.scdn.co/embedded/index.html")
                .add_header("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.88 Safari/537.36")
                .string_body(req_payload.dump());

        const auto resp = _client.execute(req);
        if (resp.status_code() != 200) {
            log->error("Error creating device: {} {} --> {} ", resp.status_code(), resp.status_text(), utils::bytes_to_utf8(resp.body()));
            throw std::runtime_error{"Error creating device"};
        }
    }

    void SpotifyDevice::enable_notifications() {
        net::HttpRequest req{"PUT", fmt::format("https://api.spotify.com/v1/me/notifications/user?connection_id={}", _connection_id)};
        req.add_header("Content-Length", "0")
                .add_header("Authorization", fmt::format("Bearer {}", _access_token));

        const auto resp = _client.execute(req);
        if (resp.status_code() != 200) {
            log->error("Error enabling notifications: {}", resp.to_string());
            throw std::runtime_error{"Error enabling notifications"};
        }
    }
}