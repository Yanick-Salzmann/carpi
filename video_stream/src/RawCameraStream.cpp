#include <common_utils/error.hpp>
#include "video_stream/RawCameraStream.hpp"

#include "interface/mmal/util/mmal_default_components.h"
#include "interface/mmal/util/mmal_util_params.h"

#define MMAL_CAMERA_VIDEO_PORT 1
#define MMAL_CAMERA_CAPTURE_PORT 2
#define VIDEO_FRAME_RATE_DEN 1
#define VIDEO_OUTPUT_BUFFERS_NUM 3

namespace carpi::video {
    LOGGER_IMPL(RawCameraStream);

    RawCameraStream::RawCameraStream(std::function<void(const std::vector<uint8_t>&, std::size_t)> data_callback) : _data_callback{std::move(data_callback)} {

    }

    void RawCameraStream::initialize_camera(const CameraConfiguration &configuration) {
        MMAL_COMPONENT_T *camera = nullptr;

        auto status = mmal_component_create(MMAL_COMPONENT_DEFAULT_CAMERA, &camera);
        if (status != MMAL_SUCCESS) {
            log->error("Error creating MMAL camera component: {}", status);
            throw std::runtime_error("Error setting up camera");
        }

        std::shared_ptr<MMAL_COMPONENT_T> camera_component{camera, [](MMAL_COMPONENT_T *cam) { mmal_component_destroy(cam); }};

        if (camera->output_num == 0) {
            log->error("MMAL camera component does not have any output ports");
            throw std::runtime_error("Error setting up camera");
        }

        const auto video_port = camera->output[MMAL_CAMERA_VIDEO_PORT];

        MMAL_PARAMETER_CAMERA_CONFIG_T cam_config{};
        cam_config.hdr.id = MMAL_PARAMETER_CAMERA_CONFIG;
        cam_config.hdr.size = sizeof cam_config;
        cam_config.max_stills_h = VCOS_ALIGN_UP(configuration.height(), 16u);
        cam_config.max_stills_w = VCOS_ALIGN_UP(configuration.width(), 32u);
        cam_config.stills_yuv422 = MMAL_FALSE;
        cam_config.one_shot_stills = MMAL_FALSE;
        cam_config.max_preview_video_w = VCOS_ALIGN_UP(configuration.width(), 32u);
        cam_config.max_preview_video_h = VCOS_ALIGN_UP(configuration.height(), 16u);
        cam_config.num_preview_video_frames = configuration.preview_fps();
        cam_config.stills_capture_circular_buffer_height = 0;
        cam_config.fast_preview_resume = MMAL_FALSE;
        cam_config.use_stc_timestamp = MMAL_PARAM_TIMESTAMP_MODE_RESET_STC;

        status = mmal_port_parameter_set(camera->control, &cam_config.hdr);
        if (status != MMAL_SUCCESS) {
            log->error("Error sending camera parameter config to camera: {}", status);
            throw std::runtime_error("Error setting up camera");
        }

        MMAL_PARAMETER_CHANGE_EVENT_REQUEST_T change_event_request = {
                {MMAL_PARAMETER_CHANGE_EVENT_REQUEST, sizeof(MMAL_PARAMETER_CHANGE_EVENT_REQUEST_T)},
                MMAL_PARAMETER_CAMERA_SETTINGS,
                1
        };

        status = mmal_port_parameter_set(camera->control, &change_event_request.hdr);
        if (status != MMAL_SUCCESS) {
            log->error("Error sending parameter change event to camera: {}", status);
            throw std::runtime_error("Error setting up camera");
        }

        status = mmal_port_enable(camera->control, &RawCameraStream::camera_control_callback);
        if (status != MMAL_SUCCESS) {
            log->error("Error sending camera event callback to camera: {}", status);
            throw std::runtime_error("Error setting up camera");
        }

        auto format = video_port->format;

        format->encoding_variant = map_format(VideoFormat::H264);
        format->encoding = MMAL_ENCODING_OPAQUE;
        format->es->video.width = VCOS_ALIGN_UP(configuration.width(), 32);
        format->es->video.height = VCOS_ALIGN_UP(configuration.height(), 16);
        format->es->video.crop.x = 0;
        format->es->video.crop.y = 0;
        format->es->video.crop.width = format->es->video.width;
        format->es->video.crop.height = format->es->video.height;
        format->es->video.frame_rate.num = configuration.fps();
        format->es->video.frame_rate.den = VIDEO_FRAME_RATE_DEN;

        _actual_width = format->es->video.width;
        _actual_height = format->es->video.height;

        status = mmal_port_format_commit(video_port);
        if (status != MMAL_SUCCESS) {
            log->error("Error setting video format: {}", status);
            throw std::runtime_error("Error setting up camera");
        }

        video_port->userdata = reinterpret_cast<struct MMAL_PORT_USERDATA_T *>(this);

        status = mmal_port_enable(video_port, &RawCameraStream::video_data_callback);
        if (status != MMAL_SUCCESS) {
            log->error("Error sending video event callback to camera: {}", status);
            throw std::runtime_error{"Error setting up camera"};
        }

        video_port->buffer_size = video_port->buffer_size_recommended;
        video_port->buffer_num = video_port->buffer_num_recommended;

        if (video_port->buffer_num < VIDEO_OUTPUT_BUFFERS_NUM) {
            video_port->buffer_num = VIDEO_OUTPUT_BUFFERS_NUM;
        }

        const auto pool = mmal_port_pool_create(video_port, video_port->buffer_num, video_port->buffer_size);
        if (pool == nullptr) {
            log->error("Error creating mmal buffer queue");
            throw std::runtime_error("Error setting up camera");
        }

        _video_pool = std::shared_ptr<MMAL_POOL_T>(pool, [video_port](auto *pool) { mmal_port_pool_destroy(video_port, pool); });

        status = mmal_component_enable(camera);
        if (status != MMAL_SUCCESS) {
            log->error("Error enabling camera component: {})", status);
            throw std::runtime_error{"Error setting up camera"};
        }

        mmal_port_parameter_set_rational(video_port, MMAL_PARAMETER_BRIGHTNESS, MMAL_RATIONAL_T{50, 100});
        mmal_port_parameter_set_uint32(video_port, MMAL_PARAMETER_ISO, 400);
        mmal_port_parameter_set_rational(video_port, MMAL_PARAMETER_SHARPNESS, MMAL_RATIONAL_T{0, 100});
        mmal_port_parameter_set_uint32(video_port, MMAL_PARAMETER_SHUTTER_SPEED, 0);
        mmal_port_parameter_set_rational(video_port, MMAL_PARAMETER_CONTRAST, MMAL_RATIONAL_T{0, 100});

        _camera = camera_component;
        _video_port = video_port;
    }

    uint32_t RawCameraStream::map_format(VideoFormat format) {
        switch (format) {
            default:
                return MMAL_ENCODING_I420;
        }
    }

    void RawCameraStream::camera_control_callback(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer) {
        std::shared_ptr<MMAL_BUFFER_HEADER_T> p{buffer, [](auto *buff) { mmal_buffer_header_release(buff); }};

        if (port->userdata == nullptr) {
            return;
        }

        auto *camera_stream = (RawCameraStream *) port->userdata;
        camera_stream->handle_camera_control(port, buffer);
    }

    void RawCameraStream::handle_camera_control(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer) {
        switch (buffer->cmd) {
            case MMAL_EVENT_ERROR: {
                const auto status = *(MMAL_STATUS_T *) buffer->data;
                log->error("Error event received from camera. No data will be received. Error = {}, errno = {}", utils::error_to_string(status), status);
                break;
            }

            case MMAL_EVENT_EOS: {
                const auto eos_event = (MMAL_EVENT_END_OF_STREAM_T *) buffer->data;
                log->info("Camera stream on port {} of type {} ended", eos_event->port_index, eos_event->port_type);
                break;
            }
        }
    }

    void RawCameraStream::video_data_callback(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer) {
        std::shared_ptr<MMAL_BUFFER_HEADER_T> p{buffer, [](auto *buff) { mmal_buffer_header_release(buff); }};

        if (port->userdata == nullptr) {
            return;
        }

        auto *camera_stream = (RawCameraStream *) port->userdata;
        camera_stream->handle_video_data(port, buffer, p);
    }

    void RawCameraStream::handle_video_data(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer, std::shared_ptr<MMAL_BUFFER_HEADER_T>& buffer_ptr) {
        if (buffer->length <= 0 || _camera == nullptr || _video_port == nullptr) {
            return;
        }

        std::unique_lock<std::mutex> l{_data_read_lock};
        auto status = mmal_buffer_header_mem_lock(buffer);
        if (status != MMAL_SUCCESS) {
            log->warn("Could not lock buffer memory: {} (errno={})", utils::error_to_string(status), status);
            return;
        }

        std::size_t length = buffer->length;
        {
            std::shared_ptr<MMAL_BUFFER_HEADER_T> bp{buffer, [this](auto *buff) { mmal_buffer_header_mem_unlock(buff); }};
            if (_buffer_data.size() != buffer->length) {
                _buffer_data.resize(buffer->length);
            }

            memcpy(_buffer_data.data(), buffer->data + buffer->offset, buffer->length);
        }

        buffer_ptr.reset();

        status = MMAL_SUCCESS;
        const auto new_buffer = mmal_queue_get(_video_pool.get()->queue);
        if (new_buffer != nullptr) {
            status = mmal_port_send_buffer(port, new_buffer);
        }

        if (!new_buffer || status != MMAL_SUCCESS) {
            log->warn("Error sending new buffer to camera");
        }

        _data_callback(_buffer_data, length);
    }

    bool RawCameraStream::start_capture() {
        if (mmal_port_parameter_set_boolean(_video_port, MMAL_PARAMETER_CAPTURE, 1) != MMAL_SUCCESS) {
            log->warn("Error setting MMAL_PARAMETER_CAPTURE to true");
            return false;
        }

        const auto queue_size = mmal_queue_length(_video_pool.get()->queue);
        for (auto i = 0u; i < queue_size; ++i) {
            auto buffer = mmal_queue_get(_video_pool.get()->queue);
            if (!buffer) {
                log->warn("There was an error getting a buffer from the queue");
            } else {
                if (mmal_port_send_buffer(_video_port, buffer) != MMAL_SUCCESS) {
                    log->warn("There was an error sending a buffer to the video port");
                }
            }
        }

        return true;
    }

    void RawCameraStream::stop_capture() {
        if (mmal_port_parameter_set_boolean(_video_port, MMAL_PARAMETER_CAPTURE, 0) != MMAL_SUCCESS) {
            log->warn("Error setting MMAL_PARAMETER_CAPTURE to false");
        }
    }

    uint32_t RawCameraStream::calculate_width(uint32_t width) {
        return VCOS_ALIGN_UP(width, 32u);
    }

    uint32_t RawCameraStream::calculate_height(uint32_t height) {
        return VCOS_ALIGN_UP(height, 16u);
    }
}