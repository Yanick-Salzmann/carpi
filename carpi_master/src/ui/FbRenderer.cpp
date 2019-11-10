#include "FbRenderer.hpp"

#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <sys/mman.h>

#include <common_utils/error.hpp>

namespace carpi::ui {
    LOGGER_IMPL(FbRenderer);

#pragma pack(push, 1)
    union RGB565 {
        uint16_t value;
        struct {
            uint16_t r : 5;
            uint16_t g : 6;
            uint16_t b : 5;
        };
    };
#pragma pack(pop)

    FbRenderer::FbRenderer(const std::string &device) {
        const auto ttyfd = open("/dev/tty", O_RDWR);
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
            log->error("Error opening frame buffer device '{}': {} (errno={})", device, utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error opening frame buffer"};
        }

        log->info("Opened frame buffer device: {}", device);

        fb_var_screeninfo vinfo{};
        fb_fix_screeninfo finfo{};

        if (ioctl(_device, FBIOGET_FSCREENINFO, &finfo)) {
            log->error("Error getting fixed display information: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error getting display information"};
        }

        if (ioctl(_device, FBIOGET_VSCREENINFO, &vinfo)) {
            log->error("Error getting variable display information: {} (errno={})", utils::error_to_string(errno), errno);
            throw std::runtime_error{"Error getting display information"};
        }

        log->info("Frame buffer size: {}x{}, BPP: {}", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

        void *fb_addr = mmap(nullptr, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, _device, 0);
        log->info("Mapped frame buffer to {}", fb_addr);

        RGB565 fbuffer[480 * 320]{};
        for (auto i = 0; i < 480; ++i) {
            for (auto j = 0; j < 320; ++j) {
                fbuffer[j * 480 + i].r = (uint16_t) ((i / 480.0f) * 32.0f);
                fbuffer[j * 480 + i].b = (uint16_t) ((j / 320.0f) * 32.0f);
            }
        }

        memcpy(fb_addr, fbuffer, sizeof fbuffer);
    }
}