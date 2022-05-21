#include "ada/app.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
// #define GLFW_INCLUDE_ES3
#endif


namespace ada {

void App::run(glm::ivec4 &_viewport, WindowProperties _properties) {
    ada::initGL(_viewport, _properties);

    setup();

#ifdef EVENTS_AS_CALLBACKS
    ada::setViewportResizeCallback( [&](int _width, int _height) { onViewportResize(_width, _height); } );
    ada::setKeyPressCallback( [&](int _key) { onKeyPress(_key); } );
    ada::setMouseMoveCallback( [&](float _x, float _y) { onMouseMove(_x, _y); } );
    ada::setMouseClickCallback( [&](float _x, float _y, int _button) { onMouseClick(_x, _y, _button); } );
    ada::setMouseDragCallback( [&](float _x, float _y, int _button) { onMouseDrag(_x, _y, _button); } );
    ada::setScrollCallback( [&](float _yoffset) { onScroll(_yoffset); } );
#endif

#ifdef __EMSCRIPTEN__
        // Run the loop 
    emscripten_request_animation_frame_loop(loop, 0);

    double width,  height;
    emscripten_get_element_css_size("#canvas", &width, &height);
    ada::setWindowSize(width, height);
    
#else
    
    // Render Loop
    while ( ada::isGL() )
        loop();

    ada::closeGL();
        close();

#endif

}


#if defined(__EMSCRIPTEN__)
EM_BOOL App::loop (double time, void* userData) {
#else
void App::loop() {
#endif

    // Update
    update();
    ada::updateGL();

    draw();
    ada::renderGL();

    #if defined(__EMSCRIPTEN__)
    return true;
    #endif
}

}