include_directories("${CMAKE_SOURCE_DIR}/include")

add_executable (hello_world hello_world.cpp)
target_link_libraries (hello_world PRIVATE ada)

