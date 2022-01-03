project(skymodel)

set(SKYMODEL_HEADERS
    ArHosekSkyModel.h
    ArHosekSkyModelData_CIEXYZ.h
    ArHosekSkyModelData_RGB.h
    ArHosekSkyModelData_Spectral.h)

set(SKYMODEL_SOURCES
    ArHosekSkyModel.cpp
)

set_source_files_properties(${SKYMODEL_SOURCES} PROPERTIES LANGUAGE CXX)
add_library(skymodel STATIC ${SKYMODEL_SOURCES} ${SKYMODEL_HEADERS})