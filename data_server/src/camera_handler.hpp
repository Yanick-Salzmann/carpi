#ifndef CARPI_DATA_SERVER_CAMERA_HANDLER_HPP
#define CARPI_DATA_SERVER_CAMERA_HANDLER_HPP

#include <common_utils/singleton.hpp>
#include <functional>
#include <video_stream/RawCameraStream.hpp>
#include <deque>
#include <memory>
#include <array>
#include <atomic>
#include <set>
#include <map>

#include <video_stream/H264Conversion.hpp>
#include <video_stream/H264Stream.hpp>
#include <common_utils/process.hpp>

namespace carpi::data {
    class CameraHandler : public utils::Singleton<CameraHandler> {
        LOGGER;

        struct ReaderContext {
            std::function<bool(void *, std::size_t)> callback;

            utils::SubProcess ffmpeg_process;

            std::size_t last_sent_position{};

            std::vector<uint8_t> data_buffer{};
            std::condition_variable data_event{};
            std::mutex data_lock;
        };

        static constexpr std::size_t QUEUE_SIZE = 300;

        std::map<std::string, std::shared_ptr<ReaderContext>> _listener_map{};

        std::set<std::shared_ptr<ReaderContext>> _data_listeners;
        std::mutex _listener_lock;

        std::mutex _load_lock;
        std::shared_ptr<video::RawCameraStream> _camera_stream;

        std::atomic_size_t _num_frames = 0;

        void initialize_camera();

        void handle_camera_frame(const std::vector<uint8_t> &data, std::size_t size);

        void handle_stdout_reader(const std::shared_ptr<ReaderContext>& context);
        void handle_stderr_reader(const std::shared_ptr<ReaderContext>& context);

    public:
        void begin_streaming(
                const std::function<bool(void *, std::size_t)> &data_callback,
                const std::function<void()> &complete_callback = {}
        );

        bool is_current_stream(const std::string& session_cookie, const std::string& range);
    };
}

#define sCameraHandler (carpi::data::CameraHandler::instance())

#endif //CARPI_DATA_SERVER_CAMERA_HANDLER_HPP
