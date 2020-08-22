#.rst:
# FindGBM
# -------
#
# Try to find gbm on a Unix system.
#
# This will define the following variables:
#
# ``GBM_FOUND``
#     True if (the requested version of) GBM is available
# ``GBM_VERSION``
#     The version of GBM
# ``GBM_LIBRARIES``
#     This can be passed to target_link_libraries() instead of the ``GBM::GBM``
#     target
# ``GBM_INCLUDE_DIRS``
#     This should be passed to target_include_directories() if the target is not
#     used for linking
# ``GBM_DEFINITIONS``
#     This should be passed to target_compile_options() if the target is not
#     used for linking
#
# If ``GBM_FOUND`` is TRUE, it will also define the following imported target:
#
# ``GBM::GBM``
#     The GBM library
#
# In general we recommend using the imported target, as it is easier to use.
# Bear in mind, however, that if the target is in the link interface of an
# exported library, it must be made available by the package config file.

#=============================================================================
# Copyright 2014 Alex Merry <alex.merry@kde.org>
# Copyright 2014 Martin Gräßlin <mgraesslin@kde.org>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#=============================================================================

# Use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls
find_package(PkgConfig QUIET)
pkg_check_modules(PKG_GBM QUIET gbm)

set(GBM_DEFINITIONS "${PKG_GBM_CFLAGS_OTHER}")
set(GBM_VERSION "${PKG_GBM_VERSION}")

find_path(GBM_INCLUDE_DIRS
    NAMES
        gbm.h
    HINTS
        ${PKG_GBM_INCLUDE_DIRS}
)
find_library(GBM_LIBRARIES
    NAMES
        gbm
    HINTS
        ${PKG_GBM_LIBRARIES_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GBM
    FOUND_VAR
        GBM_FOUND
    REQUIRED_VARS
        GBM_LIBRARIES
        GBM_INCLUDE_DIRS
    VERSION_VAR
        GBM_VERSION
)

if(GBM_FOUND AND NOT TARGET GBM::GBM)
    add_library(GBM::GBM UNKNOWN IMPORTED)
    set_target_properties(GBM::GBM PROPERTIES
        IMPORTED_LOCATION "${GBM_LIBRARIES}"
        INTERFACE_COMPILE_OPTIONS "${GBM_DEFINITIONS}"
        INTERFACE_INCLUDE_DIRECTORIES "${GBM_INCLUDE_DIRS}"
    )
endif()

mark_as_advanced(GBM_LIBRARIES GBM_INCLUDE_DIRS)

include(FeatureSummary)
set_package_properties(GBM PROPERTIES
    URL "http://www.mesa3d.org"
    DESCRIPTION "Mesa gbm library."
)

# # FindGBM
# # ----------
# # Finds the GBM library
# #
# # This will define the following variables::
# #
# # GBM_FOUND - system has GBM
# # GBM_INCLUDE_DIRS - the GBM include directory
# # GBM_LIBRARIES - the GBM libraries
# # GBM_DEFINITIONS  - the GBM definitions
# #
# # and the following imported targets::
# #
# #   GBM::GBM   - The GBM library

# if(PKG_CONFIG_FOUND)
#   pkg_check_modules(PC_GBM gbm QUIET)
# endif()

# find_path(GBM_INCLUDE_DIR NAMES gbm.h
#                           PATHS ${PC_GBM_INCLUDEDIR})
# find_library(GBM_LIBRARY NAMES gbm
#                          PATHS ${PC_GBM_LIBDIR})

# set(GBM_VERSION ${PC_GBM_VERSION})

# include(FindPackageHandleStandardArgs)
# find_package_handle_standard_args(GBM
#                                   REQUIRED_VARS GBM_LIBRARY GBM_INCLUDE_DIR
#                                   VERSION_VAR GBM_VERSION)

# include(CheckCSourceCompiles)
# set(CMAKE_REQUIRED_LIBRARIES ${GBM_LIBRARY})
# check_c_source_compiles("#include <gbm.h>

#                          int main()
#                          {
#                            gbm_bo_map(NULL, 0, 0, 0, 0, GBM_BO_TRANSFER_WRITE, NULL, NULL);
#                          }
#                          " GBM_HAS_BO_MAP)

# check_c_source_compiles("#include <gbm.h>

#                          int main()
#                          {
#                            gbm_surface_create_with_modifiers(NULL, 0, 0, 0, NULL, 0);
#                          }
#                          " GBM_HAS_MODIFIERS)

# if(GBM_FOUND)
#   set(GBM_LIBRARIES ${GBM_LIBRARY})
#   set(GBM_INCLUDE_DIRS ${GBM_INCLUDE_DIR})
#   set(GBM_DEFINITIONS -DHAVE_GBM=1)
#   if(GBM_HAS_BO_MAP)
#     list(APPEND GBM_DEFINITIONS -DHAS_GBM_BO_MAP=1)
#   endif()
#   if(GBM_HAS_MODIFIERS)
#     list(APPEND GBM_DEFINITIONS -DHAS_GBM_MODIFIERS=1)
#   endif()
#   if(NOT TARGET GBM::GBM)
#     add_library(GBM::GBM UNKNOWN IMPORTED)
#     set_target_properties(GBM::GBM PROPERTIES
#                                    IMPORTED_LOCATION "${GBM_LIBRARY}"
#                                    INTERFACE_INCLUDE_DIRECTORIES "${GBM_INCLUDE_DIR}")
#   endif()
# endif()

# mark_as_advanced(GBM_INCLUDE_DIR GBM_LIBRARY)
