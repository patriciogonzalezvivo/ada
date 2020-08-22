file(GLOB ROOT_HEADER "${PROJECT_SOURCE_DIR}/include/ada/*.h")
file(GLOB ROOT_SOURCE "${PROJECT_SOURCE_DIR}/src/*.cpp")

file(GLOB GL_HEADER "${PROJECT_SOURCE_DIR}/include/ada/gl/*.h")
file(GLOB GL_SOURCE "${PROJECT_SOURCE_DIR}/src/gl/*.cpp")

file(GLOB SCENE_HEADER "${PROJECT_SOURCE_DIR}/include/ada/scene/*.h")
file(GLOB SCENE_SOURCE "${PROJECT_SOURCE_DIR}/src/scene/*.cpp")

file(GLOB TOOLS_HEADER "${PROJECT_SOURCE_DIR}/include/ada/tools/*.h")
file(GLOB TOOLS_SOURCE "${PROJECT_SOURCE_DIR}/src/tools/*.cpp")

set(INCLUDE_FOLDER "${PROJECT_SOURCE_DIR}/include")

# C++ LIBRAY
#
add_library(ada ${ROOT_SOURCE} ${GL_SOURCE} ${SCENE_SOURCE} ${TOOLS_SOURCE})

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
    message(STATUS "DRIVER_GBM: ${DRIVER_GBM}")
    target_compile_definitions(ada PUBLIC DRIVER_GBM)

    find_package(GBM)
    find_package(EGL)
    find_package(DRM)
    find_package(GLESv2)
    include_directories(
        ${DRM_INCLUDE_DIRS}
        # ${GBM_INCLUDE_DIRS}
        # ${EGL_INCLUDE_DIRS}
        ${GLESv2_INCLUDE_DIRS}
    )
    target_compile_definitions(ada PUBLIC DRIVER_GBM)
    target_link_libraries(ada PUBLIC
        ${DRM_LIBRARIES}
        ${EGL_LIBRARY}
        ${GLESv2_LIBRARIES}
        ${GBM_LIBRARY}
    )
else()
    message(STATUS "GLFW_LIBRARIES: ${GLFW_LIBRARIES}")
    target_compile_definitions(ada PUBLIC DRIVER_GLFW)
    target_link_libraries(ada PUBLIC 
        glfw
        ${GLFW_LIBRARIES}
    )
endif()

# OPENGL
message(STATUS "OPENGL_LIBRARIES: ${OPENGL_LIBRARIES}")
message(STATUS "OPENGL_opengl_LIBRARY: ${OPENGL_opengl_LIBRARY}")
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

## SKYMODLE
target_link_libraries(ada PUBLIC 
    skymodel
)

# IDEs should put the headers in a nice place
source_group(   TREE ${INCLUDE_FOLDER} 
                PREFIX "Header Files" FILES ${ROOT_HEADER})

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(FILES ${ROOT_HEADER} DESTINATION include/ada)
install(FILES ${GL_HEADER} DESTINATION include/ada/gl)
install(FILES ${SCENE_HEADER} DESTINATION include/ada/scene)
install(FILES ${TOOLS_HEADER} DESTINATION include/ada/tools)
