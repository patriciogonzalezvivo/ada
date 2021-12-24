# Finds the GLESv2 library
#
# This will define the following variables::
#
# GLESv2_FOUND - system has GLESv2
# GLESv2_INCLUDE_DIRS - the GLESv2 include directory
# GLESv2_LIBRARIES - the GLESv2 libraries
# GLESv2_DEFINITIONS - the GLESv2 definitions
#

find_library(GLESv2_LIBRARIES NAMES GLESv2)
find_path(GLESv2_INCLUDE_DIRS NAMES GLES2/gl2.h OGLES2/GLES2/gl2.h)
find_path(KHR_INCLUDE_DIRS NAMES KHR/khrplatform.h PATH_SUFFIXES OGLES2)

if(NOT ${KHR_INCLUDE_DIRS} STREQUAL KHR_INCLUDE_DIRS-NOTFOUND)
    set(GLESv2_INCLUDE_DIRS ${GLESv2_INCLUDE_DIRS} ${KHR_INCLUDE_DIRS})
endif()

message(STATUS KHR ${KHR_INCLUDE_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLESv2 DEFAULT_MSG GLESv2_LIBRARIES GLESv2_INCLUDE_DIRS)

mark_as_advanced(GLESv2_INCLUDE_DIRS GLESv2_LIBRARIES)