set(ADA_HEADERS
    ${PROJECT_SOURCE_DIR}/include/window.h
    ${PROJECT_SOURCE_DIR}/include/gl/defines.h
    ${PROJECT_SOURCE_DIR}/include/gl/face.h
    ${PROJECT_SOURCE_DIR}/include/gl/fbo.h
    ${PROJECT_SOURCE_DIR}/include/gl/gl.h
    ${PROJECT_SOURCE_DIR}/include/gl/mesh.h
    ${PROJECT_SOURCE_DIR}/include/gl/pingpong.h
    ${PROJECT_SOURCE_DIR}/include/gl/shader.h
    ${PROJECT_SOURCE_DIR}/include/gl/texture.h
    ${PROJECT_SOURCE_DIR}/include/gl/textureCube.h
    ${PROJECT_SOURCE_DIR}/include/gl/vbo.h 
    ${PROJECT_SOURCE_DIR}/include/gl/vertexLayout.h
    ${PROJECT_SOURCE_DIR}/include/scene/camera.h
    ${PROJECT_SOURCE_DIR}/include/scene/light.h
    ${PROJECT_SOURCE_DIR}/include/scene/node.h
    ${PROJECT_SOURCE_DIR}/include/tools/fs.h 
    ${PROJECT_SOURCE_DIR}/include/tools/list.h
    ${PROJECT_SOURCE_DIR}/include/tools/math.h
    ${PROJECT_SOURCE_DIR}/include/tools/pixels.h
    ${PROJECT_SOURCE_DIR}/include/tools/text.h
)

set(ADA_SOURCES
    window.cpp
    gl/defines.cpp 
    gl/fbo.cpp
    gl/mesh.cpp 
    gl/pingpong.cpp 
    gl/shader.cpp
    gl/texture.cpp 
    gl/textureCube.cpp 
    gl/vbo.cpp 
    gl/vertexLayout.cpp 
    scene/camera.cpp 
    scene/light.cpp 
    scene/node.cpp 
    tools/fs.cpp 
    tools/list.cpp
    tools/pixels.cpp 
    tools/text.cpp
)

set(INCLUDE_FOLDER "${PROJECT_SOURCE_DIR}/include")

# C++ LIBRAY
#
add_library(ada ${ADA_SOURCES})

set_target_properties(ada PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)

# We need this directory, and users of our library will need it too
target_include_directories(ada PUBLIC 
    ${INCLUDE_FOLDER}
    "${PROJECT_SOURCE_DIR}/deps/glm"
    "${PROJECT_SOURCE_DIR}/deps/stb"
    "${PROJECT_SOURCE_DIR}/deps/skymodel"
)

# WINDOW
if (NO_GLFW)

    if (BROADCOM_FOUND)

        target_compile_definitions(ada PUBLIC 
            DRIVER_BROADCOM 
            ${BROADCOM_DEFINITIONS}
        )
            
        include_directories(ada PUBLIC
            ${BROADCOM_INCLUDE_DIR}
        )

        target_link_libraries(ada PUBLIC
            ${BROADCOM_LIBRARIES}
        )

    else()

        target_compile_definitions(ada PUBLIC 
            DRIVER_GBM
        )

        include_directories(ada PUBLIC
            ${DRM_INCLUDE_DIRS}
            ${GBM_INCLUDE_DIRS}
        )

        target_link_libraries(ada PUBLIC
            ${DRM_LIBRARIES}
            ${GBM_LIBRARY}
        )

    endif()

    target_compile_definitions(ada PUBLIC 
        GL_ES
    )

    include_directories(ada PUBLIC
        ${EGL_INCLUDE_DIRS}
        ${GLESv2_INCLUDE_DIRS}
    )

    target_link_libraries(ada PUBLIC
        ${EGL_LIBRARY}
        ${GLESv2_LIBRARIES}
    )
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
    else()
        target_link_libraries(ada PUBLIC 
            ${OPENGL_LIBRARIES}
            ${OPENGL_opengl_LIBRARY}
        )
    endif (APPLE)

endif()


## SKYMODLE
target_link_libraries(ada PUBLIC 
    skymodel
)

# IDEs should put the headers in a nice place
# source_group(   TREE ${INCLUDE_FOLDER} 
#                 PREFIX "Header Files" FILES ${ADA_HEADERS})

# install(TARGETS ${PROJECT_NAME} DESTINATION lib)
# install(FILES ${ADA_HEADERS} DESTINATION include/ada)
