#pragma once

#ifndef EVENTS_AS_CALLBACKS
#define EVENTS_AS_CALLBACKS
#endif

#include "window.h"

namespace ada {

class App {
public:

    void run(glm::ivec4 &_viewport, WindowProperties _properties = WindowProperties());

    virtual void setup() {};
    virtual void update() {};
    virtual void draw() {};
    virtual void close() {};

    virtual void onViewportResize(int _width, int _height) {};
    virtual void onKeyPress(int _key) {};
    virtual void onMouseMove(float _x, float _y) {};
    virtual void onMouseClick(float _x, float _y, int _button) {};
    virtual void onMouseDrag(float _x, float _y, int _button) {};
    virtual void onScroll(float _yoffset) {};

protected:

    #if defined(__EMSCRIPTEN__)
    EM_BOOL loop (double time, void* userData);
    #else
    void loop();
    #endif

};

}