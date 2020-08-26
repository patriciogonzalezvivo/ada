#pragma once

// Default for RASPBERRYPI
#if defined(GL_ES)

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
#include <GLFW/glfw3.h>
#define APIENTRY __stdcall
#else

// ANY LINUX using GLFW 
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#endif
