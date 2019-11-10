#include "FbRenderer.hpp"

#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>

#include <common_utils/error.hpp>

namespace carpi::ui {
    LOGGER_IMPL(FbRenderer);

    FbRenderer::FbRenderer(const std::string &device) {
        _device = open(device.c_str(), O_RDWR);
        if(_device < 0) {
            log->error("Error opening frame buffer device '{}': {} (errno={})", device, utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error opening frame buffer"};
        }

        log->info("Opened frame buffer device: {}", device);

        fb_var_screeninfo vinfo{};
        fb_fix_screeninfo finfo{};

        if(ioctl(_device, FBIOGET_FSCREENINFO, &finfo)) {
            log->error("Error getting fixed display information: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error getting display information"};
        }

        if(ioctl(_device, FBIOGET_VSCREENINFO, &vinfo)) {
            log->error("Error getting variable display information: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error getting display information"};
        }

        log->info("Frame buffer info: {}x{}@{}", vinfo.xres, vinfo.yres, vinfo.sync);
    }
}