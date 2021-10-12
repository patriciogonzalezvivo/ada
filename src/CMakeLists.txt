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
    tools/list.cpp
    tools/pixels.cpp 
    tools/geom.cpp
    tools/text.cpp
    tools/time.cpp
    scene/node.cpp
    scene/model.cpp
    scene/light.cpp
    scene/camera.cpp
    scene/material.cpp
    shaders/defaultShaders.cpp
)

# file(GLOB ROOT_SOURCES  "${SOURCE_FOLDER}/*.cpp")
# file(GLOB GL_SOURCES  "${SOURCE_FOLDER}/gl/*.cpp")
# file(GLOB TOOLS_SOURCES  "${SOURCE_FOLDER}/tools/*.cpp")
# file(GLOB SHADERS_SOURCES  "${SOURCE_FOLDER}/shaders/*.cpp")
# set(ADA_SOURCES ${ROOT_SOURCES} ${GL_SOURCES} ${TOOLS_SOURCES} ${SHADERS_SOURCES})

# C++ LIBRAY
#
add_library(ada ${ADA_SOURCES})

set_target_properties(ada PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)

find_package(BROADCOM)
if (BROADCOM_FOUND)
    target_compile_definitions(ada PUBLIC   PLATFORM_RPI)
    message(STATUS "BROADCOM_DEFINITIONS:   ${BROADCOM_DEFINITIONS}")
    message(STATUS "BROADCOM_INCLUDE_DIRS:  ${BROADCOM_INCLUDE_DIRS}")
    message(STATUS "BROADCOM_LIBRARIES:     ${BROADCOM_LIBRARIES}")
    target_include_directories(ada PUBLIC   ${BROADCOM_INCLUDE_DIRS})
    target_link_libraries(ada PUBLIC        ${BROADCOM_LIBRARIES})
    target_compile_definitions(ada PUBLIC   ${BROADCOM_DEFINITIONS})
endif()

if(MSVC)
	add_definitions( -DPLATFORM_WINDOWS )
endif()

if (NO_X11 OR FORCE_GBM)

    # add_compile_options(ada PUBLIC "-Wall -O3 -std=c++11 -DGLM_FORCE_CXX14 -fpermissive -Wno-psabi")

    if (BROADCOM_FOUND AND NOT FORCE_GBM)

        target_compile_definitions(ada PUBLIC   
            PLATFORM_RPI 
            DRIVER_BROADCOM
        )

        find_package(ILCLIENT)

        message(STATUS "LOADING BROADCOM versions of EGL GL_ESv2")
        message(STATUS "EGL_LIBRARY:            ${BCM_EGL}")
        message(STATUS "GLESv2_LIBRARIES:       ${BCM_GLESv2}")
        target_link_libraries(ada PUBLIC        
            ${BCM_EGL} 
            ${BCM_GLESv2}
            ${ILCLIENT_LIBRARY}
        )

        target_include_directories(ada PUBLIC
            ${EGL_INCLUDE_DIRS}
            ${GLESv2_INCLUDE_DIRS}
            ${ILCLIENT_INCLUDE_DIR}
        )
            
    else()

        message(STATUS "LOADING GBM/DRM")
        
        find_package(GBM)
        find_package(DRM)
        
        message(STATUS "DRM_INCLUDE_DIRS:       ${DRM_INCLUDE_DIRS}")
        message(STATUS "DRM_LIBRARIES:          ${DRM_LIBRARIES}")
        message(STATUS "GBM_INCLUDE_DIRS:       ${GBM_INCLUDE_DIRS}")
        message(STATUS "GBM_LIBRARY:            ${GBM_LIBRARY}")

        target_compile_definitions(ada PUBLIC 
            DRIVER_GBM
            PLATFORM_RPI
        )

        target_include_directories(ada PUBLIC
            ${DRM_INCLUDE_DIRS}
            ${GBM_INCLUDE_DIRS}
        )

        target_link_libraries(ada PUBLIC
            ${DRM_LIBRARIES}
            ${GBM_LIBRARY}
        )

        message(STATUS "LOADING EGL GL_ESv2")
        find_package(EGL)
        find_package(GLESv2)
        message(STATUS "EGL_INCLUDE_DIRS:       ${EGL_INCLUDE_DIRS}")
        message(STATUS "EGL_LIBRARY:            ${EGL_LIBRARY}")
        message(STATUS "GLESv2_INCLUDE_DIRS:    ${GLESv2_INCLUDE_DIRS}")
        message(STATUS "GLESv2_LIBRARIES:       ${GLESv2_LIBRARIES}")
    
        target_include_directories(ada PUBLIC
            ${EGL_INCLUDE_DIRS}
            ${GLESv2_INCLUDE_DIRS}
        )
    
        target_link_libraries(ada PUBLIC
            ${EGL_LIBRARY}
            ${GLESv2_LIBRARIES}
        )
    endif()

else()
    message(STATUS "LOADING GLFW")

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
        target_link_libraries(ada PUBLIC 
            "-framework OpenGL"
        )

    elseif (WIN32)
        target_include_directories(ada PUBLIC 
            "${PROJECT_SOURCE_DIR}/deps/glew/include"
        )

        target_link_libraries(ada PUBLIC 
            libglew_static
        )

    # RASPBERRY PI
    elseif (BROADCOM_FOUND)

        find_package(EGL)
        find_package(GLESv2)
        message(STATUS "EGL_INCLUDE_DIRS:       ${EGL_INCLUDE_DIRS}")
        message(STATUS "EGL_LIBRARY:            ${EGL_LIBRARY}")
        message(STATUS "GLESv2_INCLUDE_DIRS:    ${GLESv2_INCLUDE_DIRS}")
        message(STATUS "GLESv2_LIBRARIES:       ${GLESv2_LIBRARIES}")
    
        target_include_directories(ada PUBLIC
            ${EGL_INCLUDE_DIRS}
            ${GLESv2_INCLUDE_DIRS}
        )
    
        target_link_libraries(ada PUBLIC
            ${EGL_LIBRARY}
            ${GLESv2_LIBRARIES}
            ${OPENGL_LIBRARIES}
        )

    #
    # elseif (EMSCRIPTEN)
    #     add_compile_options(-fpermissive -Wno-psabi -lpthread)
    #     set(LFLAGS "${LFLAGS} -pthread")
    #     set(LFLAGS "${LFLAGS} -s USE_PTHREADS=1")
    #     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LFLAGS}")
    #     set_target_properties(ada PROPERTIES LINK_FLAGS "${LFLAGS}")
    #
    #     target_link_libraries(ada PUBLIC 
    #         ${OPENGL_LIBRARIES}
    #         ${OPENGL_opengl_LIBRARY}
    #         -pthread
    #     )
    #
    # UNCOMMENT FOR MULTIPLE THREAD SUPPORT FOR WASM
    #   note: only works on desktop chrome 
    #

    else()

        target_link_libraries(ada PUBLIC 
            ${OPENGL_LIBRARIES}
            ${OPENGL_opengl_LIBRARY}
        )

    endif()

endif()

find_package(FFMPEG)
if (FFMPEG_FOUND)
    message(STATUS "LIBAV Supported")
    target_compile_definitions(ada PUBLIC LIBAV )
    target_include_directories(ada PUBLIC ${FFMPEG_INCLUDE_DIRS})
    target_link_libraries(ada PUBLIC ${FFMPEG_LIBRARIES})
endif()

# We need this directory, and users of our library will need it too
target_include_directories(ada PUBLIC 
    ${INCLUDE_FOLDER}
    "${PROJECT_SOURCE_DIR}/deps/glm"
    "${PROJECT_SOURCE_DIR}/deps/stb"
    "${PROJECT_SOURCE_DIR}/deps/glob"
    "${PROJECT_SOURCE_DIR}/deps/skymodel"
)

## DEPS
target_link_libraries(ada PUBLIC stb glob skymodel )

# IDEs should put the headers in a nice place
# source_group(   TREE ${INCLUDE_FOLDER} 
#                 PREFIX "Header Files" FILES ${ROOT_HEADER})

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(FILES ${ROOT_HEADER} DESTINATION include/ada)
install(FILES ${GL_HEADER} DESTINATION include/ada/gl)
install(FILES ${TOOLS_HEADER} DESTINATION include/ada/tools)
install(FILES ${SHADERS_SOURCES} DESTINATION include/ada/shaders)

