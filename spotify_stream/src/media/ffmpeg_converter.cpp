#include "ffmpeg_converter.hpp"
#include "media_stream.hpp"
#include <common_utils/process.hpp>
#include <algorithm>
#include <unistd.h>

namespace carpi::spotify::media {
    LOGGER_IMPL(FfmpegConverter);

    FfmpegConverter::FfmpegConverter(std::shared_ptr<MediaStream> upstream) : _upstream{std::move(upstream)} {
        if (!_upstream->read_supported()) {
            log->error("FFMPEG input media stream does not support reading, this cannot work");
            throw std::runtime_error{"Input stream is not readable"};
        }

        _output_stream = std::make_shared<FfmpegConverterStream>();

        auto sub_process = utils::launch_subprocess("ffmpeg", {"ffmpeg", "-hide_banner", "-loglevel", "error", "-i", "-", "-f", "s16le", "-acodec", "pcm_s16le", "-"});

        _subprocess_threads.emplace_back(
                [=]() {
                    uint8_t buffer[8192]{};
                    auto num_read = _upstream->read(buffer, sizeof buffer);
                    while (num_read > 0) {
                        write(sub_process.stdin_pipe, buffer, num_read);
                        num_read = _upstream->read(buffer, sizeof buffer);
                    }

                    close(sub_process.stdin_pipe);
                }
        );

        _subprocess_threads.emplace_back(
                [=]() {
                    uint8_t buffer[8192]{};
                    auto num_read = read(sub_process.stdout_pipe, buffer, 8192);
                    while (num_read > 0) {
                        _output_stream->write(buffer, static_cast<size_t>(num_read));
                        num_read = read(sub_process.stdout_pipe, buffer, 8192);
                    }
                }
        );

        _subprocess_threads.emplace_back(
                [=]() {
                    uint8_t buffer[8192]{};
                    auto num_read = read(sub_process.stderr_pipe, buffer, 8192);
                    while (num_read > 0) {
                        fwrite(buffer, 1, static_cast<size_t>(num_read), stderr);
                        num_read = read(sub_process.stderr_pipe, buffer, 8192);
                    }
                }
        );

        log->info("Conversion started");
    }

    void FfmpegConverter::wait_for_completion() {
        std::for_each(_subprocess_threads.begin(), _subprocess_threads.end(), [](std::thread &thread) {
            if (thread.joinable()) {
                thread.join();
            }
        });
    }
}
