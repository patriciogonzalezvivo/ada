# Find BROADCOM drivers
#
# This module defines
#   BROADCOM_INCLUDE_DIR
#   BROADCOM_LIBRARIES
#   BROADCOM_FOUND
#   BROADCOM_DEFINITIONS
#
find_path(BROADCOM_INCLUDE_DIR
        NAMES bcm_host.h
        DOC "Broadcom include directory"
        PATHS /opt/vc/include)

if (BROADCOM_INCLUDE_DIR)
    mark_as_advanced(BROADCOM_INCLUDE_DIR)

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

    find_library(   BCM_HOST_LIBRARY
                    NAMES libbcm_host.so
                    DOC "Path to Broadcom Host Library"
                    PATHS /opt/vc/lib)
    mark_as_advanced(BCM_HOST_LIBRARY)

    find_library(   BCM_EGL 
                    NAMES libbrcmEGL.so
                    DOC "Path to Broadcom EGL Library"
                    PATHS /opt/vc/lib)
    mark_as_advanced(BCM_EGL)

    find_library(   BCM_GLESv2
                    NAMES libbrcmGLESv2.so
                    DOC "Path to Broadcom EGL Library"
                    PATHS /opt/vc/lib)
    mark_as_advanced(BCM_GLESv2)

    # find_library(   MMAL_LIBRARY
    #                 NAMES libmmal.so
    #                 DOC "Path to MMAL Library"
    #                 PATHS /opt/vc/lib)
    # mark_as_advanced(MMAL_LIBRARY)

    # find_library(   MMAL_CORE_LIBRARY
    #                 NAMES libmmal_core.so
    #                 DOC "Path to MMAL CORE Library"
    #                 PATHS /opt/vc/lib)
    # mark_as_advanced(MMAL_CORE_LIBRARY)

    # find_library(   MMAL_UTIL_LIBRARY
    #                 NAMES libmmal_util.so
    #                 DOC "Path to MMAL UTIL Library"
    #                 PATHS /opt/vc/lib)
    # mark_as_advanced(MMAL_UTIL_LIBRARY)

    # find_library(   MMAL_VC_CLIENT_LIBRARY
    #                 NAMES libmmal_vc_client.so
    #                 DOC "Path to MMAL VC CLIENT Library"
    #                 PATHS /opt/vc/lib)
    # mark_as_advanced(MMAL_VC_CLIENT_LIBRARY)

    # find_library(   OPENMAXIL_LIBRARY
    #                 NAMES libopenmaxil.so
    #                 DOC "Path to OpenMAX IL Library"
    #                 PATHS /opt/vc/lib)
    # mark_as_advanced(OPENMAXIL_LIBRARY)

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(BROADCOM 
        DEFAULT_MSG 
        BROADCOM_INCLUDE_DIR 
        VCOS_LIBRARY 
        VCHIQ_LIBRARY
        BCM_HOST_LIBRARY
        BCM_EGL
        BCM_GLESv2

        # MMAL_LIBRARY
        # MMAL_CORE_LIBRARY
        # MMAL_UTIL_LIBRARY
        # MMAL_VC_CLIENT_LIBRARY
        # OPENMAXIL_LIBRARY 
    )

    set(BROADCOM_LIBRARIES 
        ${BCM_HOST_LIBRARY} 
        ${VCHIQ_LIBRARY} 
        ${VCOS_LIBRARY}
        # ${BCM_EGL}
        # ${BCM_GLESv2} 

        # ${MMAL_LIBRARY}
        # ${MMAL_CORE_LIBRARY}
        # ${MMAL_UTIL_LIBRARY}
        # ${MMAL_VC_CLIENT_LIBRARY}
        # ${OPENMAXIL_LIBRARY} 
    )

    set(BROADCOM_INCLUDE_DIRS 
        ${BROADCOM_INCLUDE_DIR} 
        ${BROADCOM_INCLUDE_DIR}/interface/vmcs_host/linux 
        ${BROADCOM_INCLUDE_DIR}/interface/vcos/pthreads
    )

    set(BROADCOM_DEFINITIONS 
        USE_VCHIQ_ARM
        HAVE_LIBBCM_HOST 
        USE_EXTERNAL_LIBBCM_HOST

        # SUPPORT_MMAL
        # HAVE_LIBOPENMAX=2 
        # OMX 
        # OMX_SKIP64BIT 
        # USE_EXTERNAL_OMX 
        # SUPPORT_OMAX
    )
endif()