
    find_path(OPENGLES2_INCLUDE_DIR GLES2/gl2.h
            PATHS /usr/openwin/share/include
            /opt/graphics/OpenGL/include
            /opt/vc/include
            /usr/X11R6/include
            /usr/include
            )

    find_library(OPENGLES2_LIBRARY
            NAMES GLESv2 GLESv2_static
            PATHS /opt/graphics/OpenGL/lib
            /usr/openwin/lib
            /usr/shlib /usr/X11R6/lib
            /opt/vc/lib
            /usr/lib/aarch64-linux-gnu
            /usr/lib/arm-linux-gnueabihf
            /usr/lib
            )

    if (NOT BUILD_ANDROID)
        find_path(EGL_INCLUDE_DIR EGL/egl.h
                PATHS /usr/openwin/share/include
                /opt/graphics/OpenGL/include
                /opt/vc/include
                /usr/X11R6/include
                /usr/include
                )

        find_library(EGL_LIBRARY
                NAMES EGL EGL_static
                PATHS /opt/graphics/OpenGL/lib
                /usr/openwin/lib
                /usr/shlib
                /usr/X11R6/lib
                /opt/vc/lib
                /usr/lib/aarch64-linux-gnu
                /usr/lib/arm-linux-gnueabihf
                /usr/lib
                )
    endif ()

    set(OPENGLES2_LIBRARIES ${OPENGLES2_LIBRARY})

    if (OPENGLES2_LIBRARY AND EGL_LIBRARY)
        set(EGL_LIBRARIES ${EGL_LIBRARY})
        set(OPENGLES2_FOUND TRUE)
    endif ()

    mark_as_advanced(
            OPENGLES2_INCLUDE_DIR
            OPENGLES2_LIBRARY
            EGL_INCLUDE_DIR
            EGL_LIBRARY
    )

    if (OPENGLES2_FOUND)
        message(STATUS "Found system OpenGL ES 2 library: ${OPENGLES2_LIBRARIES}")
        message(STATUS "Found system EGL library: ${EGL_LIBRARIES}")
    else ()
        set(OPENGLES2_LIBRARIES "")
    endif ()