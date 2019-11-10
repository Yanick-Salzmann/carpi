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

    int msleep(long msec)
    {
        struct timespec ts;
        int res;

        if (msec < 0)
        {
            errno = EINVAL;
            return -1;
        }

        ts.tv_sec = msec / 1000;
        ts.tv_nsec = (msec % 1000) * 1000000;

        do {
            res = nanosleep(&ts, &ts);
        } while (res && errno == EINTR);

        return res;
    }

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

        log->info("Frame buffer size: {}x{}, BPP: {}, Line Size: {}", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel, finfo.line_length);

        void *fb_addr = mmap(nullptr, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, _device, 0);
        log->info("Mapped frame buffer to {}", fb_addr);

        uint32_t offsetx = 0, offsety = 0;
        log->info("{}", sizeof(RGB565));
        std::vector<RGB565> fbuffer(vinfo.yres * vinfo.xres);

        while(true) {
            //RGB565 fbuffer[480 * 320]{};
            for (auto i = 0; i < vinfo.xres; ++i) {
                for (auto j = 0; j < vinfo.yres; ++j) {
                    fbuffer[j * vinfo.xres + i].r = (uint16_t) ((((i + offsetx) % vinfo.xres) / (float) vinfo.xres) * 32.0f);
                    fbuffer[j * vinfo.xres + i].b = (uint16_t) ((((j + offsety) % vinfo.yres) / (float) vinfo.yres) * 32.0f);
                }
            }

            memcpy(fb_addr, fbuffer.data(), fbuffer.size() * sizeof(RGB565));
            msleep(16);
            offsetx = (offsetx + 1) % vinfo.xres;
            offsety = (offsety + 1) % vinfo.yres;
        }
    }
}