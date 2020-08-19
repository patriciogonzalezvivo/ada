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
    target_compile_options(glfw PUBLIC "-Wno-deprecated-declarations")
endif()

# OpenGL
if (APPLE)
    SET(CMAKE_CXX_FLAGS "-std=c++11")
    set(CMAKE_FIND_FRAMEWORK ONLY)
    find_package(COCOA REQUIRED)
    find_package(CoreVideo REQUIRED)
    find_package(IOKit REQUIRED)
    find_package(OpenGL)

    include_directories(${COCOA_INCLUDE_DIRS})
    include_directories(${CoreVideo_INCLUDE_DIRS})
    include_directories(${IOKit_INCLUDE_DIRS})
    set(CMAKE_EXE_LINKER_FLAGS "-framework OpenGL")
else()
    find_package(OpenGL REQUIRED)
endif (APPLE)

find_package(OpenGL)
