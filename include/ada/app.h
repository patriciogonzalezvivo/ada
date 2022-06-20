#pragma once

#ifndef EVENTS_AS_CALLBACKS
#define EVENTS_AS_CALLBACKS
#endif

#include "draw.h"
#include "window.h"
#include "scene/camera.h"

#include <math.h>

namespace ada {

class App {
public:

    void run(WindowProperties _properties = WindowProperties());

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
    
    virtual void mouseClicked() {};
    virtual void mousePressed() {};
    virtual void onMousePress(float _x, float _y, int _button) {};

    virtual void mouseReleased() {}
    virtual void onMouseRelease(float _x, float _y, int _button) {};

    virtual void mouseDragged() {};
    virtual void onMouseDrag(float _x, float _y, int _button) {};

    virtual void onScroll(float _yoffset) {};

    virtual void background();
    virtual void background( float _brightness );
    virtual void background( const glm::vec3& _color );
    virtual void background( const glm::vec4& _color );
    virtual void orbitControl();

    int     year()  const { return getDate().x; }
    int     month() const { return getDate().y; }
    int     day()   const { return getDate().z; }
    
    int     hour()      const { return std::fmod( getDate().w * 0.000277778f, 24.0f); }
    int     minute()    const { return std::fmod( getDate().w * 0.0166667f, 60.0f); }
    int     second()    const { return std::fmod( getDate().w, 60.0f ); }
    float   millis()    const { return getDate().w * 1000.0f; }


    float   cameraLat = 180.0f;
    float   cameraLon = 0.0f;

    double  width, height;

    float   mouseX, mouseY;
    float   movedX, movedY;
    float   pmouseX, pmouseY;
    int     mouseButton;
    bool    mouseIsPressed;
    bool    focused;

    double  time, deltaTime;
    int     frameCount;


protected:

    glm::vec4   auto_background_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    bool        auto_background_enabled = false;

    bool        post_setup = false;

    #if defined(__EMSCRIPTEN__)
    static EM_BOOL loop (double time, void* userData);
    #else
    static void loop(double _time, App* _app);
    #endif

};

}