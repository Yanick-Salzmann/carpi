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
#include <list>

#include <video_stream/H264Conversion.hpp>
#include <video_stream/H264Stream.hpp>
#include <common_utils/process.hpp>

namespace carpi::data {
    class Range;

    class CameraHandler : public utils::Singleton<CameraHandler> {
        LOGGER;

        struct RangeRequest {
            std::size_t start;
            std::size_t end;

            std::function<void(const std::vector<uint8_t> &, std::size_t)> callback;

            RangeRequest() = default;

            RangeRequest(std::size_t start, std::size_t end, std::function<void(const std::vector<uint8_t> &, std::size_t)> callback) :
                    start(start), end(end), callback(std::move(callback)) {

            }
        };

        struct ReaderContext {
            utils::SubProcess ffmpeg_process;
            std::thread stdout_thread;
            std::thread stderr_thread;

            std::size_t last_sent_position{};

            std::vector<uint8_t> data_buffer{};
            std::mutex data_lock;

            std::list<RangeRequest> pending_requests;
        };

        static const uint32_t CAMERA_WIDTH = 480;
        static const uint32_t CAMERA_HEIGHT = 360;

        std::map<std::string, std::shared_ptr<ReaderContext>> _listener_map{};

        std::set<std::shared_ptr<ReaderContext>> _data_listeners;
        std::mutex _listener_lock;

        std::mutex _load_lock;
        std::shared_ptr<video::RawCameraStream> _camera_stream;

        std::atomic_size_t _num_frames = 0;

        void initialize_camera();

        void handle_camera_frame(const std::vector<uint8_t> &data, std::size_t size);

        void handle_stdout_reader(const std::shared_ptr<ReaderContext> &context);

        static void handle_stderr_reader(const std::shared_ptr<ReaderContext> &context);

        void handle_context_data(const std::shared_ptr<ReaderContext>& context);
    public:
        bool is_current_stream(const std::string &session_cookie, std::size_t start);

        void cancel_stream(const std::string &session_cookie);

        void start_stream(const std::string &token);

        bool request_range(const std::string &token, std::size_t start, std::size_t end, const std::function<bool(const std::vector<uint8_t> &, std::size_t)> &callback);
    };
}

#define sCameraHandler (carpi::data::CameraHandler::instance())

#endif //CARPI_DATA_SERVER_CAMERA_HANDLER_HPP
