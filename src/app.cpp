#include "ada/app.h"

#include <iostream>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
// #define GLFW_INCLUDE_ES3
#endif

namespace ada {


#if defined(__EMSCRIPTEN__)
EM_BOOL App::loop (double _time, void* _userData) {
    App* _app = (App*)_userData;
#else
void App::loop(double _time, App* _app) {
#endif

    _app->time = _time;
    _app->width = getWindowWidth();
    _app->height = getWindowHeight();
    _app->focused = getMouseEntered();
    _app->deltaTime = getDelta();
    _app->frameCount++;

    // Update
    _app->update();
    updateGL();

    if (_app->auto_background_enabled)
        clear(_app->auto_background_color);

    _app->draw();
    renderGL();

    #if defined(__EMSCRIPTEN__)
    return true;
    #endif
}

void App::run(WindowProperties _properties) {
    initGL(_properties);

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

    post_setup = true;

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

        // mouseIsPressed = false;

        onMouseMove(_x, _y); 
        mouseMoved();
    } );
    
    setMousePressCallback( [&](float _x, float _y, int _button) { 
        mouseButton = _button;
        mouseIsPressed = true;

        onMousePress(_x, _y, _button); 
        mousePressed();
        mouseClicked();
    } );

    setMouseDragCallback( [&](float _x, float _y, int _button) { 
        mouseButton = _button;

        mouseX = _x;
        mouseY = _y;
        
        movedX = getMouseVelX();
        movedY = getMouseVelY();

        pmouseX = _x - movedX;
        pmouseY = _y - movedY;

        // mouseIsPressed = true;

        mouseDragged();
        onMouseDrag(_x, _y, _button); 
    } );

    setMouseReleaseCallback( [&](float _x, float _y, int _button) { 
        mouseButton = 0;
        mouseIsPressed = false;

        onMouseRelease(_x, _y, _button);
        mouseReleased();
    } );


    setScrollCallback( [&](float _yoffset) { onScroll(_yoffset); } );
#endif

#if defined(__EMSCRIPTEN__)
        // Run the loop 
    emscripten_request_animation_frame_loop(loop, (void*)this);    
#else
    
    // Render Loop
    while ( isGL() )
        loop(getTime(), this);

    closeGL();
        close();

#endif
}

void App::background() { background(auto_background_color); }
void App::background( float _brightness ) { background(glm::vec3(_brightness)); }
void App::background( const glm::vec3& _color ) { background(glm::vec4(_color, 1.0f)); }
void App::background( const glm::vec4& _color ) {
    auto_background_color = _color;
    if (!post_setup)
        auto_background_enabled = true;
    clear(auto_background_color);
}

void App::orbitControl() {
    Camera* cam = getCamera();

    if (cam) {
        double aspect = width/height;
        if (cam->getAspect() != aspect)
            cam->setViewport(width, height);
    }

    if (mouseIsPressed) {

        if (cam) {
            float dist = cam->getDistance();

            if (mouseButton == 1) {

                // Left-button drag is used to rotate geometry.
                if (fabs(movedX) < 50.0 && fabs(movedY) < 50.0) {
                    cameraLat -= getMouseVelX();
                    cameraLon -= getMouseVelY() * 0.5;
                    cam->orbit(cameraLat, cameraLon, dist);
                    cam->lookAt(glm::vec3(0.0));
                }
            } 
            else if (mouseButton == 2) {

                // Right-button drag is used to zoom geometry.
                dist += (-.008f * movedY);
                if (dist > 0.0f) {
                    cam->orbit(cameraLat, cameraLon, dist);
                    cam->lookAt(glm::vec3(0.0));
                }
            }
        }
    }

    glEnable(GL_DEPTH_TEST);
}

}