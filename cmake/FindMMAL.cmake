# - Find the MMAL dependencies

# This module defines
#   MMAL_INCLUDE_DIR, where to find png.h, etc.
#   MMAL_LIBRARIES, the libraries to link against to use PNG.
#   MMAL_FOUND, If false, do not try to use PNG.
#   MMAL_DEFINITIONS

find_library(   MMAL_LIBRARY
                NAMES libmmal.so
                DOC "Path to MMAL Library"
                PATHS /opt/vc/lib)

if (MMAL_LIBRARY) 
    mark_as_advanced(MMAL_LIBRARY)

    find_library(   MMAL_CORE_LIBRARY
                    NAMES libmmal_core.so
                    DOC "Path to MMAL CORE Library"
                    PATHS /opt/vc/lib)
    mark_as_advanced(MMAL_CORE_LIBRARY)

    find_library(   MMAL_UTIL_LIBRARY
                    NAMES libmmal_util.so
                    DOC "Path to MMAL UTIL Library"
                    PATHS /opt/vc/lib)
    mark_as_advanced(MMAL_UTIL_LIBRARY)

    find_library(   MMAL_VC_CLIENT_LIBRARY
                    NAMES libmmal_vc_client.so
                    DOC "Path to MMAL VC CLIENT Library"
                    PATHS /opt/vc/lib)
    mark_as_advanced(MMAL_VC_CLIENT_LIBRARY)

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(MMAL 
        MMAL_LIBRARY
        MMAL_CORE_LIBRARY
        MMAL_UTIL_LIBRARY
        MMAL_VC_CLIENT_LIBRARY
    )

    set(MMAL_LIBRARIES 
        ${MMAL_LIBRARY}
        ${MMAL_CORE_LIBRARY}
        ${MMAL_UTIL_LIBRARY}
        ${MMAL_VC_CLIENT_LIBRARY}
    )

    set(MMAL_DEFINITIONS 
        SUPPORT_MMAL
    )
endif()
