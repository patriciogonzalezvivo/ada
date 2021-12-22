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
mark_as_advanced(OPENMAX_LIBRARY)

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenMax 
    OPENMAX_LIBRARY 
)

set(OPENMAX_LIBRARIES 
    ${OPENMAX_LIBRARY}
)

# set(OPENMAX_INCLUDE_DIRS 
#     ${OPENMAX_INCLUDE_DIR} 
#     ${OPENMAX_INCLUDE_DIR}/interface/vmcs_host/linux 
#     ${OPENMAX_INCLUDE_DIR}/interface/vcos/pthreads
# )

set(OPENMAX_DEFINITIONS 
    SUPPORT_OMAX
    HAVE_LIBOPENMAX=2 
    OMX 
    OMX_SKIP64BIT 
    USE_EXTERNAL_OMX
)