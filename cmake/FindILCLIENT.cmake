# This module defines
#  ILCLIENT_DIR, where to find png.h, etc.
#  ILCLIENT_LIBRARIES, the libraries to link against to use PNG.
#  ILCLEINT_FOUND, If false, do not try to use PNG.


find_library(   ILCLIENT_LIBRARY
                NAME libilclient.a
                DOC "Path ILCLIENT Library"
                PATHS /opt/vc/src/hello_pi/libs/ilclient )
if (ILCLIENT_LIBRARY)
    set(ILCLIENT_INCLUDE_DIR 
        /opt/vc/src/hello_pi/libs/ilclient)
    mark_as_advanced(ILCLIENT_LIBRARY ILCLIENT_INCLUDE_DIR)

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(ILCLIENT 
        ILCLIENT_LIBRARY
    )
endif()
