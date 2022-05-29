#pragma once

#ifndef EVENTS_AS_CALLBACKS
#define EVENTS_AS_CALLBACKS
#endif

#include "window.h"
#include <math.h>

namespace ada {

class App {
public:

    void run(glm::ivec4 &_viewport, WindowProperties _properties = WindowProperties());

    virtual void setup() {};
    virtual void update() {};
    virtual void draw() {};
    virtual void close() {};

    virtual void windowResized() {};
    virtual void onViewportResize(int _width, int _height) {};

    virtual void keyPressed() {};
    virtual void onKeyPress(int _key) {};
    
    virtual void mouseMoved() {}
    virtual void onMouseMove(float _x, float _y) {};
    
    virtual void mouseClick() {};
    virtual void onMouseClick(float _x, float _y, int _button) {};

    virtual void mouseDragged() {};
    virtual void onMouseDrag(float _x, float _y, int _button) {};

    virtual void onScroll(float _yoffset) {};

    int     year()  const { return getDate().x; }
    int     month() const { return getDate().y; }
    int     day()   const { return getDate().z; }
    
    int     hour()      const { return std::fmod( getDate().w * 0.000277778f, 24.0f); }
    int     minute()    const { return std::fmod( getDate().w * 0.0166667f, 60.0f); }
    int     second()    const { return std::fmod( getDate().w, 60.0f ); }
    float   millis()    const { return getDate().w * 1000.0f; }

    float   mouseX, mouseY;
    float   movedX, movedY;
    float   pmouseX, pmouseY;
    int     mouseButton;

    float   width, height;
    float   time, deltaTime;
    int     frameCount;

protected:

    #if defined(__EMSCRIPTEN__)
    EM_BOOL loop (double time, void* userData);
    #else
    void loop();
    #endif

};

}