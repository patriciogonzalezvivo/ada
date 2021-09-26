project(stb CXX)
set(STB_HEADERS stb_image.h stb_image_write.h)
set(STB_SOURCES stb_image.cpp  stb_image_write.cpp)

set_source_files_properties(${STB_SOURCES} PROPERTIES LANGUAGE CXX)
add_library(stb STATIC ${STB_SOURCES} ${STB_HEADERS})