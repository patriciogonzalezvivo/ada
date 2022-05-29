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
};

//	GL Context
//----------------------------------------------
int         initGL(glm::ivec4 &_viewport, WindowProperties _properties = WindowProperties());
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

glm::ivec2  getScreenSize();
float       getPixelDensity();

const glm::ivec4& getViewport();
const glm::mat4&  getOrthoMatrix();
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
void        setMousePosition(glm::vec2 _pos);

float       getMouseX();
float       getMouseY();
glm::vec2   getMousePosition();
float       getMouseVelX();
float       getMouseVelY();
glm::vec2   getMouseVelocity();
int         getMouseButton();
glm::vec4   getMouse4();
bool        getMouseEntered();

bool        isShiftPressed();
bool        isControlPressed();

// EVENTS
//----------------------------------------------

#ifdef EVENTS_AS_CALLBACKS
void    setViewportResizeCallback(std::function<void(int,int)>);
void    setKeyPressCallback(std::function<void(int)>);
void    setMouseMoveCallback(std::function<void(float, float)>);
void    setMouseDownCallback(std::function<void(float, float, int)>);
void    setMouseReleaseCallback(std::function<void(float, float, int)>);
void    setMouseDragCallback(std::function<void(float, float, int)>);
void    setScrollCallback(std::function<void(float)>);

#else
void    onViewportResize(int _width, int _height);
void    onKeyPress(int _key);
void    onMouseMove(float _x, float _y);
void    onMouseDown(float _x, float _y, int _button);
void    onMouseRelease(float _x, float _y, int _button);
void    onMouseDrag(float _x, float _y, int _button);
void    onScroll(float _yoffset);

#endif


}