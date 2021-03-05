cmake_minimum_required(VERSION 3.2)

project(ada)
# project(ada
#     VERSION 0.1
#     DESCRIPTION "library to set the bases of any OpenGL app"
#     LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

if (NO_X11 OR FORCE_GBM)
    add_compile_options(-std=c++14 -DGLM_FORCE_CXX14 -fpermissive -Wno-psabi)
endif()

# The compiled library code is here
add_subdirectory(deps)
add_subdirectory(src)

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    add_subdirectory(examples)
endif()
