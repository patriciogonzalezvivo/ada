cmake_minimum_required(VERSION 3.2)

project(ada)
# project(ada
#     VERSION 0.1
#     DESCRIPTION "library to set the bases of any OpenGL app"
#     LANGUAGES CXX)
    
# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# The compiled library code is here
add_subdirectory(deps)
add_subdirectory(src)
add_subdirectory(examples)
