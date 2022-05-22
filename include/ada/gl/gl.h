#pragma once

// Default for RASPBERRYPI
#if defined(PLATFORM_RPI)

    #include "bcm_host.h"
    #undef countof

    // RASPBERRY PI - WINDOWLESS using GBM (RPi 4 )
    #if defined(DRIVER_GBM)
        #include <xf86drm.h>
        #include <xf86drmMode.h>
        #include <gbm.h>
    #endif

    #if defined(DRIVER_GLFW)
        #define GLFW_INCLUDE_GLEXT
        #define GLFW_EXPOSE_NATIVE_EGL
        #define GL_GLEXT_PROTOTYPES
        #define GLFW_INCLUDE_ES2
        #define GLFW_INCLUDE_GLCOREARB
        #include <GLFW/glfw3.h>
        #include <GLFW/glfw3native.h>
        #include <EGL/eglext.h>

    // RASPBERRY PI - WINDOWLESS (BROADCOM or GBM)  
    #else
        #include <EGL/egl.h>
        #include <EGL/eglext.h>
        #include <GLES2/gl2.h>
        #include <GLES2/gl2ext.h>

    #endif

    namespace ada {
    EGLImageKHR createImage(EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attrib_list);
    EGLBoolean destroyImage(EGLDisplay dpy, EGLImageKHR image);
    void imageTargetTexture2D(EGLenum target, EGLImageKHR image);
    }

#elif defined(DRIVER_GBM)

    // #include "bcm_host.h"
    // #undef countof

    #include <xf86drm.h>
    #include <xf86drmMode.h>
    #include <gbm.h>

    #include <EGL/egl.h>
    #include <EGL/eglext.h>
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>

    namespace ada {
    EGLImageKHR createImage(EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attrib_list);
    EGLBoolean destroyImage(EGLDisplay dpy, EGLImageKHR image);
    void imageTargetTexture2D(EGLenum target, EGLImageKHR image);
    }

// MACOS
#elif defined(__APPLE__)
    #define GL_PROGRAM_BINARY_LENGTH 0x8741
    #define GL_SILENCE_DEPRECATION
    #include <OpenGL/gl.h>
    #include <OpenGL/glext.h>

// WINDOWS
#elif defined(_WIN32)
    #include <GL/glew.h>
    #define APIENTRY __stdcall

// EMSCRIPTEN WASM (web assembly)
#elif defined(__EMSCRIPTEN__)
    #include <emscripten.h>
    #define GL_GLEXT_PROTOTYPES
    #define EGL_EGLEXT_PROTOTYPES
    #include <GLFW/glfw3.h>

// LINUX
#else
    #define GL_GLEXT_PROTOTYPES
    #include <GLFW/glfw3.h>

#endif


namespace ada {

enum BlendMode {
    BLEND_NONE = 0,
    BLEND_ALPHA,
    BLEND_ADD,
    BLEND_MULTIPLY,
    BLEND_SCREEN,
    BLEND_SUBSTRACT
};

enum CullingMode {
    CULL_NONE = 0,
    CULL_FRONT,
    CULL_BACK,
    CULL_BOTH
};

void blendMode( BlendMode _mode );
void cullingMode( CullingMode _mode );

};