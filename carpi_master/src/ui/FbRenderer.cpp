#include "FbRenderer.hpp"

#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <sys/mman.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <drm/drm.h>

#include <common_utils/error.hpp>

namespace carpi::ui {
    LOGGER_IMPL(FbRenderer);

    FbRenderer::FbRenderer(const std::string &device) {
        const auto ttyfd = open("/dev/tty0", O_RDWR);
        if(ttyfd < 0) {
            log->error("Error opening /dev/tty: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error opening tty"};
        }

        if(ioctl(ttyfd, KDSETMODE, KD_GRAPHICS)) {
            log->error("Error setting /dev/tty into graphics mode: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error setting tty to graphics mode"};
        }

        _device = open(device.c_str(), O_RDWR);
        if (_device < 0) {
            log->error("Error opening DRM device '{}': {} (errno={})", device, utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error opening DRM device"};
        }

        log->info("Opened frame buffer device: {}", device);

        int flags = fcntl(_device, F_GETFD);
        if(flags < 0) {
            log->error("Error getting flags of DRM device: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error opening DRM device"};
        }

        if(fcntl(_device, F_SETFD, flags | FD_CLOEXEC) < 0) {
            log->error("Error setting FD_CLOEXEC flag on DRM device: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error opening DRM device"};
        }

        uint64_t has_dumb_buffer = 0;
        if(drmGetCap(_device, DRM_CAP_DUMB_BUFFER, &has_dumb_buffer) < 0 || has_dumb_buffer == 0) {
            log->error("Error querying DRM_CAP_DUMB_BUFFER flag or dumb buffer not supported: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error opening DRM device"};
        }
    }
}