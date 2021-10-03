# This module defines
#  ILCLIENT_DIR, where to find png.h, etc.
#  ILCLIENT_LIBRARIES, the libraries to link against to use PNG.
#  ILCLEINT_FOUND, If false, do not try to use PNG.

set(ILCLIENT_INCLUDE_DIR /opt/vc/src/hello_pi/libs/ilclient)

find_library(   ILCLIENT_LIBRARY
                NAME libilclient.a
                DOC "Path ILCLIENT Library"
                PATHS ILCLIENT_INCLUDE_DIR )

mark_as_advanced(ILCLIENT_INCLUDE_DIR ILCLIENT_LIBRARY)

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(ilclient 
    ILCLIENT_LIBRARY
)
