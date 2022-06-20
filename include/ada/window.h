#pragma once

#include "gl/gl.h"
#include "glm/glm.hpp"
#include <string>

#ifdef EVENTS_AS_CALLBACKS
#include <functional>
#endif

namespace ada {

enum WindowStyle {
    HEADLESS = 0,
    REGULAR,
    ALLWAYS_ON_TOP,
    UNDECORATED,
    UNDECORATED_ALLWAYS_ON_TOP,
    FULLSCREEN,
    LENTICULAR
};

struct WindowProperties {
    WindowStyle style   = REGULAR;
    size_t      major   = 2;
    size_t      minor   = 0;
    size_t      msaa    = 0;
    
    std::string vendor = "";
    std::string renderer = "";
    std::string version = "";
    std::string glsl = "";
    std::string extensions = "";

    #if defined(DRIVER_GBM) 
    std::string display = "/dev/dri/card0";
    #endif

    #if !defined(DRIVER_GLFW)
    std::string mouse = "/dev/input/mice";
    #endif

    #if defined(__EMSCRIPTEN__)
    size_t      webgl = 0;
    #endif

    int         screen_x = -1;
    int         screen_y = -1;
    int         screen_width = -1;
    int         screen_height = -1;
};

//	GL Context
//----------------------------------------------
int         initGL(WindowProperties _properties = WindowProperties());
bool        isGL();
void        updateGL();
void        renderGL();
void        closeGL();

std::string getVendor();
std::string getRenderer();
std::string getGLVersion();
std::string getGLSLVersion();
std::string getExtensions();
bool        haveExtension(std::string _name);

#if defined(__EMSCRIPTEN__)
size_t      getWebGLVersionNumber();
#elif defined(PLATFORM_RPI)
EGLDisplay  getEGLDisplay();
EGLContext  getEGLContext();
#endif

//	Windows/Viewport
//----------------------------------------------
void        updateViewport();

void        setFps(int _fps);
void        setViewport(float _width, float _height);
void        setWindowSize(int _width, int _height);
void        setWindowTitle(const char* _title);
void        setWindowVSync(bool _value);

bool        isFullscreen();
void        setFullscreen(bool _fullscreen);
int         getScreenWidth();
int         getScreenHeight();
float       getPixelDensity();

const glm::ivec4& getViewport();
const glm::mat4&  getOrthoMatrix();
const glm::mat4&  getFlippedOrthoMatrix();

int         getWindowWidth();
int         getWindowHeight();
int         getWindowMSAA();


// TIME
// ---------------------------------------
glm::vec4   getDate();
double      getTimeSec();
double      getTime();
double      getDelta();
double      getFps();
float       getRestSec();
int         getRestMs();
int         getRestUs();

// Mouse Keyboards
// ---------------------------------------
void        setMousePosition(float _x, float _y);

float       getMouseX();
float       getMouseY();

float       getMouseVelX();
float       getMouseVelY();
int         getMouseButton();
bool        getMouseEntered();

bool        isShiftPressed();
bool        isControlPressed();

// EVENTS
//----------------------------------------------

#ifdef EVENTS_AS_CALLBACKS
void    setViewportResizeCallback(std::function<void(int,int)>);
void    setKeyPressCallback(std::function<void(int)>);
void    setMouseMoveCallback(std::function<void(float, float)>);
void    setMousePressCallback(std::function<void(float, float, int)>);
void    setMouseReleaseCallback(std::function<void(float, float, int)>);
void    setMouseDragCallback(std::function<void(float, float, int)>);
void    setScrollCallback(std::function<void(float)>);

#else
void    onViewportResize(int _width, int _height);
void    onKeyPress(int _key);
void    onMouseMove(float _x, float _y);
void    onMousePress(float _x, float _y, int _button);
void    onMouseRelease(float _x, float _y, int _button);
void    onMouseDrag(float _x, float _y, int _button);
void    onScroll(float _yoffset);

#endif


}