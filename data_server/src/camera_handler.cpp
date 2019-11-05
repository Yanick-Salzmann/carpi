#include <future>
#include "camera_handler.hpp"

namespace carpi::data {
    LOGGER_IMPL(CameraHandler);

    void CameraHandler::begin_streaming(const std::function<bool(void *, std::size_t)> &data_callback, const std::function<void()> &complete_callback) {
        initialize_camera();

        std::shared_ptr<ReaderContext> context = std::make_shared<ReaderContext>();
        {
            std::lock_guard<std::mutex> l{_listener_lock};
            _data_listeners.emplace(context);
        }

        context->callback = data_callback;
        context->ffmpeg_process = utils::launch_subprocess("ffmpeg", {"-pix_format", "yuv420p", "-video_size", "1920x1080", "-framerate", "30", "-i", "-", "-c", "libx264", "-f", "mp4", "-"});

        std::thread stdout_thread{[this, context]() { handle_stdout_reader(context); }};
        std::thread stderr_thread{[this, context]() { handle_stderr_reader(context); }};

        stdout_thread.join();
        stderr_thread.join();
    }

    void CameraHandler::initialize_camera() {
        std::lock_guard<std::mutex> l{_load_lock};
        if (_camera_stream) {
            return;
        }


        _camera_stream = std::make_shared<video::RawCameraStream>(
                [this](const std::vector<uint8_t> &data, std::size_t size) { handle_camera_frame(data, size); }
        );

        _camera_stream->initialize_camera({1920, 1080, 1, 30});
    }

    void CameraHandler::handle_camera_frame(const std::vector<uint8_t> &data, std::size_t size) {
        std::lock_guard<std::mutex> l{_listener_lock};
        for(const auto& listener : _data_listeners) {
            write(listener->ffmpeg_process.stdin_pipe, data.data(), size);
        }
    }

    void CameraHandler::handle_stderr_reader(const std::shared_ptr<ReaderContext> &context) {
        char buffer[4096]{};
        int32_t num_read = 0;
        while((num_read = read(context->ffmpeg_process.stderr_pipe, buffer, sizeof buffer)) > 0) {
            log->error("ffmpeg error: {}", std::string{buffer, buffer + num_read});
        }
    }

    void CameraHandler::handle_stdout_reader(const std::shared_ptr<ReaderContext> &context) {
        char buffer[4096]{};
        int32_t num_read = 0;
        while((num_read > read(context->ffmpeg_process.stdout_pipe, buffer, sizeof buffer)) > 0) {
            context->callback(buffer, num_read);
        }
    }
}