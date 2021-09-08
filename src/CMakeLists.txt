set(INCLUDE_FOLDER "${PROJECT_SOURCE_DIR}/include")

file(GLOB ROOT_HEADER "${INCLUDE_FOLDER}/ada/*.h")
file(GLOB GL_HEADER "${INCLUDE_FOLDER}/ada/gl/*.h")
file(GLOB TOOLS_HEADER "${INCLUDE_FOLDER}/ada/tools/*.h")

set(ADA_SOURCES
    window.cpp
    gl/defines.cpp 
    gl/fbo.cpp
    gl/mesh.cpp 
    gl/pingpong.cpp
    gl/shader.cpp
    gl/texture.cpp 
    gl/textureBump.cpp
    gl/textureCube.cpp 
    gl/textureProps.cpp 
    gl/textureStreamSequence.cpp
    gl/vbo.cpp 
    gl/vertexLayout.cpp 
    tools/fs.cpp 
    tools/list.cpp
    tools/pixels.cpp 
    tools/text.cpp
    scene/node.cpp
    scene/camera.cpp
)

# C++ LIBRAY
#
add_library(ada ${ADA_SOURCES})

set_target_properties(ada PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)

# WINDOW
if (NO_X11 OR FORCE_GBM)
    message(STATUS "LOADING BROADCOM")
    find_package(BROADCOM)

    if (BROADCOM_FOUND)
        target_compile_definitions(ada PUBLIC 
            DRIVER_BROADCOM
            PLATFORM_RPI
        )
    endif()

    # add_compile_options(ada PUBLIC "-Wall -O3 -std=c++11 -DGLM_FORCE_CXX14 -fpermissive -Wno-psabi")
    
    if (BROADCOM_FOUND AND NOT FORCE_GBM)
    
        message(STATUS "BROADCOM_DEFINITIONS:   ${BROADCOM_DEFINITIONS}")
        message(STATUS "BROADCOM_INCLUDE_DIRS:  ${BROADCOM_INCLUDE_DIRS}")
        message(STATUS "BROADCOM_LIBRARIES:     ${BROADCOM_LIBRARIES}")

        target_compile_definitions(ada PUBLIC 
            DRIVER_BROADCOM
            ${BROADCOM_DEFINITIONS}
        )
            
        target_include_directories(ada PUBLIC
            ${BROADCOM_INCLUDE_DIRS}
        )

        target_link_libraries(ada PUBLIC
            ${BROADCOM_LIBRARIES}
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

    elseif (EMSCRIPTEN)

        add_compile_options(-fpermissive -Wno-psabi -lpthread)
        set(LFLAGS "${LFLAGS} -pthread")
        set(LFLAGS "${LFLAGS} -s USE_PTHREADS=1")
        
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LFLAGS}")
        set_target_properties(ada PROPERTIES LINK_FLAGS "${LFLAGS}")
        target_link_libraries(ada PUBLIC 
            ${OPENGL_LIBRARIES}
            ${OPENGL_opengl_LIBRARY}
            -pthread
        )

    elseif (RPI)

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
            ${OPENGL_LIBRARIES}
        )

    else()
        target_link_libraries(ada PUBLIC 
            ${OPENGL_LIBRARIES}
            ${OPENGL_opengl_LIBRARY}
        )
    endif (APPLE)

endif()

# We need this directory, and users of our library will need it too
target_include_directories(ada PUBLIC 
    ${INCLUDE_FOLDER}
    "${PROJECT_SOURCE_DIR}/deps/glm"
    "${PROJECT_SOURCE_DIR}/deps/stb"
    "${PROJECT_SOURCE_DIR}/deps/skymodel"
    "${PROJECT_SOURCE_DIR}/deps/glob"
)

## SKYMODLE
target_link_libraries(ada PUBLIC 
    skymodel glob
)

# IDEs should put the headers in a nice place
# source_group(   TREE ${INCLUDE_FOLDER} 
#                 PREFIX "Header Files" FILES ${ROOT_HEADER})

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(FILES ${ROOT_HEADER} DESTINATION include/ada)
install(FILES ${GL_HEADER} DESTINATION include/ada/gl)
install(FILES ${TOOLS_HEADER} DESTINATION include/ada/tools)
