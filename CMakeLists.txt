cmake_minimum_required(VERSION 3.2)

project(ada)
# project(ada
#     VERSION 0.1
#     DESCRIPTION "library to set the bases of any OpenGL app"
#     LANGUAGES CXX)
    
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if (NO_X11)
    add_compile_options(-Wall -O3 -std=c++11 -DGLM_FORCE_CXX14 -fpermissive -Wno-psabi)
endif()

# The compiled library code is here
add_subdirectory(deps)
add_subdirectory(src)
add_subdirectory(examples)
