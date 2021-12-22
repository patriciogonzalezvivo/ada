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
mark_as_advanced(BCM_INCLUDE_DIR)

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

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Broadcom 
    DEFAULT_MSG 
    BROADCOM_INCLUDE_DIR 
    VCOS_LIBRARY 
    VCHIQ_LIBRARY
    BCM_HOST_LIBRARY
    BCM_EGL
    BCM_GLESv2
)

set(BROADCOM_LIBRARIES 
    ${BCM_HOST_LIBRARY} 
    # ${BCM_GLESv2} 
    # ${BCM_EGL}
    ${VCHIQ_LIBRARY} 
    ${VCOS_LIBRARY}
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
)