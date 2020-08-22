add_executable (hello_world 
    hello_world.cpp
)

set_target_properties(hello_world PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)

# include_directories(hello_world PRIVATE ${DRM_INCLUDE_DIRS})

target_link_libraries (hello_world PRIVATE ada)