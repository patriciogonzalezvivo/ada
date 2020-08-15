file(GLOB ROOT_HEADER "${PROJECT_SOURCE_DIR}/include/ada/*.h")
file(GLOB ROOT_SOURCE "${PROJECT_SOURCE_DIR}/src/*.cpp")

file(GLOB GL_HEADER "${PROJECT_SOURCE_DIR}/include/ada/gl/*.h")
file(GLOB GL_SOURCE "${PROJECT_SOURCE_DIR}/src/gl/*.cpp")

file(GLOB SCENE_HEADER "${PROJECT_SOURCE_DIR}/include/ada/scene/*.h")
file(GLOB SCENE_SOURCE "${PROJECT_SOURCE_DIR}/src/scene/*.cpp")

file(GLOB TOOLS_HEADER "${PROJECT_SOURCE_DIR}/include/ada/tools/*.h")
file(GLOB TOOLS_SOURCE "${PROJECT_SOURCE_DIR}/src/tools/*.cpp")

set(INCLUDE_FOLDER "${PROJECT_SOURCE_DIR}/include")
# set(INCLUDE_SWIG_FILE "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}.i")

# C++ LIBRAY
#
add_library(ada ${ROOT_SOURCE} ${GL_SOURCE} ${SCENE_SOURCE} ${TOOLS_SOURCE})


# We need this directory, and users of our library will need it too
target_include_directories(ada PUBLIC 
    ${INCLUDE_FOLDER}
    "${PROJECT_SOURCE_DIR}/deps/glm"
    "${PROJECT_SOURCE_DIR}/deps/stb"
    "${PROJECT_SOURCE_DIR}/deps/skymodel"
)

target_compile_definitions(ada PUBLIC DRIVER_GLFW)

target_link_libraries(ada PUBLIC
    OpenGL
    glfw
    skymodel
    ${GLFW_LIBRARIES}
    ${OPENGL_LIBRARIES}
)

# IDEs should put the headers in a nice place
source_group(   TREE ${INCLUDE_FOLDER} 
                PREFIX "Header Files" FILES ${ROOT_HEADER})

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(FILES ${ROOT_HEADER} DESTINATION include/ada)
install(FILES ${GL_HEADER} DESTINATION include/ada/gl)
install(FILES ${SCENE_HEADER} DESTINATION include/ada/scene)
install(FILES ${TOOLS_HEADER} DESTINATION include/ada/tools)

# # PYTHON BINDINGS 
# #

# if(POLICY CMP0078)
#   cmake_policy(SET CMP0078 OLD)
# endif(POLICY CMP0078)

# if(POLICY CMP0086)
#   cmake_policy(SET CMP0086 OLD)
# endif(POLICY CMP0086)

# find_package(SWIG REQUIRED)
# include(${SWIG_USE_FILE})

# find_package(PythonLibs REQUIRED)
# include_directories(${PYTHON_INCLUDE_DIRS})

# if(NOT PYTHON_NUMPY_INCLUDE_DIR)
#     execute_process(COMMAND python3 -c "import numpy; print(numpy.get_include())"
#                     RESULT_VARIABLE PYTHON_NUMPY_PROCESS
#                     OUTPUT_VARIABLE PYTHON_NUMPY_INCLUDE_DIR
#                     OUTPUT_STRIP_TRAILING_WHITESPACE)

#     if(PYTHON_NUMPY_PROCESS EQUAL 0)
#         file(TO_CMAKE_PATH "${PYTHON_NUMPY_INCLUDE_DIR}" PYTHON_NUMPY_INCLUDE_CMAKE_PATH)
#         set(PYTHON_NUMPY_INCLUDE_DIR ${PYTHON_NUMPY_INCLUDE_CMAKE_PATH} CACHE PATH "Numpy include directory")
#     else()
#         message(SEND_ERROR "Could not determine the NumPy include directory, verify that NumPy was installed correctly.")
#     endif()
# endif()

# include_directories(${PYTHON_NUMPY_INCLUDE_DIR} ${INCLUDE_FOLDER})
# set_property(SOURCE ${INCLUDE_SWIG_FILE} PROPERTY CPLUSPLUS ON)

# if (CMAKE_VERSION VERSION_LESS "3.8")
#     swig_add_module(${PROJECT_NAME} 
#                     python 
#                     ${INCLUDE_SWIG_FILE} ${ROOT_SOURCE} ${GL_SOURCE} ${SCENE_SOURCE} ${TYPES_SOURCE} ${ACCEL_SOURCE})
# else()
#     swig_add_library(${PROJECT_NAME} 
#                     LANGUAGE python
#                     SOURCES ${INCLUDE_SWIG_FILE} ${ROOT_SOURCE} ${GL_SOURCE} ${SCENE_SOURCE} ${TYPES_SOURCE} ${ACCEL_SOURCE})
# endif()
# swig_link_libraries(${PROJECT_NAME} ${PYTHON_LIBRARIES})

# # INSTALL PYTHON BINDINGS
# # Get the python site packages directory by invoking python
# execute_process(COMMAND python3 -c "import site; print(site.getsitepackages()[0])" OUTPUT_VARIABLE PYTHON_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE)
# message("Installing at ${SITE_PACKAGES}")

# install(TARGETS _${PROJECT_NAME} DESTINATION ${PYTHON_SITE_PACKAGES}/${PROJECT_NAME})
# install(FILES ${PROJECT_SOURCE_DIR}/__init__.py DESTINATION ${PYTHON_SITE_PACKAGES}/${PROJECT_NAME})
# install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.py DESTINATION ${PYTHON_SITE_PACKAGES}/${PROJECT_NAME})
