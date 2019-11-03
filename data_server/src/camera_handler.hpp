#ifndef CARPI_DATA_SERVER_CAMERA_HANDLER_HPP
#define CARPI_DATA_SERVER_CAMERA_HANDLER_HPP

#include <common_utils/singleton.hpp>
#include <functional>
#include <video_stream/RawCameraStream.hpp>
#include <deque>
#include <memory>

#include <video_stream/H264Conversion.hpp>
#include <video_stream/H264Stream.hpp>

namespace carpi::data {
    class CameraHandler : public utils::Singleton<CameraHandler>, public video::IStreamSource {
        struct StreamSource : public video::IStreamSource {
            CameraHandler& parent;

            explicit StreamSource(CameraHandler& parent) : parent(parent) { }

            size_t read(void *buffer, std::size_t num_bytes) override {
                return parent.read(buffer, num_bytes);
            }
        };

        std::deque<std::function<bool(void *, std::size_t)>> _data_listeners;
        std::mutex _listener_lock;

        std::mutex _load_lock;
        std::shared_ptr<video::RawCameraStream> _camera_stream;
        std::shared_ptr<video::H264Conversion> _converter;

        std::mutex _frame_lock;
        std::condition_variable _frame_event;
        std::deque<std::vector<uint8_t>> _frame_queue;
        std::vector<uint8_t> _partial_frame;
        std::size_t _partial_position = 0;

        void initialize_camera();

        void handle_camera_frame(const std::vector<uint8_t> &data, std::size_t size);

        void handle_conversion_data(void *data, std::size_t size);

    public:
        size_t read(void *buffer, std::size_t num_bytes) override;

        void begin_streaming(const std::function<bool(void *, std::size_t)> &data_callback);
    };
}

#define sCameraHandler (carpi::data::CameraHandler::instance())

#endif //CARPI_DATA_SERVER_CAMERA_HANDLER_HPP
