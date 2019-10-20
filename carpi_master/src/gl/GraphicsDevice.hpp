#ifndef CARPI_CARPI_GRAPHICSDEVICE_HPP
#define CARPI_CARPI_GRAPHICSDEVICE_HPP

#include <cstdint>
#include <memory>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <common_utils/log.hpp>

namespace carpi::gl {
    class GraphicsDevice {
        LOGGER;

        typedef struct {
            unsigned int element;
            uint32_t width;
            uint32_t height;
        } EGL_DISPMANX_WINDOW_T;

        EGLContext _context;
        EGLDisplay _display;
        EGLSurface _surface;
        std::shared_ptr<EGL_DISPMANX_WINDOW_T> _native_window;

        uint32_t _width = 0;
        uint32_t _height = 0;

        void dispmanx_init();

        void egl_initialize();

        void context_setup();

    public:
        void initialize();

        void begin_frame();

        void end_frame();

        [[nodiscard]] uint32_t width() const { return _width; }

        [[nodiscard]] uint32_t height() const { return _height; }
    };
}

#endif //CARPI_CARPI_GRAPHICSDEVICE_HPP
