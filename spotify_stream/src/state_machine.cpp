#include "state_machine.hpp"
#include "websocket_interface.hpp"
#include "spotify_device.hpp"

namespace carpi::spotify {
    LOGGER_IMPL(StateMachine);

    using nlohmann::json;

    StateMachine::StateMachine(SpotifyDevice &device, WebsocketInterface &wss) :
            _device{device},
            _wss{wss} {
        _wss.state_machine(this);
    }

    void StateMachine::handle_state_replace(nlohmann::json payload) {
        const auto prev_state = payload["prev_state_ref"];
        const auto state_machine = payload["state_machine"];
        const auto state_ref = payload["state_ref"];
        const auto seek_to = payload["seek_to"];

        if (state_machine.is_null()) {
            return;
        }

        _id = state_machine["state_machine_id"];

        if (state_ref.is_null()) {
            return;
        }

        const uint64_t state_index = state_ref["state_index"];
        const auto active_state = state_machine["states"][state_index];
        _current_state = active_state["state_id"];
        log->info("Switching to state {}", _current_state);

        const bool paused = state_ref["paused"];

        if (!seek_to.is_null()) {
            _cur_position = seek_to;
        }

        const uint64_t track_index = active_state["track"];
        if (track_index >= state_machine["tracks"].size()) {
            return;
        }

        const auto active_track = state_machine["tracks"][track_index];
        const auto metadata = active_track["metadata"];
        _cur_duration = metadata["duration"];

        const std::string track_id = metadata["uri"];
        if (track_id != _current_track_id) {
            log->info("Switching to track {}", (std::string) metadata["name"]);
        }

        update_state(_current_state, paused);
    }

    void StateMachine::reject_state(const std::string &state_id, bool paused) {
        auto req = create_request("POST", "state_conflict")
                .string_body(json{
                        {"seq_nums",            {_device.next_sequence_number()}},
                        {"state_ref",           nullptr},
                        {"sub_state",           {
                                                 {"playback_speed", 0},
                                                        {"stream_time", 0}
                                                }
                        },
                        {"rejected_state_refs", {
                                                 {
                                                  {"state_machine_id", _id},
                                                                    {"state_id", state_id},
                                                         {"paused", paused}
                                                 }
                                                }}
                }.dump());

        const auto resp = _client.execute(req);
    }

    net::HttpRequest StateMachine::create_request(const std::string &method, const std::string &path) {
        return net::HttpRequest{method, fmt::format("https://api.spotify.com/v1/track-playback/v1/devices/{}/{}", _device.device_id(), path)}
                .add_header("Authorization", fmt::format("Bearer {}", _device.access_token()));
    }

    void StateMachine::update_state(const std::string &state_id, bool paused) {
        auto req = create_request("PUT", "state")
                .string_body(json{
                        {"seq_num",      _device.next_sequence_number()},
                        {"sub_state",    {
                                                 {"playback_speed",   paused ? 0 : 1},
                                                 {"stream_time", 0},
                                                 {"position", _cur_position},
                                                 {"duration", _cur_duration}
                                         }
                        },
                        {"state_ref",
                                         {
                                                 {"state_machine_id", _id},
                                                 {"state_id",    state_id},
                                                 {"paused",   paused}
                                         }
                        },
                        {"debug_source", "before_track_load"}
                }.dump());
        _client.execute(req);
    }
}