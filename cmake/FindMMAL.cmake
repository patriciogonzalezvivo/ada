# # - Find the MMAL dependencies

# # This module defines
# #   MMAL_INCLUDE_DIR, where to find png.h, etc.
# #   MMAL_LIBRARIES, the libraries to link against to use PNG.
# #   MMAL_FOUND, If false, do not try to use PNG.
# #   MMAL_DEFINITIONS

# if(PKG_CONFIG_FOUND)
#     pkg_check_modules(PC_MMAL mmal QUIET)
# endif()

# find_library(   MMAL_LIBRARY
#                 NAMES libmmal.so
#                 DOC "Path to MMAL Library"
#                 PATHS /opt/vc/lib)

# if (BROADCOM_INCLUDE_DIR && MMAL_LIBRARY_FOUND) 
#     mark_as_advanced(MMAL_LIBRARY)

#     find_library(   MMAL_CORE_LIBRARY
#                     NAMES libmmal_core.so
#                     DOC "Path to MMAL CORE Library"
#                     PATHS /opt/vc/lib)
#     mark_as_advanced(MMAL_CORE_LIBRARY)

#     find_library(   MMAL_UTIL_LIBRARY
#                     NAMES libmmal_util.so
#                     DOC "Path to MMAL UTIL Library"
#                     PATHS /opt/vc/lib)
#     mark_as_advanced(MMAL_UTIL_LIBRARY)

#     find_library(   MMAL_VC_CLIENT_LIBRARY
#                     NAMES libmmal_vc_client.so
#                     DOC "Path to MMAL VC CLIENT Library"
#                     PATHS /opt/vc/lib)
#     mark_as_advanced(MMAL_VC_CLIENT_LIBRARY)

#     include(FindPackageHandleStandardArgs)
#     find_package_handle_standard_args(MMAL 
#         MMAL_LIBRARY
#         MMAL_CORE_LIBRARY
#         MMAL_UTIL_LIBRARY
#         MMAL_VC_CLIENT_LIBRARY
#     )

#     set(MMAL_LIBRARIES 
#         ${MMAL_LIBRARY}
#         ${MMAL_CORE_LIBRARY}
#         ${MMAL_UTIL_LIBRARY}
#         ${MMAL_VC_CLIENT_LIBRARY}
#     )

#     set(MMAL_DEFINITIONS 
#         SUPPORT_MMAL
#     )
# endif()

# - Try to find MMAL
# Once done this will define
#
# MMAL_FOUND - system has MMAL
# MMAL_INCLUDE_DIRS - the MMAL include directory
# MMAL_LIBRARIES - The MMAL libraries

if(PKG_CONFIG_FOUND)
    pkg_check_modules(PC_MMAL mmal QUIET)
endif()

find_path(MMAL_INCLUDE_DIR NAMES interface/mmal/mmal.h PATHS ${PC_MMAL_INCLUDEDIR})
find_library(MMAL_LIBRARY NAMES mmal libmmal PATHS ${PC_MMAL_LIBDIR})
find_library(MMALCORE_LIBRARY NAMES mmal_core libmmal_core PATHS ${PC_MMAL_LIBDIR})
find_library(MMALUTIL_LIBRARY NAMES mmal_util libmmal_util PATHS ${PC_MMAL_LIBDIR})
find_library(MMALCLIENT_LIBRARY NAMES mmal_vc_client libmmal_vc_client PATHS ${PC_MMAL_LIBDIR})
find_library(MMALCOMPONENT_LIBRARY NAMES mmal_components libmmal_components PATHS ${PC_MMAL_LIBDIR})
find_library(BCM_LIBRARY NAMES bcm_host libbcm_host PATHS ${PC_MMAL_LIBDIR})
find_library(VCHIQ_LIBRARY NAMES vchiq_arm libvchiq_arm PATHS ${PC_MMAL_LIBDIR})
find_library(VCHOSTIF_LIBRARY NAMES vchostif libvchostif PATHS ${PC_MMAL_LIBDIR})
# find_library(VCILCS_LIBRARY NAMES vcilcs libvcilcs PATHS ${PC_MMAL_LIBDIR})
find_library(VCOS_LIBRARY NAMES vcos libvcos PATHS ${PC_MMAL_LIBDIR})
find_library(VCSM_LIBRARY NAMES vcsm libvcsm PATHS ${PC_MMAL_LIBDIR})
find_library(CONTAINER_LIBRARY NAMES containers libcontainers PATHS ${PC_MMAL_LIBDIR})


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MMAL  REQUIRED_VARS   MMAL_INCLUDE_DIR
                                                        MMAL_LIBRARY MMALCORE_LIBRARY MMALUTIL_LIBRARY
                                                        MMALCLIENT_LIBRARY MMALCOMPONENT_LIBRARY BCM_LIBRARY
                                                        VCHIQ_LIBRARY VCOS_LIBRARY VCSM_LIBRARY VCHOSTIF_LIBRARY
                                                        #  VCILCS_LIBRARY 
                                                        CONTAINER_LIBRARY)


if(MMAL_FOUND)
    set(MMAL_INCLUDE_DIRS ${MMAL_INCLUDE_DIR})
    set(MMAL_LIBRARIES ${MMAL_LIBRARY} ${MMALCORE_LIBRARY} ${MMALUTIL_LIBRARY}
                        ${MMALCLIENT_LIBRARY} ${MMALCOMPONENT_LIBRARY}
                        ${BCM_LIBRARY} ${VCHIQ_LIBRARY} ${VCOS_LIBRARY} ${VCSM_LIBRARY}
                        ${VCHOSTIF_LIBRARY} ${CONTAINER_LIBRARY} 
                        #  ${VCILCS_LIBRARY} 
        CACHE STRING "mmal libraries" FORCE)
    list(APPEND MMAL_DEFINITIONS -DHAVE_MMAL=1 -DHAS_MMAL=1)

    if(NOT TARGET MMAL::MMAL)
        add_library(MMAL::MMAL UNKNOWN IMPORTED)
        set_target_properties(MMAL::MMAL PROPERTIES
                                        IMPORTED_LOCATION "${MMAL_LIBRARIES}"
                                        INTERFACE_INCLUDE_DIRECTORIES "${MMAL_INCLUDE_DIR}")
    endif()
endif()

mark_as_advanced(   
    MMAL_INCLUDE_DIRS MMAL_LIBRARIES MMAL_DEFINITIONS
    MMAL_LIBRARY MMALCORE_LIBRARY MMALUTIL_LIBRARY MMALCLIENT_LIBRARY MMALCOMPONENT_LIBRARY BCM_LIBRARY
    VCHIQ_LIBRARY VCOS_LIBRARY VCSM_LIBRARY VCHOSTIF_LIBRARY CONTAINER_LIBRARY 
    # VCILCS_LIBRARY
)