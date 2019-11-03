#include <future>
#include "camera_handler.hpp"

namespace carpi::data {
    LOGGER_IMPL(CameraHandler);

    void CameraHandler::begin_streaming(const std::function<bool(void *, std::size_t)> &data_callback) {
        _data_listeners.emplace_back(data_callback);
        initialize_camera();
    }

    void CameraHandler::initialize_camera() {
        std::lock_guard<std::mutex> l{_load_lock};
        if (_camera_stream) {
            return;
        }

        auto future = std::async(std::launch::async, [this]() {
            _converter = std::make_shared<video::H264Conversion>(
                    std::make_shared<video::H264Stream>(std::make_shared<StreamSource>(*this), 800, 600, 30),
                    "mp4",
                    [this](void *data, std::size_t size) { handle_conversion_data(data, size); },
                    []() {}
            );
        });


        _camera_stream = std::make_shared<video::RawCameraStream>(
                [this](const std::vector<uint8_t> &data, std::size_t size) { handle_camera_frame(data, size); }
        );

        _camera_stream->initialize_camera({800, 600, 1, 30});
        future.get();
    }

    size_t CameraHandler::read(void *buffer, std::size_t num_bytes) {
        if (!_partial_frame.empty()) {
            if (_partial_position < _partial_frame.size()) {
                const auto to_read = std::min<std::size_t>(num_bytes, _partial_frame.size() - _partial_position);
                memcpy(buffer, &_partial_frame[_partial_position], to_read);
                _partial_position += to_read;
                return to_read;
            }
        }

        std::vector<uint8_t> frame_data{};
        {
            std::unique_lock<std::mutex> l{_frame_lock};
            if (_frame_queue.empty()) {
                _frame_event.wait(l, [this]() { return !_frame_queue.empty(); });
            }

            frame_data = _frame_queue.front();
            _frame_queue.pop_front();
        }

        if (frame_data.size() > num_bytes) {
            _partial_frame = frame_data;
            _partial_position = num_bytes;
        }

        const auto to_read = std::min<std::size_t>(num_bytes, frame_data.size());
        memcpy(buffer, frame_data.data(), to_read);

        return to_read;
    }

    void CameraHandler::handle_camera_frame(const std::vector<uint8_t> &data, std::size_t size) {
        std::lock_guard<std::mutex> l{_frame_lock};
        _frame_queue.emplace_back(data.begin(), data.begin() + size);
        while (_frame_queue.size() > 60) {
            _frame_queue.pop_front();
        }

        _frame_event.notify_all();
        log->info("Received camera frame");
    }

    void CameraHandler::handle_conversion_data(void *data, std::size_t size) {
        std::lock_guard<std::mutex> l{_listener_lock};
        std::deque<std::function<bool(void *, std::size_t)>> listeners{};
        for (const auto &listener : _data_listeners) {
            if (listener(data, size)) {
                listeners.push_back(listener);
            }
        }

        _data_listeners = listeners;
        log->info("Received conversion data");
    }
}