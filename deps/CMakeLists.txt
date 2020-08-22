## GLFW

if (NO_GLFW)
    message(STATUS "LOADING GBM EGL DRM GL_ESv2")

    find_package(GBM)
    find_package(EGL)
    find_package(DRM)
    find_package(GLESv2)

    message(STATUS "DRM_INCLUDE_DIRS: ${DRM_INCLUDE_DIRS}")
    message(STATUS "GBM_INCLUDE_DIRS: ${GBM_INCLUDE_DIRS}")
    message(STATUS "EGL_INCLUDE_DIRS: ${EGL_INCLUDE_DIRS}")
    message(STATUS "GLESv2_INCLUDE_DIRS: ${GLESv2_INCLUDE_DIRS}")

else()
    message(STATUS "LOADING GLFW OPENGL")

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

    message(STATUS "OPENGL_LIBRARIES: ${OPENGL_LIBRARIES}")
    message(STATUS "OPENGL_opengl_LIBRARY: ${OPENGL_opengl_LIBRARY}")
endif()


## SkyModel
add_subdirectory(skymodel)