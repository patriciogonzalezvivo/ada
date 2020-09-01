add_executable (hello_world 
    hello_world.cpp
)

set_target_properties(hello_world PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)

if(EMSCRIPTEN)
    set_target_properties(hello_world PROPERTIES SUFFIX ".html")
    set(LFLAGS "-s USE_GLFW=3")
    if(USE_LEGACY_OPENGL)
        set(LFLAGS "${LFLAGS} -s LEGACY_GL_EMULATION=1")
    endif()
    set_target_properties(hello_world PROPERTIES LINK_FLAGS "${LFLAGS}")
    target_link_libraries(hello_world PRIVATE ada glfw)
else()
    target_link_libraries (hello_world PRIVATE ada)

endif()


