#include "ada/window.h"

#include <time.h>
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>

#ifdef _WIN32
    #define NOMINMAX
    #include <windows.h>
#else
    #include <sys/time.h>
#endif 

#include "ada/fs.h"
#include "ada/time.h"
#include "ada/string.h"
#include "glm/gtc/matrix_transform.hpp"

// Common global variables
//----------------------------------------------------
static glm::ivec4               viewport;
static ada::WindowProperties    properties;
struct timespec                 time_start;
static glm::mat4                orthoMatrix;

typedef struct {
    bool      entered;
    float     x,y;
    int       button;
    float     velX,velY;
    glm::vec2 drag;
} Mouse;
static Mouse            mouse;
static glm::vec4        mouse4 = glm::vec4(0.0, 0.0, 0.0, 0.0);

struct timeval          tv;
static double           elapseTime = 0.0;
static double           delta = 0.0;
static double           FPS = 0.0;
static double           restSec = 0.0167; // default 60fps 
static float            fPixelDensity = 1.0;

static bool             bShift = false;    
static bool             bControl = false;    

#if defined(DRIVER_GLFW)

    #if defined(__APPLE__)
        #define GL_PROGRAM_BINARY_LENGTH 0x8741
        #include <GLFW/glfw3.h>
        #include <OpenGL/gl.h>
        #include <OpenGL/glext.h>

    #elif defined(_WIN32)
        #include <GL/glew.h>
        #include "GLFW/glfw3.h"
        #define APIENTRY __stdcall

    #elif defined(__EMSCRIPTEN__)
        #include <emscripten.h>
        #include <emscripten/html5.h>
        #define GL_GLEXT_PROTOTYPES
        #define EGL_EGLEXT_PROTOTYPES
        #include <GLFW/glfw3.h>

    #else
        // ANY LINUX using GLFW 
        #define GL_GLEXT_PROTOTYPES
        #include "GLFW/glfw3.h"
    #endif

namespace ada {
//----------------------------------------------------
static bool             left_mouse_button_down = false;
static GLFWwindow*      window;

    #ifdef PLATFORM_RPI
    #include "GLFW/glfw3native.h"
    EGLDisplay getEGLDisplay() { return glfwGetEGLDisplay(); }
    EGLContext getEGLContext() { return glfwGetEGLContext(window); }
    #endif

#else

    #if defined(DRIVER_BROADCOM)
        #include "bcm_host.h"
        #undef countof
    #elif defined(DRIVER_GBM)
        #include <xf86drm.h>
        #include <xf86drmMode.h>
        #include <gbm.h>
    #endif

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <assert.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h> 
#include <unistd.h>


namespace ada {

#define check() assert(glGetError() == 0)

// EGL context globals
EGLDisplay display;
EGLContext context;
EGLSurface surface;

EGLDisplay getEGLDisplay() { return display; }
EGLContext getEGLContext() { return context; }

// Get the EGL error back as a string. Useful for debugging.
static const char *eglGetErrorStr() {
    switch (eglGetError()) {
    case EGL_SUCCESS:
        return "The last function succeeded without error.";
    case EGL_NOT_INITIALIZED:
        return "EGL is not initialized, or could not be initialized, for the "
               "specified EGL display connection.";
    case EGL_BAD_ACCESS:
        return "EGL cannot access a requested resource (for example a context "
               "is bound in another thread).";
    case EGL_BAD_ALLOC:
        return "EGL failed to allocate resources for the requested operation.";
    case EGL_BAD_ATTRIBUTE:
        return "An unrecognized attribute or attribute value was passed in the "
               "attribute list.";
    case EGL_BAD_CONTEXT:
        return "An EGLContext argument does not name a valid EGL rendering "
               "context.";
    case EGL_BAD_CONFIG:
        return "An EGLConfig argument does not name a valid EGL frame buffer "
               "configuration.";
    case EGL_BAD_CURRENT_SURFACE:
        return "The current surface of the calling thread is a window, pixel "
               "buffer or pixmap that is no longer valid.";
    case EGL_BAD_DISPLAY:
        return "An EGLDisplay argument does not name a valid EGL display "
               "connection.";
    case EGL_BAD_SURFACE:
        return "An EGLSurface argument does not name a valid surface (window, "
               "pixel buffer or pixmap) configured for GL rendering.";
    case EGL_BAD_MATCH:
        return "Arguments are inconsistent (for example, a valid context "
               "requires buffers not supplied by a valid surface).";
    case EGL_BAD_PARAMETER:
        return "One or more argument values are invalid.";
    case EGL_BAD_NATIVE_PIXMAP:
        return "A NativePixmapType argument does not refer to a valid native "
               "pixmap.";
    case EGL_BAD_NATIVE_WINDOW:
        return "A NativeWindowType argument does not refer to a valid native "
               "window.";
    case EGL_CONTEXT_LOST:
        return "A power management event has occurred. The application must "
               "destroy all contexts and reinitialise OpenGL ES state and "
               "objects to continue rendering.";
    default:
        break;
    }
    return "Unknown error!";
}
#endif

#if defined(EVENTS_AS_CALLBACKS)
std::function<void(int,int)>            onViewportResize;
std::function<void(int)>                onKeyPress;
std::function<void(float, float)>       onMouseMove;
std::function<void(float, float, int)>  onMousePress;
std::function<void(float, float, int)>  onMouseDrag;
std::function<void(float, float, int)>  onMouseRelease;
std::function<void(float)>              onScroll;

void setViewportResizeCallback(std::function<void(int,int)> _callback) { onViewportResize = _callback; }
void setKeyPressCallback(std::function<void(int)> _callback) { onKeyPress = _callback; }
void setMouseMoveCallback(std::function<void(float, float)> _callback) { onMouseMove = _callback; }
void setMousePressCallback(std::function<void(float, float, int)> _callback) { onMousePress = _callback; }
void setMouseDragCallback(std::function<void(float, float, int)> _callback) { onMouseDrag = _callback; }
void setMouseReleaseCallback(std::function<void(float, float, int)> _callback) { onMouseRelease = _callback; }
void setScrollCallback(std::function<void(float)>_callback) { onScroll = _callback; }
#endif

#if defined(DRIVER_BROADCOM)
    DISPMANX_DISPLAY_HANDLE_T dispman_display;

#elif defined(DRIVER_GBM)
    // https://github.com/matusnovak/rpi-opengl-without-x/blob/master/triangle_rpi4.c

    int device;
    drmModeModeInfo mode;
    struct gbm_device *gbmDevice;
    struct gbm_surface *gbmSurface;
    drmModeCrtc *crtc;
    uint32_t connectorId;

    static drmModeConnector *getConnector(drmModeRes *resources) {
        for (int i = 0; i < resources->count_connectors; i++) {
            drmModeConnector *connector = drmModeGetConnector(device, resources->connectors[i]);
            if (connector->connection == DRM_MODE_CONNECTED)
                return connector;
            drmModeFreeConnector(connector);
        }
        return NULL;
    }

    static drmModeEncoder *findEncoder(drmModeRes *resources, drmModeConnector *connector) {
        if (connector->encoder_id)
            return drmModeGetEncoder(device, connector->encoder_id);
        return NULL;
    }

    static struct gbm_bo *previousBo = NULL;
    static uint32_t previousFb;

    static void gbmSwapBuffers() {
        struct gbm_bo *bo = gbm_surface_lock_front_buffer(gbmSurface);
        uint32_t handle = gbm_bo_get_handle(bo).u32;
        uint32_t pitch = gbm_bo_get_stride(bo);
        uint32_t fb;
        drmModeAddFB(device, mode.hdisplay, mode.vdisplay, 24, 32, pitch, handle, &fb);
        drmModeSetCrtc(device, crtc->crtc_id, fb, 0, 0, &connectorId, 1, &mode);
        if (previousBo) {
            drmModeRmFB(device, previousFb);
            gbm_surface_release_buffer(gbmSurface, previousBo);
        }
        previousBo = bo;
        previousFb = fb;
    }

    static void gbmClean() {
        // set the previous crtc
        drmModeSetCrtc(device, crtc->crtc_id, crtc->buffer_id, crtc->x, crtc->y, &connectorId, 1, &crtc->mode);
        drmModeFreeCrtc(crtc);
        if (previousBo) {
            drmModeRmFB(device, previousFb);
            gbm_surface_release_buffer(gbmSurface, previousBo);
        }
        gbm_surface_destroy(gbmSurface);
        gbm_device_destroy(gbmDevice);
    }
#endif

#if !defined(DRIVER_GLFW)
    static bool bHostInited = false;
    static void initHost() {
        if (bHostInited)
            return;

        #if defined(DRIVER_BROADCOM)
            bcm_host_init();

        #elif defined(DRIVER_GBM)
            if (!urlExists(properties.display))
                std::cout << "Can't open display " <<  properties.display << " seams it doesn't exist" << std::endl;
            
            device = open(  properties.display.c_str(), O_RDWR | O_CLOEXEC);

            drmModeRes *resources = drmModeGetResources(device);
            if (resources == NULL) {
                std::cerr << "Unable to get DRM resources" << std::endl;
                return EXIT_FAILURE;
            }

            drmModeConnector *connector = getConnector(resources);
            if (connector == NULL) {
                std::cerr << "Unable to get connector" << std::endl;
                drmModeFreeResources(resources);
                return EXIT_FAILURE;
            }

            connectorId = connector->connector_id;
            mode = connector->modes[0];

            drmModeEncoder *encoder = findEncoder(resources, connector);
            if (connector == NULL) {
                std::cerr << "Unable to get encoder" << std::endl;
                drmModeFreeConnector(connector);
                drmModeFreeResources(resources);
                return EXIT_FAILURE;
            }

            crtc = drmModeGetCrtc(device, encoder->crtc_id);
            drmModeFreeEncoder(encoder);
            drmModeFreeConnector(connector);
            drmModeFreeResources(resources);
            gbmDevice = gbm_create_device(device);
            gbmSurface = gbm_surface_create(gbmDevice, mode.hdisplay, mode.vdisplay, GBM_FORMAT_XRGB8888, GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
        #endif

        bHostInited = true;
    }

    static EGLDisplay getDisplay() {
        initHost();
        // printf("resolution: %ix%i\n", mode.hdisplay, mode.vdisplay);

        #if defined(DRIVER_BROADCOM)
            return eglGetDisplay(EGL_DEFAULT_DISPLAY);

        #elif defined(DRIVER_GBM)

            return eglGetDisplay(gbmDevice);
        #endif
    }
#endif

#ifdef __EMSCRIPTEN__

static EM_BOOL on_canvassize_changed(int eventType, const void *reserved, void *userData) {

    double width, height;
    emscripten_get_element_css_size("#canvas", &width, &height);
    if ((int)width != (int)viewport.z  || (int)height != (int)viewport.w)
        setWindowSize(width, height);

    return EM_FALSE;
}

bool enable_extension(const char* name) {
    auto ctx = emscripten_webgl_get_current_context();
    return emscripten_webgl_enable_extension(ctx, name);
}

static EM_BOOL mouse_callback(int eventType, const EmscriptenMouseEvent *e, void* userData) {
    float x = (float)e->targetX;
    float y = viewport.w - (float)e->targetY;

    if (eventType == EMSCRIPTEN_EVENT_MOUSEDOWN) {
        mouse.x = x;
        mouse.y = y;
        mouse.drag.x = mouse.x;
        mouse.drag.y = mouse.y;
        mouse.entered = true;
        mouse.button = 0;
        onMousePress(mouse.x, mouse.y, mouse.button);

    } else if (eventType == EMSCRIPTEN_EVENT_MOUSEUP) {
        mouse.entered = false;
        mouse.button = 0;
        onMouseRelease(mouse.x, mouse.y, mouse.button);

    } else if (eventType == EMSCRIPTEN_EVENT_MOUSEMOVE) {
        mouse.velX = x - mouse.drag.x;
        mouse.velY = y - mouse.drag.y;
        mouse.drag.x = x;
        mouse.drag.y = y;

        mouse.x = x;
        mouse.y = y;

        int action1 = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1);
        int action2 = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2);
        int button = 0;

        if (action1 == GLFW_PRESS) button = 1;
        else if (action2 == GLFW_PRESS) button = 2;

        // Lunch events
        if (mouse.button == 0 && button != mouse.button) {
            mouse.button = button;
            onMousePress(mouse.x, mouse.y, mouse.button);
        }
        else {
            mouse.button = button;
        }

        if (mouse.velX != 0.0 || mouse.velY != 0.0) {
            if (button != 0) onMouseDrag(mouse.x, mouse.y, mouse.button);
            else onMouseMove(mouse.x, mouse.y);
        }

    } else {
        printf("eventType is invalid. (%d)\n", eventType);
        return false;
    }

    return true;
}

static EM_BOOL touch_callback(int eventType, const EmscriptenTouchEvent *e, void *userData) {
    float x = e->touches[0].targetX;
    float y = viewport.w - e->touches[0].targetY;

    if (eventType == EMSCRIPTEN_EVENT_TOUCHSTART) {
        mouse.x = x;
        mouse.y = y;
        mouse.drag.x = mouse.x;
        mouse.drag.y = mouse.y;
        mouse.entered = true;
        mouse.button = 1;
        onMousePress(mouse.x, mouse.y, mouse.button);

    } else if (eventType == EMSCRIPTEN_EVENT_TOUCHEND) {
        mouse.entered = false;
        mouse.button = 0;
        onMouseRelease(mouse.x, mouse.y, mouse.button);

    } else if (eventType == EMSCRIPTEN_EVENT_TOUCHMOVE) {
        mouse.velX = x - mouse.drag.x;
        mouse.velY = y - mouse.drag.y;
        mouse.drag.x = x;
        mouse.drag.y = y;

        mouse.x = x;
        mouse.y = y;

        if (mouse.velX != 0.0 || mouse.velY != 0.0) {
            if (mouse.button != 0) onMouseDrag(mouse.x, mouse.y, mouse.button);
            else onMouseMove(mouse.x, mouse.y);
        }

    } else if (eventType == EMSCRIPTEN_EVENT_TOUCHCANCEL) {
        mouse.entered = false;
        mouse.button = 0;
        onMouseRelease(mouse.x, mouse.y, mouse.button);

    } else {
        printf("eventType is invalid. (%d)\n", eventType);
        return false;
    }

    return true;
}
#endif

#ifdef PLATFORM_WINDOWS
const int CLOCK_MONOTONIC = 0;
int clock_gettime(int, struct timespec* spec)      //C-file part
{
    __int64 wintime; GetSystemTimeAsFileTime((FILETIME*)&wintime);
    wintime -= 116444736000000000i64;  //1jan1601 to 1jan1970
    spec->tv_sec = wintime / 10000000i64;           //seconds
    spec->tv_nsec = wintime % 10000000i64 * 100;      //nano-seconds
    return 0;
}
#endif

int initGL(glm::ivec4 &_viewport, WindowProperties _prop) {
    clock_gettime(CLOCK_MONOTONIC, &time_start);
    properties = _prop;

    // NON GLFW
    #if !defined(DRIVER_GLFW)

        // Clear application state
        EGLBoolean result;

        display = getDisplay();
        assert(display != EGL_NO_DISPLAY);
        check();

        result = eglInitialize(display, NULL, NULL);
        assert(EGL_FALSE != result);
        check();

        // Make sure that we can use OpenGL in this EGL app.
        // result = eglBindAPI(EGL_OPENGL_API);
        result = eglBindAPI(EGL_OPENGL_ES_API);
        assert(EGL_FALSE != result);
        check();
       
        static const EGLint configAttribs[] = {
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_SAMPLE_BUFFERS, 1, EGL_SAMPLES, 4,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_DEPTH_SIZE, 16,
            EGL_NONE
        };

        static const EGLint contextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
        };

        EGLConfig config;
        EGLint numConfigs;

        // get an appropriate EGL frame buffer configuration
        if (eglChooseConfig(display, configAttribs, &config, 1, &numConfigs) == EGL_FALSE) {
            std::cerr << "Failed to get EGL configs! Error: " << eglGetErrorStr() << std::endl;
            eglTerminate(display);
            #ifdef DRIVER_GBM
            gbmClean();
            #endif
            return EXIT_FAILURE;
        }

        // create an EGL rendering context
        context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
        if (context == EGL_NO_CONTEXT) {
            std::cerr << "Failed to create EGL context! Error: " << eglGetErrorStr() << std::endl;
            eglTerminate(display);
            #ifdef DRIVER_GBM
            gbmClean();
            #endif
            return EXIT_FAILURE;
        }

        #ifdef DRIVER_BROADCOM
            static EGL_DISPMANX_WINDOW_T nativeviewport;

            VC_RECT_T dst_rect;
            VC_RECT_T src_rect;

            //  Initially the viewport is for all the screen
            dst_rect.x = _viewport.x;
            dst_rect.y = _viewport.y;
            dst_rect.width = _viewport.z;
            dst_rect.height = _viewport.w;

            src_rect.x = 0;
            src_rect.y = 0;
            src_rect.width = _viewport.z << 16;
            src_rect.height = _viewport.w << 16;

            DISPMANX_ELEMENT_HANDLE_T dispman_element;
            DISPMANX_UPDATE_HANDLE_T dispman_update;

            if (_prop.style == HEADLESS) {
                uint32_t dest_image_handle;
                DISPMANX_RESOURCE_HANDLE_T dispman_resource;
                dispman_resource = vc_dispmanx_resource_create(VC_IMAGE_RGBA32, _viewport.z, _viewport.w, &dest_image_handle);
                dispman_display = vc_dispmanx_display_open_offscreen(dispman_resource, DISPMANX_NO_ROTATE);
            } else {
                dispman_display = vc_dispmanx_display_open(0); // LCD
            }

            VC_DISPMANX_ALPHA_T alpha = { 
                DISPMANX_FLAGS_ALPHA_FROM_SOURCE | DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS, 
                0, //255, 
                0 
            };

            dispman_update = vc_dispmanx_update_start(0);
            dispman_element = vc_dispmanx_element_add(  dispman_update, dispman_display,
                                                        0/*layer*/, &dst_rect, 0/*src*/,
                                                        &src_rect, DISPMANX_PROTECTION_NONE,
                                                        &alpha , 0/*clamp*/, (DISPMANX_TRANSFORM_T)0/*transform*/);

            nativeviewport.element = dispman_element;
            nativeviewport.width = _viewport.z;
            nativeviewport.height = _viewport.w;
            vc_dispmanx_update_submit_sync( dispman_update );
            check();

            surface = eglCreateWindowSurface( display, config, &nativeviewport, NULL );
            assert(surface != EGL_NO_SURFACE);
            check();

        #elif defined(DRIVER_GBM)
            surface = eglCreateWindowSurface(display, config, gbmSurface, NULL);
            if (surface == EGL_NO_SURFACE) {
                std::cerr << "Failed to create EGL surface! Error: " << eglGetErrorStr() << std::endl;
                eglDestroyContext(display, context);
                eglTerminate(display);
                gbmClean();
                return EXIT_FAILURE;
            }
        #endif

        // connect the context to the surface
        result = eglMakeCurrent(display, surface, surface, context);
        assert(EGL_FALSE != result);
        check();

    // GLFW
    #else

        glfwSetErrorCallback([](int err, const char* msg)->void {
            std::cerr << "GLFW error 0x"<<std::hex<<err<<std::dec<<": "<<msg<<"\n";
        });
        if(!glfwInit()) {
            std::cerr << "ABORT: GLFW init failed" << std::endl;
            exit(-1);
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, _prop.major);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, _prop.minor);
        if (_prop.major >= 3 && _prop.minor >= 2) {
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
        }

        if (_prop.msaa != 0)
            glfwWindowHint(GLFW_SAMPLES, _prop.msaa);

        if (_prop.style == HEADLESS)
            glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

        else if (_prop.style == UNDECORATED )
            glfwWindowHint(GLFW_DECORATED, GL_FALSE);
            
        else if (_prop.style == ALLWAYS_ON_TOP )
            glfwWindowHint(GLFW_FLOATING, GL_TRUE);

        else if (_prop.style == UNDECORATED_ALLWAYS_ON_TOP) {
            glfwWindowHint(GLFW_DECORATED, GL_FALSE);
            glfwWindowHint(GLFW_FLOATING, GL_TRUE);
        }
        
        if (_prop.style == FULLSCREEN) {
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            _viewport.z = mode->width;
            _viewport.w = mode->height;
            glfwWindowHint(GLFW_RED_BITS, mode->redBits);
            glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
            glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
            glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
            window = glfwCreateWindow(_viewport.z, _viewport.w, "", monitor, NULL);
        }
        else if (_prop.style == LENTICULAR) {
            glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
            glfwWindowHint(GLFW_DECORATED, false);

            int count;
            GLFWmonitor **monitors = glfwGetMonitors(&count);
            int monitorID = 1;

            if(count > 2){ //if we have more than 2 screens, try and find the looking glass screen ID 
                monitorID = -1;
                for (int i = 0; i < count; i++){
                    const char* name = glfwGetMonitorName(monitors[i]);
                    if(name && strlen(name) >= 3){ // if monitor name is > than 3 chars
                        if(name[0] == 'L' && name[1] == 'K' && name[2] == 'G'){ // found a match for the looking glass screen
                            monitorID = i;
                        }
                    }
                }
                if(monitorID == -1){ //could not find the looking glass screen
                    monitorID = 1;
                }
            }

            const GLFWvidmode* mode = glfwGetVideoMode(monitors[monitorID]);
            _viewport.z = mode->width;
            _viewport.w = mode->height;

            int xpos, ypos;
            glfwGetMonitorPos(monitors[monitorID], &xpos, &ypos);
            window = glfwCreateWindow(_viewport.z, _viewport.w, "", NULL, NULL);
            
            if (_viewport.x != 0 || _viewport.y != 0) {
                xpos += _viewport.x;
                ypos += _viewport.y;
            }

            glfwSetWindowPos(window, xpos, ypos);
        }
        else
            window = glfwCreateWindow(_viewport.z, _viewport.w, "", NULL, NULL);

        if (!window) {
            glfwTerminate();
            std::cerr << "ABORT: GLFW create window failed" << std::endl;
            exit(-1);
        }

        glfwMakeContextCurrent(window);
        #ifdef _WIN32
            glewInit();
        #endif

        #ifdef EVENTS_AS_CALLBACKS
        if (onKeyPress)
        #endif 
        glfwSetKeyCallback(window, [](GLFWwindow* _window, int _key, int _scancode, int _action, int _mods) {
            if (_action == GLFW_PRESS && (_key == GLFW_KEY_LEFT_SHIFT || GLFW_KEY_RIGHT_SHIFT) )
                bShift = true;
            else if (_action == GLFW_RELEASE && (_key == GLFW_KEY_LEFT_SHIFT || GLFW_KEY_RIGHT_SHIFT) )
                bShift = false;

            else if (_action == GLFW_PRESS && (_key == GLFW_KEY_LEFT_CONTROL || GLFW_KEY_RIGHT_CONTROL) )
                bControl = true;
            else if (_action == GLFW_RELEASE && (_key == GLFW_KEY_LEFT_CONTROL || GLFW_KEY_RIGHT_CONTROL) )
                bControl = false;

            else if (_action == GLFW_PRESS)
                onKeyPress(_key);
        });

        // callback when a mouse button is pressed or released
        glfwSetMouseButtonCallback(window, [](GLFWwindow* _window, int button, int action, int mods) {
            if (button == GLFW_MOUSE_BUTTON_1) {
                // update mouse4 when left mouse button is pressed or released
                if (action == GLFW_PRESS && !left_mouse_button_down) {
                    left_mouse_button_down = true;
                    mouse4.x = mouse.x;
                    mouse4.y = mouse.y;
                    mouse4.z = mouse.x;
                    mouse4.w = mouse.y;
                } else if (action == GLFW_RELEASE && left_mouse_button_down) {
                    left_mouse_button_down = false;
                    mouse4.z = -mouse4.z;
                    mouse4.w = -mouse4.w;
                }
            }
            if (action == GLFW_PRESS) {
                mouse.drag.x = mouse.x;
                mouse.drag.y = mouse.y;
            }
        });

        #ifdef EVENTS_AS_CALLBACKS
        if (onScroll)
        #endif 
        glfwSetScrollCallback(window, [](GLFWwindow* _window, double xoffset, double yoffset) {
            onScroll(-yoffset * fPixelDensity);
        });

#ifndef __EMSCRIPTEN__
        glfwSetWindowPosCallback(window, [](GLFWwindow* _window, int x, int y) {
            if (fPixelDensity != getPixelDensity()) {
                updateViewport();
            }
        });

        glfwSetWindowSizeCallback(window, [](GLFWwindow* _window, int _w, int _h) {
            setViewport(_w,_h);
        });

        // callback when the mouse cursor enters/leaves
        glfwSetCursorEnterCallback(window, [](GLFWwindow* _window, int entered) {
            mouse.entered = (bool)entered;
        });


        glfwSetMouseButtonCallback(window, [](GLFWwindow* _window, int _button, int _action, int _mods) {
            mouse.button = _button;
            if (_action == GLFW_PRESS) {
                #ifdef EVENTS_AS_CALLBACKS
                if (onMousePress)
                #endif 
                onMousePress(mouse.x, mouse.y, mouse.button);
            }
            else {
                #ifdef EVENTS_AS_CALLBACKS
                if (onMouseRelease)
                #endif
                onMouseRelease(mouse.x, mouse.y, mouse.button);
            }
        });	

        // callback when the mouse cursor moves
        glfwSetCursorPosCallback(window, [](GLFWwindow* _window, double x, double y) {
            // Convert x,y to pixel coordinates relative to viewport.
            // (0,0) is lower left corner.
            y = viewport.w - y;
            x *= fPixelDensity;
            y *= fPixelDensity;
            // mouse.velX,mouse.velY is the distance the mouse cursor has moved
            // since the last callback, during a drag gesture.
            // mouse.drag is the previous mouse position, during a drag gesture.
            // Note that mouse.drag is *not* constrained to the viewport.
            mouse.velX = x - mouse.drag.x;
            mouse.velY = y - mouse.drag.y;
            mouse.drag.x = x;
            mouse.drag.y = y;

            // mouse.x,mouse.y is the current cursor position, constrained
            // to the viewport.
            mouse.x = x;
            mouse.y = y;
            if (mouse.x < 0) mouse.x = 0;
            if (mouse.y < 0) mouse.y = 0;
            if (mouse.x > getWindowWidth()) mouse.x = getWindowWidth();
            if (mouse.y > getWindowHeight()) mouse.y = getWindowHeight();

            // update mouse4 when cursor moves
            if (left_mouse_button_down) {
                mouse4.x = mouse.x;
                mouse4.y = mouse.y;
            }

            /*
             * TODO: the following code would best be moved into the
             * mouse button callback. If you click the mouse button without
             * moving the mouse, then using this code, the mouse click doesn't
             * register until the cursor is moved. (@doug-moen)
             */
            int action1 = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1);
            int action2 = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2);
            int button = 0;

            if (action1 == GLFW_PRESS) button = 1;
            else if (action2 == GLFW_PRESS) button = 2;

            // Lunch events
            if (mouse.button == 0 && button != mouse.button) {
                mouse.button = button;
                #ifdef EVENTS_AS_CALLBACKS
                if (onMousePress)
                #endif 
                onMousePress(mouse.x, mouse.y, mouse.button);
            }
            else {
                mouse.button = button;
            }

            if (mouse.velX != 0.0 || mouse.velY != 0.0) {
                if (button != 0) {
                    #ifdef EVENTS_AS_CALLBACKS
                    if (onMouseDrag)
                    #endif 
                    onMouseDrag(mouse.x, mouse.y, mouse.button);
                }
                else {
                    #ifdef EVENTS_AS_CALLBACKS
                    if (onMouseMove)
                    #endif 
                    onMouseMove(mouse.x, mouse.y);
                }
            }
        });

#else
        enable_extension("OES_texture_float");
        enable_extension("OES_texture_half_float");
        enable_extension("OES_standard_derivatives");
        enable_extension("OES_texture_float_linear");
        enable_extension("OES_texture_half_float_linear");
        
        emscripten_set_mousedown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, true, mouse_callback);
        emscripten_set_mouseup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, true, mouse_callback);
        emscripten_set_mousemove_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, true, mouse_callback);

        emscripten_set_touchstart_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, true, touch_callback);
        emscripten_set_touchend_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, true, touch_callback);
        emscripten_set_touchmove_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, true, touch_callback);
        emscripten_set_touchcancel_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, true, touch_callback);

        EmscriptenFullscreenStrategy strategy{};

        strategy.scaleMode = EMSCRIPTEN_FULLSCREEN_SCALE_STRETCH;
        strategy.canvasResolutionScaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_STDDEF;
        // strategy.canvasResolutionScaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_HIDEF;
        strategy.filteringMode = EMSCRIPTEN_FULLSCREEN_FILTERING_DEFAULT;

        strategy.canvasResizedCallback = on_canvassize_changed;
        strategy.canvasResizedCallbackUserData = NULL;

        emscripten_enter_soft_fullscreen("#canvas", &strategy);
#endif
        
    #endif

    setViewport(_viewport.z, _viewport.w);

    return 0;
}
// get Time Function
double getTimeSec() {
    timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    timespec temp;
    if ((now.tv_nsec-time_start.tv_nsec)<0) {
        temp.tv_sec = now.tv_sec-time_start.tv_sec-1;
        temp.tv_nsec = 1000000000+now.tv_nsec-time_start.tv_nsec;
    } else {
        temp.tv_sec = now.tv_sec-time_start.tv_sec;
        temp.tv_nsec = now.tv_nsec-time_start.tv_nsec;
    }
    return double(temp.tv_sec) + double(temp.tv_nsec/1000000000.);
}

bool isGL() {
 
    #if defined(DRIVER_GLFW)
        return !glfwWindowShouldClose(window);

    #elif defined(DRIVER_BROADCOM)
        return bHostInited;

    #elif defined(DRIVER_GBM)
        return true;

    #endif
}

void updateGL() {
    // Update time
    // --------------------------------------------------------------------
    double now = getTimeSec();
    float diff = now - elapseTime;
    if (diff < restSec) {
        sleep_ms(int((restSec - diff) * 1000000));
        now = getTimeSec();
    }    

    delta = now - elapseTime;
    elapseTime = now;

    static int frame_count = 0;
    static double lastTime = 0.0;
    frame_count++;
    lastTime += delta;
    if (lastTime >= 1.) {
        FPS = double(frame_count);
        frame_count = 0;
        lastTime -= 1.;
    }

    // EVENTS
    // --------------------------------------------------------------------
    #if defined(DRIVER_GLFW)
        glfwPollEvents();

    #else
        #ifdef EVENTS_AS_CALLBACKS
        if (onMousePress || onMouseDrag || onMouseMove)
        #endif 
        {
            const int XSIGN = 1<<4, YSIGN = 1<<5;
            static int fd = -1;
            if (fd < 0) {
                fd = open(properties.mouse.c_str(),O_RDONLY|O_NONBLOCK);
            }

            if (fd >= 0) {
                // Set values to 0
                mouse.velX=0;
                mouse.velY=0;

                // Extract values from driver
                struct {char buttons, dx, dy; } m;
                while (1) {
                    int bytes = read(fd, &m, sizeof m);

                    if (bytes < (int)sizeof m)
                        return;
                    else if (m.buttons&8) 
                        break; // This bit should always be set

                    read(fd, &m, 1); // Try to sync up again
                }

                // Set button value
                int button = m.buttons&3;
                if (button) mouse.button = button;
                else mouse.button = 0;

                // Set deltas
                mouse.velX=m.dx;
                mouse.velY=m.dy;
                if (m.buttons&XSIGN) mouse.velX-=256;
                if (m.buttons&YSIGN) mouse.velY-=256;

                // Add movement
                mouse.x += mouse.velX;
                mouse.y += mouse.velY;

                // Clamp values
                if (mouse.x < 0) mouse.x=0;
                if (mouse.y < 0) mouse.y=0;
                if (mouse.x > viewport.z) mouse.x = viewport.z;
                if (mouse.y > viewport.w) mouse.y = viewport.w;

                // Lunch events
                if (mouse.button == 0 && button != mouse.button) {
                    mouse.button = button;
                    #ifdef EVENTS_AS_CALLBACKS
                    if (onMousePress)
                    #endif 
                    onMousePress(mouse.x, mouse.y, mouse.button);
                }
                else
                    mouse.button = button;

                if (mouse.velX != 0.0 || mouse.velY != 0.0) {
                    if (button != 0) {
                        #ifdef EVENTS_AS_CALLBACKS
                        if (onMouseDrag)
                        #endif 
                        onMouseDrag(mouse.x, mouse.y, mouse.button);
                    }
                    else {
                        #ifdef EVENTS_AS_CALLBACKS
                        if (onMouseMove)
                        #endif 
                        onMouseMove(mouse.x, mouse.y);
                    }
                }
            }
        }
    #endif
}

void renderGL(){
    // NON GLFW
    #if defined(DRIVER_GLFW)
        glfwSwapBuffers(window);

    #else
        eglSwapBuffers(display, surface);
        #if defined(DRIVER_GBM)
        gbmSwapBuffers();
        #endif

    #endif
}

void closeGL(){
    // NON GLFW
    #if defined(DRIVER_GLFW)
        glfwSetWindowShouldClose(window, GL_TRUE);
        glfwTerminate();

    #else
        eglSwapBuffers(display, surface);

        // Release OpenGL resources
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroySurface(display, surface);
        eglDestroyContext(display, context);
        eglTerminate(display);
        eglReleaseThread();

        #if defined(DRIVER_BROADCOM)
        vc_dispmanx_display_close(dispman_display);
        bcm_host_deinit();

        #elif defined(DRIVER_GBM)
        gbmClean();
        close(device);
        #endif

    #endif
}


//-------------------------------------------------------------
void setWindowSize(int _width, int _height) {
    #if defined(DRIVER_GLFW) 
    glfwSetWindowSize(window, _width / getPixelDensity(), _height / getPixelDensity());
    #endif
    setViewport((float)_width / getPixelDensity(), (float)_height / getPixelDensity());
}

void setWindowTitle( const char* _title) {
    #if defined(DRIVER_GLFW)
    glfwSetWindowTitle(window, _title);
    #endif
}

void setWindowVSync(bool _value) {
    #if defined(DRIVER_GLFW)
    glfwSwapInterval(_value);
    #endif
}

void setViewport(float _width, float _height) {
    viewport.z = _width;
    viewport.w = _height;
    updateViewport();
}

void updateViewport() {
    fPixelDensity = getPixelDensity();
    float width = getWindowWidth();
    float height = getWindowHeight();

    glViewport( (float)viewport.x * fPixelDensity, (float)viewport.y * fPixelDensity,
                width, height);
                
    orthoMatrix = glm::ortho(   (float)viewport.x * fPixelDensity, width, 
                                (float)viewport.y * fPixelDensity, height);

#ifdef EVENTS_AS_CALLBACKS
    if (onViewportResize)
#endif 
    onViewportResize(width, height);
}

glm::ivec2 getScreenSize() {
    glm::ivec2 screen;

    #if defined(DRIVER_GLFW)
        // glfwGetMonitorPhysicalSize(glfwGetPrimaryMonitor(), &screen.x, &screen.y);
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        screen.x = mode->width;
        screen.y = mode->height;

    #elif defined(DRIVER_BROADCOM)
        initHost();

        uint32_t screen_width;
        uint32_t screen_height;
        int32_t success = graphics_get_display_size(0 /* LCD */, &screen_width, &screen_height);
        assert(success >= 0);
        screen = glm::ivec2(screen_width, screen_height);

    #elif defined(DRIVER_GBM)
        initHost();

        screen = glm::ivec2(mode.hdisplay, mode.vdisplay);

    #endif

    return screen;
}

float getPixelDensity() {
#if defined(DRIVER_GLFW)
    #if defined(__EMSCRIPTEN__)
        return 1.0;
        // return emscripten_get_device_pixel_ratio();
    #else
        int window_width, window_height, framebuffer_width, framebuffer_height;
        glfwGetWindowSize(window, &window_width, &window_height);
        glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
        return float(framebuffer_width)/float(window_width);
    #endif
#else
        return 1.0f;
#endif
}

const glm::ivec4& getViewport() { return viewport; }
const glm::mat4& getOrthoMatrix() { return orthoMatrix; }
int         getWindowWidth() { return viewport.z * fPixelDensity; }
int         getWindowHeight() { return viewport.w * fPixelDensity; }
int         getWindowMSAA() { return properties.msaa; }

std::string getVendor() {
    if (properties.vendor == "") properties.vendor = std::string((const char*)glGetString(GL_VENDOR));
    return properties.vendor;
}

std::string getRenderer() {
    if (properties.renderer == "") properties.renderer = std::string((const char*)glGetString(GL_RENDERER));
    return properties.renderer;
}

std::string getGLVersion() {
    if (properties.version == "") properties.version = std::string((const char*)glGetString(GL_VERSION));
    return properties.version;
}

std::string getGLSLVersion() {
    if (properties.glsl == "") properties.glsl = std::string((const char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
    return properties.glsl;
}

std::string getExtensions() { 
    if (properties.extensions == "") properties.extensions = std::string((const char*)glGetString(GL_EXTENSIONS));
    return properties.extensions; 
}

bool haveExtension(std::string _name) { 
    if (properties.extensions == "") properties.extensions = std::string((const char*)glGetString(GL_EXTENSIONS));
    return properties.extensions.find(_name) == std::string::npos; 
}

#if defined(__EMSCRIPTEN__)
size_t getWebGLVersionNumber() {
    if (properties.webgl == 0) properties.webgl = (beginsWith( getGLVersion(), "OpenGL ES 2.0"))? 1 : 2 ;
    return properties.webgl;
}
#endif

glm::vec4 getDate() {
    #ifdef _MSC_VER
        time_t tv = time(NULL);

        struct tm tm_struct;
        struct tm* tm = &tm_struct;
        errno_t err = localtime_s(tm, &tv);
        if (err)
        {
                
        }

        return glm::vec4(tm->tm_year + 1900,
            tm->tm_mon,
            tm->tm_mday,
            tm->tm_hour * 3600.0f + tm->tm_min * 60.0f + tm->tm_sec);
    #else
        gettimeofday(&tv, NULL);
        struct tm *tm;
        tm = localtime(&tv.tv_sec);
        // std::cout << "y: " << tm->tm_year+1900 << " m: " << tm->tm_mon << " d: " << tm->tm_mday << " s: " << tm->tm_hour*3600.0f+tm->tm_min*60.0f+tm->tm_sec+tv.tv_usec*0.000001 << std::endl;
        return glm::vec4(tm->tm_year + 1900,
            tm->tm_mon,
            tm->tm_mday,
            tm->tm_hour * 3600.0f + tm->tm_min * 60.0f + tm->tm_sec + tv.tv_usec * 0.000001);
    #endif 
}

double  getTime() { return elapseTime;}
double  getDelta() { return delta; }

void    setFps(int _fps) { 
    if (_fps == 0)
        restSec = 0.0;
    else
        restSec = 1.0f/(float)_fps; 
}
double  getFps() { return FPS; }

float   getRestSec() { return restSec; }
int     getRestMs() { return restSec * 1000; }
int     getRestUs() { return restSec * 1000000; }

void    setMousePosition( float _x, float _y ) {
    mouse.x = _x;
    mouse.y = _y;
    mouse.velX = 0.0f;
    mouse.velY = 0.0f;
    mouse.drag.x = _x;
    mouse.drag.y = _y;
    #if defined(DRIVER_GLFW)
    float y = glm::clamp(_y, 0.0f, (float)getWindowHeight());
    glfwSetCursorPos(window, _x, getWindowHeight() - y);
    #endif
}

void    setMousePosition( glm::vec2 _pos ) { setMousePosition(_pos.x, _pos.y); }

float   getMouseX(){ return mouse.x; }
float   getMouseY(){ return mouse.y; }
glm::vec2 getMousePosition() { return glm::vec2(mouse.x,mouse.y); }
float   getMouseVelX(){ return mouse.velX; }
float   getMouseVelY(){ return mouse.velY;}
glm::vec2 getMouseVelocity() { return glm::vec2(mouse.velX,mouse.velY);}
int     getMouseButton(){ return mouse.button;}
glm::vec4 getMouse4() {return mouse4;}
bool    getMouseEntered() { return mouse.entered; }

bool    isShiftPressed() { return bShift; }
bool    isControlPressed() { return bControl; }

}
