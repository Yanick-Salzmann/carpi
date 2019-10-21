#include <common_utils/error.hpp>
#include "RawCameraStream.hpp"

#include "interface/mmal/util/mmal_default_components.h"

#define MMAL_CAMERA_VIDEO_PORT 1
#define MMAL_CAMERA_CAPTURE_PORT 2
#define VIDEO_FRAME_RATE_DEN 1
#define VIDEO_OUTPUT_BUFFERS_NUM 3

namespace carpi::camera {
    LOGGER_IMPL(RawCameraStream);

    void RawCameraStream::initialize_camera(const CameraConfiguration &configuration) {
        MMAL_COMPONENT_T *camera = nullptr;

        auto status = mmal_component_create(MMAL_COMPONENT_DEFAULT_CAMERA, &camera);
        if (status != MMAL_SUCCESS) {
            log->error("Error creating MMAL camera component: {} (error={})", utils::error_to_string(status), status);
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
        cam_config.max_stills_h = configuration.height();
        cam_config.max_stills_w = configuration.width();
        cam_config.stills_yuv422 = MMAL_FALSE;
        cam_config.one_shot_stills = MMAL_FALSE;
        cam_config.max_preview_video_w = configuration.width();
        cam_config.max_preview_video_h = configuration.height();
        cam_config.num_preview_video_frames = configuration.preview_fps();
        cam_config.stills_capture_circular_buffer_height = 0;
        cam_config.fast_preview_resume = MMAL_FALSE;
        cam_config.use_stc_timestamp = MMAL_PARAM_TIMESTAMP_MODE_RESET_STC;

        status = mmal_port_parameter_set(camera->control, &cam_config.hdr);
        if (status != MMAL_SUCCESS) {
            log->error("Error sending camera parameter config to camera: {} (errno={})", utils::error_to_string(status), status);
            throw std::runtime_error("Error setting up camera");
        }

        MMAL_PARAMETER_CHANGE_EVENT_REQUEST_T change_event_request = {
                {MMAL_PARAMETER_CHANGE_EVENT_REQUEST, sizeof(MMAL_PARAMETER_CHANGE_EVENT_REQUEST_T)},
                MMAL_PARAMETER_CAMERA_SETTINGS,
                1
        };

        status = mmal_port_parameter_set(camera->control, &change_event_request.hdr);
        if (status != MMAL_SUCCESS) {
            log->error("Error sending parameter change event to camera: {} (errno={})", utils::error_to_string(status), status);
            throw std::runtime_error("Error setting up camera");
        }

        status = mmal_port_enable(camera->control, &RawCameraStream::camera_control_callback);
        if (status != MMAL_SUCCESS) {
            log->error("Error sending camera event callback to camera: {} (errno={})", utils::error_to_string(status), status);
            throw std::runtime_error("Error setting up camera");
        }

        auto format = video_port->format;

        format->es->video.width = VCOS_ALIGN_UP(configuration.width(), 32u);
        format->es->video.height = VCOS_ALIGN_UP(configuration.height(), 16u);
        format->es->video.crop.x = 0;
        format->es->video.crop.y = 0;
        format->es->video.crop.width = format->es->video.width;
        format->es->video.crop.height = format->es->video.height;
        format->es->video.frame_rate.num = configuration.fps();
        format->es->video.frame_rate.den = VIDEO_FRAME_RATE_DEN;

        status = mmal_port_format_commit(video_port);
        if (status != MMAL_SUCCESS) {
            log->error("Error setting video format: {} (error={})", utils::error_to_string(status), status);
            throw std::runtime_error("Error setting up camera");
        }

        video_port->userdata = reinterpret_cast<struct MMAL_PORT_USERDATA_T *>(this);

        video_port->buffer_size = video_port->buffer_size_recommended;
        video_port->buffer_num = video_port->buffer_num_recommended;

        const auto pool = mmal_port_pool_create(video_port, video_port->buffer_num, video_port->buffer_size);
        if(pool == nullptr) {
            log->error("Error creating mmal buffer queue");
            throw std::runtime_error("Error setting up camera");
        }

        _video_pool = std::shared_ptr<MMAL_POOL_T>(pool, [video_port](auto* pool) { mmal_port_pool_destroy(video_port, pool); });

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
        camera_stream->handle_video_data(port, buffer);
    }

    void RawCameraStream::handle_video_data(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer) {
        if (buffer->length <= 0 || _camera == nullptr || _video_port == nullptr) {
            return;
        }

        std::unique_lock<std::mutex> l{_data_read_lock};
        auto status = mmal_buffer_header_mem_lock(buffer);
        if (status != MMAL_SUCCESS) {
            log->warn("Could not lock buffer memory: {} (errno={})", utils::error_to_string(status), status);
            return;
        }

        std::shared_ptr<MMAL_BUFFER_HEADER_T> bp{buffer, [](auto *buff) { mmal_buffer_header_mem_unlock(buff); }};
        if (_buffer_data.size() < buffer->length) {
            _buffer_data.resize(buffer->length);
        }

        memcpy(_buffer_data.data(), buffer->data, buffer->length);

        status = MMAL_SUCCESS;
        const auto new_buffer = mmal_queue_get(_video_pool.get()->queue);
        if(new_buffer != nullptr) {
            status = mmal_port_send_buffer(port, new_buffer);
        }

        if(!new_buffer || status != MMAL_SUCCESS) {
            log->warn("Error sending new buffer to camera");
        }

        _data_variable.notify_all();
    }
}