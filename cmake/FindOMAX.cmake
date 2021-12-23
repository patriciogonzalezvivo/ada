# - Find OPENMAX drivers
#
# This module defines:
#
#   OPENMAX_INCLUDE_DIR,
#   OPENMAX_LIBRARIES, 
#   OPENMAX_FOUND,
#   OPENMAX_DEFINITIONS 
#
find_library(   OPENMAX_LIBRARY
                NAMES libopenmaxil.so
                DOC "Path to OpenMAX IL Library"
                PATHS /opt/vc/lib)

if (OPENMAX_LIBRARY)
    mark_as_advanced(OPENMAX_LIBRARY)

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(OMAX 
        OPENMAX_LIBRARY 
    )

    set(OMAX_LIBRARIES 
        ${OPENMAX_LIBRARY}
    )

    set(OMAX_DEFINITIONS 
        HAVE_LIBOPENMAX=2 
        OMX 
        OMX_SKIP64BIT 
        USE_EXTERNAL_OMX
    )
endif()