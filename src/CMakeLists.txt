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

if (APPLE)
    target_link_libraries(ada PUBLIC "-framework OpenGL")
endif (APPLE)

message(STATUS "GLFW_LIBRARIES: ${GLFW_LIBRARIES}")
message(STATUS "OPENGL_LIBRARIES: ${OPENGL_LIBRARIES}")
message(STATUS "OPENGL_opengl_LIBRARY: ${OPENGL_opengl_LIBRARY}")

target_compile_definitions(ada PUBLIC DRIVER_GLFW)
target_link_libraries(ada PUBLIC 
    glfw
    ${GLFW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${OPENGL_opengl_LIBRARY}
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
