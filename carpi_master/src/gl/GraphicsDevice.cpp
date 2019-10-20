#include "GraphicsDevice.hpp"

#include <cstring>
#include <bcm_host.h>
#include <EGL/egl.h>

static const EGLint attribute_list[] =
        {
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
                EGL_ALPHA_SIZE, 8,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                EGL_NONE
        };

namespace carpi::gl {
    LOGGER_IMPL(GraphicsDevice);

    void GraphicsDevice::initialize() {
        graphics_get_display_size(0, &_width, &_height);
        dispmanx_init();
        egl_initialize();
        context_setup();
    }

    void GraphicsDevice::begin_frame() {
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    }

    void GraphicsDevice::end_frame() {
        eglSwapBuffers(_display, _surface);
    }

    void GraphicsDevice::dispmanx_init() {
        auto vc_disp = vc_dispmanx_display_open(0);
        auto update = vc_dispmanx_update_start(0);

        VC_RECT_T dst_rect{0, 0, static_cast<int32_t>(_width), static_cast<int32_t>(_height)};
        VC_RECT_T src_rect{0, 0, static_cast<int32_t>(_width << 16u), static_cast<int32_t>(_height << 16u)};

        auto element = vc_dispmanx_element_add(update, vc_disp, 0, &dst_rect, 0, &src_rect, DISPMANX_PROTECTION_NONE,
                                               nullptr, nullptr, DISPMANX_NO_ROTATE);

        _native_window = std::make_shared<EGL_DISPMANX_WINDOW_T>();
        memset(_native_window.get(), 0, sizeof(EGL_DISPMANX_WINDOW_T));
        _native_window->element = element;
        _native_window->width = _width;
        _native_window->height = _height;

        vc_dispmanx_update_submit_sync(update);
    }

    void GraphicsDevice::egl_initialize() {
        _display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

        int32_t major = 0, minor = 0;
        eglInitialize(_display, &major, &minor);

        EGLint num_config;
        EGLConfig config;
        eglChooseConfig(_display, attribute_list, &config, 1, &num_config);

        EGLint context_attribs[] = {
                EGL_CONTEXT_CLIENT_VERSION, 2,
                EGL_NONE
        };

        _context = eglCreateContext(_display, config, EGL_NO_CONTEXT, context_attribs);
        _surface = eglCreateWindowSurface(_display, config, (EGLNativeWindowType) _native_window.get(), nullptr);
        eglMakeCurrent(_display, _surface, _surface, _context);

        log->info("Graphics loaded for renderer: %s", glGetString(GL_RENDERER));
        eglBindAPI(EGL_OPENGL_API);
    }

    void GraphicsDevice::context_setup() {
        glClearColor(1, 0.5f, 0.25f, 1.0f);
        glViewport(0, 0, _width, _height);
        glDisable(GL_CULL_FACE);
    }
}