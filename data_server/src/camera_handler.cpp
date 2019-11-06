#include <future>
#include "camera_handler.hpp"

#include <sys/wait.h>
#include <common_utils/error.hpp>

namespace carpi::data {
    LOGGER_IMPL(CameraHandler);

    void CameraHandler::begin_streaming(const std::function<bool(void *, std::size_t)> &data_callback, const std::function<void()> &complete_callback) {
        std::shared_ptr<ReaderContext> context = std::make_shared<ReaderContext>();

        context->callback = data_callback;
        context->ffmpeg_process = utils::launch_subprocess(
                //"ffmpeg",
                //{"-f", "rawvideo", "-pix_fmt", "yuv420p", "-s", "1920x1080", "-r", "30", "-i", "-", "-c", "libx264", "-f", "mp4", "-movflags", "frag_keyframe+empty_moov", "-", "-loglevel", "trace"}
                "tee",
                { "dd_output.raw" }
        );
        log->info("Launched ffmpeg process. PID: {}, error: {}", context->ffmpeg_process.process_id, context->ffmpeg_process.error_code);

        {
            std::lock_guard<std::mutex> l{_listener_lock};
            _data_listeners.emplace(context);
        }

        std::thread stdout_thread{[this, context]() { handle_stdout_reader(context); }};
        std::thread stderr_thread{[this, context]() { handle_stderr_reader(context); }};

        initialize_camera();

        stdout_thread.join();
        stderr_thread.join();

        int32_t status_loc = 0;
        const auto child_proc = wait(&status_loc);
        if (WIFEXITED(status_loc)) {
            log->info("FFmpeg child process {} terminated (exit code = {}, error = {})", child_proc, WEXITSTATUS(status_loc), utils::error_to_string(WEXITSTATUS(status_loc)));
        } else {
            log->info("FFmpeg child process {} terminated abnormally", child_proc);
        }

        {
            std::lock_guard<std::mutex> l{_listener_lock};
            _data_listeners.erase(context);
        }

        complete_callback();
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
        FILE* f = fopen("output.yuv4", "ab");
        fwrite(data.data(), 1, size, f);
        fclose(f);

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
            //context->callback(buffer, num_read);
        }
    }
}