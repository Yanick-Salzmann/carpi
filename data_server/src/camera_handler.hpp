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

#include <video_stream/H264Conversion.hpp>
#include <video_stream/H264Stream.hpp>

namespace carpi::data {
    class CameraHandler : public utils::Singleton<CameraHandler> {
        LOGGER;

        struct ReaderContext {
            std::function<bool(void *, std::size_t)> callback;
            std::function<void()> complete_callback;
            std::shared_ptr<video::H264Conversion> converter;

            std::size_t last_read_index{};
            std::vector<uint8_t> partial_frame{};
            std::size_t partial_position = 0;
        };

        struct StreamSource : public video::IStreamSource {
            CameraHandler &parent;
            ReaderContext *context;

            explicit StreamSource(CameraHandler &parent, ReaderContext *context) : parent(parent), context(context) {}

            size_t read(void *buffer, std::size_t num_bytes) override {
                return parent.read(context, buffer, num_bytes);
            }
        };

        static constexpr std::size_t QUEUE_SIZE = 300;

        std::set<std::shared_ptr<ReaderContext>> _data_listeners;
        std::mutex _listener_lock;

        std::mutex _load_lock;
        std::shared_ptr<video::RawCameraStream> _camera_stream;
        std::shared_ptr<video::H264Conversion> _converter;

        std::mutex _frame_lock;

        std::condition_variable _frame_event;
        std::array<std::vector<uint8_t>, QUEUE_SIZE> _frame_queue;
        std::atomic_size_t _write_index{0};

        void initialize_camera();

        void handle_camera_frame(const std::vector<uint8_t> &data, std::size_t size);

        void handle_conversion_data(const std::shared_ptr<ReaderContext> &context, void *data, std::size_t size);

        size_t read(ReaderContext *context, void *buffer, std::size_t num_bytes);

    public:

        void begin_streaming(
                const std::function<bool(void *, std::size_t)> &data_callback,
                const std::function<void()> &complete_callback = {}
        );
    };
}

#define sCameraHandler (carpi::data::CameraHandler::instance())

#endif //CARPI_DATA_SERVER_CAMERA_HANDLER_HPP
