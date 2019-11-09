#include <future>
#include "camera_handler.hpp"
#include "range_helper.hpp"

#include <sys/wait.h>
#include <common_utils/error.hpp>

namespace carpi::data {
    LOGGER_IMPL(CameraHandler);

    void CameraHandler::initialize_camera() {
        std::lock_guard<std::mutex> l{_load_lock};
        if (_camera_stream) {
            return;
        }

        _camera_stream = std::make_shared<video::RawCameraStream>(
                [this](const std::vector<uint8_t> &data, std::size_t size) { handle_camera_frame(data, size); }
        );

        _camera_stream->initialize_camera({480, 360, 1, 30});
    }

    void CameraHandler::handle_camera_frame(const std::vector<uint8_t> &data, std::size_t size) {
        static FILE* out_file = fopen("cam_output.raw", "wb");

        if(++_num_frames == 0x100) {
            for(const auto& listener : _data_listeners) {
                close(listener->ffmpeg_process.stdin_pipe);
            }
            fclose(out_file);
        }

        if(_num_frames >= 0x100) {
            return;
        }

        fwrite(data.data(), 1, size, out_file);

        std::lock_guard<std::mutex> l{_listener_lock};
        for (const auto &listener : _data_listeners) {
            std::size_t written = 0;
            while (written < size) {
                const auto num_written = write(listener->ffmpeg_process.stdin_pipe, data.data() + written, size - written);
                written += num_written;
                if(num_written < 0) {
                    log->error("Broken stdin pipe (errno={})", errno);
                    break;
                }
            }
        }
    }

    void CameraHandler::handle_stderr_reader(const std::shared_ptr<ReaderContext> &context) {
        char buffer[4096]{};
        int32_t num_read = 0;
        while ((num_read = read(context->ffmpeg_process.stderr_pipe, buffer, sizeof buffer)) > 0) {
            std::string content{buffer, buffer + num_read};
            fputs(content.c_str(), stdout);
        }
    }

    void CameraHandler::handle_stdout_reader(const std::shared_ptr<ReaderContext> &context) {
        char buffer[4096]{};
        int32_t num_read = 0;
        while ((num_read = read(context->ffmpeg_process.stdout_pipe, buffer, sizeof buffer)) > 0) {
                std::lock_guard<std::mutex> l{context->data_lock};
                context->data_buffer.insert(context->data_buffer.end(), buffer, buffer + num_read);
                handle_context_data(context);
        }
    }

    void CameraHandler::cancel_stream(const std::string &session_cookie) {
        std::lock_guard<std::mutex> l{_listener_lock};
        const auto listeners = _listener_map.equal_range(session_cookie);
        if(listeners.first == listeners.second) {
            return;
        }

        for(auto itr = listeners.first; itr != listeners.second; ++itr) {
            close(itr->second->ffmpeg_process.stdin_pipe);
            if(itr->second->stdout_thread.joinable()) {
                itr->second->stdout_thread.join();
            }

            if(itr->second->stderr_thread.joinable()) {
                itr->second->stderr_thread.join();
            }

            _data_listeners.erase(itr->second);
        }

        _listener_map.erase(session_cookie);

        _camera_stream->stop_capture();
    }

    bool CameraHandler::is_current_stream(const std::string &session_cookie, std::size_t start) {
        std::shared_ptr<ReaderContext> ctx{};
        {
            std::lock_guard<std::mutex> l{_listener_lock};
            const auto itr = _listener_map.find(session_cookie);
            if(itr == _listener_map.end()) {
                return false;
            }

            ctx = itr->second;
        }

        return start >= ctx->last_sent_position;
    }

    void CameraHandler::start_stream(const std::string &token) {
        std::shared_ptr<ReaderContext> context = std::make_shared<ReaderContext>();

        context->ffmpeg_process = utils::launch_subprocess(
                "ffmpeg",
                {
                        "ffmpeg",
                        "-f", "rawvideo",
                        "-pix_fmt", "yuv420p",
                        "-s", fmt::format("{}x{}", _camera_stream->actual_width(), _camera_stream->actual_height()),
                        "-r", "30",
                        "-i", "-",
                        "-c", "libx264",
                        "-preset", "ultrafast",
                        "-crf", "0",
                        "-qp", "0",
                        "-f", "mp4",
                        "-movflags", "frag_keyframe+empty_moov",
                        "-",
                        "-loglevel", "error",
                        "-hide_banner"
                }
        );
        log->info("Launched ffmpeg process. PID: {}, error: {}", context->ffmpeg_process.process_id, context->ffmpeg_process.error_code);

        {
            std::lock_guard<std::mutex> l{_listener_lock};
            _data_listeners.emplace(context);
            _listener_map.emplace(token, context);
        }

        context->stdout_thread = std::thread{[this, context]() { handle_stdout_reader(context); }};
        context->stderr_thread = std::thread{[this, context]() { handle_stderr_reader(context); }};

        initialize_camera();
    }

    bool CameraHandler::request_range(const std::string& token, std::size_t start, std::size_t end, const std::function<bool(const std::vector<uint8_t> &, std::size_t)> &callback) {
        if(start >= end) {
            return false;
        }

        std::shared_ptr<ReaderContext> context{};
        {
            std::lock_guard<std::mutex> l{_listener_lock};
            const auto itr = _listener_map.find(token);
            if(itr == _listener_map.end()) {
                return false;
            }

            context = itr->second;
        }

        {
            std::lock_guard<std::mutex> l{context->data_lock};
            if(start < context->last_sent_position) {
                return false;
            }

            context->pending_requests.emplace_back(start, end, callback);
        }

        return true;
    }

    void CameraHandler::handle_context_data(const std::shared_ptr<ReaderContext>& context) {
        log->info("Handling context data");
        const auto data_end = context->last_sent_position + context->data_buffer.size();
        auto& ranges = context->pending_requests;
        const auto split = std::partition(ranges.begin(), ranges.end(), [data_end](const auto& range) { return range.start < data_end; });
        std::size_t last_position = 0;
        auto has_sent = false;

        for(auto itr = ranges.begin(); itr != split; ++itr) {
            if(itr->start < context->last_sent_position) {
                has_sent = true;
                last_position = context->last_sent_position;
                continue;
            }

            const auto offset = itr->start - context->last_sent_position;
            const auto num_bytes = std::min<std::size_t>(context->data_buffer.size() - offset, itr->end - itr->start);
            log->info("Sending {} bytes from {} (buffer size: {}, last sent position: {})", num_bytes, offset, context->data_buffer.size(), context->last_sent_position);
            std::vector<uint8_t> range_data{context->data_buffer.begin() + offset, context->data_buffer.begin() + offset + num_bytes};
            itr->callback(range_data, num_bytes);
            last_position = std::max<std::size_t>(last_position, context->last_sent_position + offset + num_bytes);
            has_sent = true;
        }

        if(!has_sent) {
            return;
        }

        std::list<RangeRequest> new_ranges{split, ranges.end()};
        context->pending_requests = new_ranges;

        const auto to_remove = last_position - context->last_sent_position;
        context->data_buffer.erase(context->data_buffer.begin(), context->data_buffer.begin() + to_remove);
        context->last_sent_position = last_position;
    }
}