## SkyModel
add_subdirectory(skymodel)

## GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)

# Disable noisy deprecation warnings within CMake for macOS.
if (APPLE)
    target_compile_options(glfw PRIVATE "-Wno-deprecated-declarations")
endif()

# OpenGL
find_package(OpenGL REQUIRED)
