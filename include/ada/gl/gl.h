#pragma once

// Default for RASPBERRYPI
#if defined(PLATFORM_RPI)

#include "bcm_host.h"
#undef countof

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

// MACOS
#elif defined(__APPLE__)
#define GL_PROGRAM_BINARY_LENGTH 0x8741
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>

#elif defined(_WIN32)
#include <GL/glew.h>
#define APIENTRY __stdcall

#elif defined(__EMSCRIPTEN__)
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#else

// ANY LINUX using GLFW 
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#endif
