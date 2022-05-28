#include "ada/app.h"

#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
// #define GLFW_INCLUDE_ES3
#endif


namespace ada {

void App::run(glm::ivec4 &_viewport, WindowProperties _properties) {
    initGL(_viewport, _properties);

    width = getWindowWidth();
    height = getWindowHeight();
    deltaTime = getDelta();
    time = getTime();
    frameCount = 0;

    mouseX = 0.0f;
    mouseY = 0.0f;
    movedX = 0.0f;
    movedY = 0.0f;
    pmouseX = 0.0f;
    pmouseY = 0.0f;

    setup();

#ifdef EVENTS_AS_CALLBACKS
    setViewportResizeCallback( [&](int _width, int _height) { 
        onViewportResize(_width, _height); 
        windowResized();
    } );
    
    setKeyPressCallback( [&](int _key) { 
        onKeyPress(_key); 
        keyPressed();
    } );

    setMouseMoveCallback( [&](float _x, float _y) { 
        mouseX = _x;
        mouseY = _y;
        
        movedX = getMouseVelX();
        movedY = getMouseVelY();

        pmouseX = _x - movedX;
        pmouseY = _y - movedY;

        onMouseMove(_x, _y); 
        mouseMoved();
    } );
    
    setMouseClickCallback( [&](float _x, float _y, int _button) { 
        mouseButton = _button;
        mouseClick();
        onMouseClick(_x, _y, _button); 
    } );

    setMouseDragCallback( [&](float _x, float _y, int _button) { 
        mouseButton = _button;

        mouseX = _x;
        mouseY = _y;
        
        movedX = getMouseVelX();
        movedY = getMouseVelY();

        pmouseX = _x - movedX;
        pmouseY = _y - movedY;

        onMouseDrag(_x, _y, _button); 
        mouseDragged();
    } );

    setScrollCallback( [&](float _yoffset) { onScroll(_yoffset); } );
#endif

#ifdef __EMSCRIPTEN__
        // Run the loop 
    emscripten_request_animation_frame_loop(loop, 0);

    double width,  height;
    emscripten_get_element_css_size("#canvas", &width, &height);
    setWindowSize(width, height);
    
#else
    
    // Render Loop
    while ( isGL() )
        loop();

    closeGL();
        close();

#endif

}


#if defined(__EMSCRIPTEN__)
EM_BOOL App::loop (double time, void* userData) {
#else
void App::loop() {
#endif

    width = getWindowWidth();
    height = getWindowHeight();
    deltaTime = getDelta();
    time = getTime();
    frameCount++;

    // Update
    update();
    updateGL();

    draw();
    renderGL();

    #if defined(__EMSCRIPTEN__)
    return true;
    #endif
}

}