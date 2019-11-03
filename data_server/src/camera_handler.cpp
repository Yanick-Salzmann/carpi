#include <future>
#include "camera_handler.hpp"

namespace carpi::data {
    LOGGER_IMPL(CameraHandler);

    void CameraHandler::begin_streaming(const std::function<bool(void *, std::size_t)> &data_callback) {
        initialize_camera();

        std::shared_ptr <ReaderContext> context = std::make_shared<ReaderContext>();
        {
            std::lock_guard <std::mutex> l{_listener_lock};
            _data_listeners.emplace(context);
        }

        context->callback = data_callback;
        context->last_read_index = _write_index;
        context->converter = std::make_shared<video::H264Conversion>(
                std::make_shared<video::H264Stream>(std::make_shared<StreamSource>(*this, context.get()), 1920, 1080, 30),
                "mp4",
                [this, context](void *data, std::size_t size) { handle_conversion_data(context, data, size); },
                []() {}
        );
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

    size_t CameraHandler::read(ReaderContext* context, void *buffer, std::size_t num_bytes) {
        if (!context->partial_frame.empty()) {
            if (context->partial_position < context->partial_frame.size()) {
                const auto to_read = std::min<std::size_t>(num_bytes, context->partial_frame.size() - context->partial_position);
                memcpy(buffer, &context->partial_frame[context->partial_position], to_read);
                context->partial_position += to_read;
                return to_read;
            }
        }

        std::vector<uint8_t> frame_data{};
        {
            std::unique_lock<std::mutex> l{_frame_lock};
            if (context->last_read_index == _write_index) {
                _frame_event.wait(l, [this, context]() { return context->last_read_index != _write_index; });
            }

            frame_data = _frame_queue[context->last_read_index++];
            context->last_read_index %= QUEUE_SIZE;
        }

        if (frame_data.size() > num_bytes) {
            context->partial_frame = frame_data;
            context->partial_position = num_bytes;
        }

        const auto to_read = std::min<std::size_t>(num_bytes, frame_data.size());
        memcpy(buffer, frame_data.data(), to_read);

        return to_read;
    }

    void CameraHandler::handle_camera_frame(const std::vector<uint8_t> &data, std::size_t size) {
        std::lock_guard<std::mutex> l{_frame_lock};
        auto next_index = ++_write_index;
        next_index %= QUEUE_SIZE;
        _write_index = next_index;

        _frame_queue[next_index].assign(data.begin(), data.begin() + size);
        _frame_event.notify_all();
    }

    void CameraHandler::handle_conversion_data(const std::shared_ptr<ReaderContext> &context, void *data, std::size_t size) {
        if (context->callback(data, size)) {
            return;
        }

        std::lock_guard<std::mutex> l{_listener_lock};
        _data_listeners.erase(context);
    }
}