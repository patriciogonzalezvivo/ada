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

    find_library(   BCM_HOST_LIBRARY
        NAMES libbcm_host.so
        DOC "Path to Broadcom Host Library"
        PATHS /opt/vc/lib)
    mark_as_advanced(BCM_HOST_LIBRARY)

    find_library(VCOS_LIBRARY
        NAMES libvcos.so
        DOC "Path to VCOS Library"
        PATHS /opt/vc/lib)
    mark_as_advanced(VCOS_LIBRARY)

    find_library(VCHIQ_LIBRARY
        NAMES libvchiq_arm.so
        DOC "Path to VCHIQ Library"
        PATHS /opt/vc/lib)
    mark_as_advanced(VCHIQ_LIBRARY)

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(MMAL 
        BCM_HOST_LIBRARY

        VCOS_LIBRARY
        VCHIQ_LIBRARY

        MMAL_LIBRARY
        MMAL_CORE_LIBRARY
        MMAL_UTIL_LIBRARY
        MMAL_VC_CLIENT_LIBRARY
    )

    set(MMAL_LIBRARIES 
        ${BCM_HOST_LIBRARY}

        ${VCOS_LIBRARY}
        ${VCHIQ_LIBRARY}

        ${MMAL_LIBRARY}
        ${MMAL_CORE_LIBRARY}
        ${MMAL_UTIL_LIBRARY}
        ${MMAL_VC_CLIENT_LIBRARY}
    )
endif()
