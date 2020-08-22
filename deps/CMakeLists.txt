## GLAD
# add_subdirectory(glad)

## GLEW
# add_library(glee glee/GLee.c)

## GLFW
# Configure GLFW to build only the library.
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")
add_subdirectory(glfw)
# Disable noisy deprecation warnings within CMake for macOS.
if(APPLE)
    target_compile_options(glfw PRIVATE "-Wno-deprecated-declarations")
endif()

## OpenGL
if(NOT DEFINED OpenGL_GL_PREFERENCE)
    # set(OpenGL_GL_PREFERENCE "LEGACY")
    set(OPENGL_GL_PREFERENCE "GLVND")
endif()
set(OpenGL_GL_PREFERENCE ${OPENGL_GL_PREFERENCE})
find_package(OpenGL REQUIRED)

## SkyModel
add_subdirectory(skymodel)