set(INCLUDE_FOLDER "${PROJECT_SOURCE_DIR}/include")
set(SOURCE_FOLDER "${PROJECT_SOURCE_DIR}/src")

file(GLOB ROOT_HEADER "${INCLUDE_FOLDER}/ada/*.h")
file(GLOB GL_HEADER "${INCLUDE_FOLDER}/ada/gl/*.h")
file(GLOB TOOLS_HEADER "${INCLUDE_FOLDER}/ada/tools/*.h")
file(GLOB SHADERS_HEADER "${INCLUDE_FOLDER}/ada/shaders/*.h")

set(ADA_SOURCES
    window.cpp
    gl/gl.cpp 
    gl/fbo.cpp
    gl/vbo.cpp 
    gl/mesh.cpp 
    gl/shader.cpp
    gl/defines.cpp 
    gl/pingpong.cpp
    gl/texture.cpp 
    gl/textureBump.cpp
    gl/textureCube.cpp 
    gl/textureProps.cpp 
    gl/textureStreamAV.cpp
    gl/textureStreamOMX.cpp
    gl/textureStreamMMAL.cpp
    gl/textureStreamSequence.cpp
    gl/convolutionPyramid.cpp
    gl/vertexLayout.cpp 
    tools/fs.cpp 
    tools/env.cpp
    tools/list.cpp
    tools/pixels.cpp 
    tools/geom.cpp
    tools/text.cpp
    tools/time.cpp
    tools/font.cpp
    tools/holoplay.cpp 
    scene/node.cpp
    scene/model.cpp
    scene/light.cpp
    scene/camera.cpp
    scene/material.cpp
    shaders/defaultShaders.cpp
)

# C++ LIBRAY
#
add_library(ada ${ADA_SOURCES})

set_target_properties(ada PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)

find_package(BROADCOM)
if (BROADCOM_FOUND)
    message(STATUS "BROADCOM_DEFINITIONS:   ${BROADCOM_DEFINITIONS}")
    message(STATUS "BROADCOM_INCLUDE_DIRS:  ${BROADCOM_INCLUDE_DIRS}")
    message(STATUS "BROADCOM_LIBRARIES:     ${BROADCOM_LIBRARIES}")
    target_include_directories(ada PUBLIC   ${BROADCOM_INCLUDE_DIRS})
    target_link_libraries(ada PUBLIC        ${BROADCOM_LIBRARIES})
    target_compile_definitions(ada PUBLIC   ${BROADCOM_DEFINITIONS})
endif()

if (BROADCOM_INCLUDE_DIR)
message(STATUS "PLATFORM RASPBERRYPI")
target_compile_definitions(ada PUBLIC       PLATFORM_RPI)
endif()
    
find_package(ILCLIENT)
if (ILCLIENT_FOUND)
    target_include_directories(ada PUBLIC   ${ILCLIENT_INCLUDE_DIR} )
    target_link_libraries(ada PUBLIC        ${ILCLIENT_LIBRARY} )
endif()      

find_package(OMAX)
if (OMAX_FOUND AND ILCLIENT_FOUND)
    target_compile_definitions(ada PUBLIC   ${OMAX_DEFINITIONS})
    target_link_libraries(ada PUBLIC        ${OMAX_LIBRARIES})
    target_compile_definitions(ada PUBLIC   SUPPORT_OMAX)
endif()

find_package(MMAL)
if (MMAL_FOUND)
    target_link_libraries(ada PUBLIC        ${MMAL_LIBRARIES})
    target_compile_definitions(ada PUBLIC   SUPPORT_MMAL)
endif()

find_package(FFMPEG)
if (FFMPEG_FOUND)
    message(STATUS "LIBAV Supported")
    target_include_directories(ada PUBLIC ${FFMPEG_INCLUDE_DIRS})
    target_link_libraries(ada PUBLIC ${FFMPEG_LIBRARIES})
    target_compile_definitions(ada PUBLIC SUPPORT_LIBAV )
endif()

if (NO_X11 OR FORCE_GBM)
    if (BROADCOM_FOUND AND NOT FORCE_GBM)
        message(STATUS "USING BROADCOM DRIVERS")        
        target_compile_definitions(ada PUBLIC   DRIVER_BROADCOM )

        target_link_libraries(ada PUBLIC        
            ${BCM_EGL} 
            ${BCM_GLESv2}
        )

    else()
        message(STATUS "USING DRM and GBM DRIVERS")
        target_compile_definitions(ada PUBLIC   DRIVER_GBM)
        find_package(DRM)
        find_package(GBM)
        target_include_directories(ada PUBLIC
            ${DRM_INCLUDE_DIRS}
            ${GBM_INCLUDE_DIRS}
        )
        target_link_libraries(ada PUBLIC
            ${DRM_LIBRARIES}
            ${GBM_LIBRARY}
        )

        find_package(EGL)
        find_package(GLESv2)
        message(STATUS "EGL_INCLUDE_DIRS:       ${EGL_INCLUDE_DIRS}")
        message(STATUS "EGL_LIBRARIES:          ${EGL_LIBRARIES}")
        message(STATUS "GLESv2_INCLUDE_DIRS:    ${GLESv2_INCLUDE_DIRS}")
        message(STATUS "GLESv2_LIBRARIES:       ${GLESv2_LIBRARIES}")
    
        target_include_directories(ada PUBLIC
            ${EGL_INCLUDE_DIRS}
            ${GLESv2_INCLUDE_DIRS}
        )
    
        target_link_libraries(ada PUBLIC
            ${EGL_LIBRARIES}
            ${GLESv2_LIBRARIES}
        )
    endif()

else()
    message(STATUS "USING GLFW DRIVERS")

    target_compile_definitions(ada PUBLIC 
        DRIVER_GLFW 
        GL_OPENGL
    )

    target_link_libraries(ada PUBLIC 
        glfw
        ${GLFW_LIBRARIES}
    )

    # OPENGL
    if (APPLE)
        message(STATUS "PLATFORM OSX")
        target_compile_definitions(ada PUBLIC   PLATFORM_OSX)
        target_link_libraries(ada PUBLIC 
            "-framework OpenGL"
        )

    elseif (MSVC)
        message(STATUS "PLATFORM WINDOWS")
        target_compile_definitions(ada PUBLIC   PLATFORM_WINDOWS)
        target_include_directories(ada PUBLIC 
            "${PROJECT_SOURCE_DIR}/deps/glew/include"
        )

        target_link_libraries(ada PUBLIC 
            libglew_static
        )

    # RASPBERRY PI
    elseif (BROADCOM_INCLUDE_DIR)
        find_package(EGL)
        find_package(GLESv2)
        message(STATUS "EGL_INCLUDE_DIRS:       ${EGL_INCLUDE_DIRS}")
        message(STATUS "EGL_LIBRARIES:          ${EGL_LIBRARIES}")
        message(STATUS "GLESv2_INCLUDE_DIRS:    ${GLESv2_INCLUDE_DIRS}")
        message(STATUS "GLESv2_LIBRARIES:       ${GLESv2_LIBRARIES}")
    
        target_include_directories(ada PUBLIC
            ${EGL_INCLUDE_DIRS}
            ${GLESv2_INCLUDE_DIRS}
        )
    
        target_link_libraries(ada PUBLIC
            ${EGL_LIBRARIES}
            ${GLESv2_LIBRARIES}
            ${OPENGL_LIBRARIES}
        )

    elseif (EMSCRIPTEN)
        message(STATUS "PLATFORM EMSCRIPTEN")
        target_compile_definitions(ada PUBLIC   PLATFORM_EMSCRIPTEN)
        target_link_libraries(ada PUBLIC 
            ${OPENGL_LIBRARIES}
        )

    #   UNCOMMENT FOR MULTIPLE THREAD SUPPORT FOR WASM
    #   note: only works on desktop chrome 
        # set(LFLAGS "${LFLAGS} -pthread")
        # set(LFLAGS "${LFLAGS} -s USE_PTHREADS=1")
        # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LFLAGS}")
        # set_target_properties(ada PROPERTIES LINK_FLAGS "${LFLAGS}")
        # target_link_libraries(ada PUBLIC -pthread )

    else()
        message(STATUS "PLATFORM LINUX")
        target_compile_definitions(ada PUBLIC   PLATFORM_LINUX)

        set(OpenGL_GL_PREFERENCE GLVND)
        find_package(OpenGL REQUIRED)
        target_link_libraries(ada PUBLIC 
            OpenGL::GL
        )

    endif()

endif()

# We need this directory, and users of our library will need it too
target_include_directories(ada PUBLIC 
    ${INCLUDE_FOLDER}
    "${PROJECT_SOURCE_DIR}/deps/glm"
    "${PROJECT_SOURCE_DIR}/deps/stb"
    "${PROJECT_SOURCE_DIR}/deps/glob"
    "${PROJECT_SOURCE_DIR}/deps/skymodel"
    "${PROJECT_SOURCE_DIR}/deps/fontstash"
)

## DEPS
target_link_libraries(ada PUBLIC stb glob skymodel )

# IDEs should put the headers in a nice place
# source_group(   TREE ${INCLUDE_FOLDER} 
#                 PREFIX "Header Files" FILES ${ROOT_HEADER})

# install(TARGETS ${PROJECT_NAME} DESTINATION lib)
# install(FILES ${ROOT_HEADER} DESTINATION include/ada)
# install(FILES ${GL_HEADER} DESTINATION include/ada/gl)
# install(FILES ${TOOLS_HEADER} DESTINATION include/ada/tools)
# install(FILES ${SHADERS_SOURCES} DESTINATION include/ada/shaders)

