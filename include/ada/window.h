#pragma once

#include "gl/gl.h"
#include "glm/glm.hpp"
#include <string>

namespace ada {

enum WindowStyle {
    HEADLESS = 0,
    REGULAR,
    ALLWAYS_ON_TOP,
    FULLSCREEN,
    HOLOPLAY
};

struct WindowProperties {
    WindowStyle style   = REGULAR;
    size_t      major   = 2;
    size_t      minor   = 0;
    size_t      msaa    = 0;
    
    #if defined(DRIVER_GBM) 
    std::string display = "/dev/dri/card1";
    #endif

    #if !defined(DRIVER_GLFW)
    std::string mouse = "/dev/input/mice";
    #endif
};

//	GL Context
//----------------------------------------------
int  initGL(glm::ivec4 &_viewport, WindowProperties _properties = WindowProperties());
bool isGL();
void updateGL();
void renderGL();
void closeGL();

//	SET
//----------------------------------------------
void updateViewport();

void setFps(int _fps);
void setViewport(float _width, float _height);
void setWindowSize(int _width, int _height);
void setWindowTitle(const char* _title);
void setMousePosition(float _x, float _y);
void setMousePosition(glm::vec2 _pos);

//	GET
//----------------------------------------------
glm::ivec2  getScreenSize();
float       getPixelDensity();

glm::ivec4  getViewport();
glm::mat4   getOrthoMatrix();
int         getWindowWidth();
int         getWindowHeight();
int         getWindowMSAA();

glm::vec4   getDate();
double      getTimeSec();
double      getTime();
double      getDelta();
double      getFps();
float       getRestSec();
int         getRestMs();
int         getRestUs();

float       getMouseX();
float       getMouseY();
glm::vec2   getMousePosition();
float       getMouseVelX();
float       getMouseVelY();
glm::vec2   getMouseVelocity();
int         getMouseButton();
glm::vec4   getMouse4();
bool        getMouseEntered();

// EVENTS
//----------------------------------------------
void onKeyPress(int _key);
void onMouseMove(float _x, float _y);
void onMouseClick(float _x, float _y, int _button);
void onMouseDrag(float _x, float _y, int _button);
void onViewportResize(int _width, int _height);
void onScroll(float _yoffset);

#ifdef PLATFORM_RPI
EGLDisplay getEGLDisplay();
EGLContext getEGLContext();
#endif

}