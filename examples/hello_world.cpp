#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
// #define GLFW_INCLUDE_ES3
#endif

#include <iostream>
#include <string>

#include "ada/window.h"
#include "ada/gl/gl.h"
#include "ada/gl/mesh.h"
#include "ada/gl/shader.h"
#include "ada/tools/text.h"

ada::Mesh rect (float _x, float _y, float _w, float _h) {
    float x = _x-1.0f;
    float y = _y-1.0f;
    float w = _w*2.0f;
    float h = _h*2.0f;

    ada::Mesh mesh;
    mesh.addVertex(glm::vec3(x, y, 0.0));
    // mesh.addColor(glm::vec4(1.0));
    // mesh.addNormal(glm::vec3(0.0, 0.0, 1.0));
    mesh.addTexCoord(glm::vec2(0.0, 0.0));

    mesh.addVertex(glm::vec3(x+w, y, 0.0));
    // mesh.addColor(glm::vec4(1.0));
    // mesh.addNormal(glm::vec3(0.0, 0.0, 1.0));
    mesh.addTexCoord(glm::vec2(1.0, 0.0));

    mesh.addVertex(glm::vec3(x+w, y+h, 0.0));
    // mesh.addColor(glm::vec4(1.0));
    // mesh.addNormal(glm::vec3(0.0, 0.0, 1.0));
    mesh.addTexCoord(glm::vec2(1.0, 1.0));

    mesh.addVertex(glm::vec3(x, y+h, 0.0));
    // mesh.addColor(glm::vec4(1.0));
    // mesh.addNormal(glm::vec3(0.0, 0.0, 1.0));
    mesh.addTexCoord(glm::vec2(0.0, 1.0));

    mesh.addIndex(0);   mesh.addIndex(1);   mesh.addIndex(2);
    mesh.addIndex(2);   mesh.addIndex(3);   mesh.addIndex(0);

    return mesh;
}

const std::string vert = R"(
#ifdef GL_ES
precision mediump float;
#endif

attribute vec4 a_position;
attribute vec2 a_texcoord;

varying vec4 v_position;
varying vec2 v_texcoord;

void main(void) {
    v_position =  a_position;
    v_texcoord = a_texcoord;
    
    gl_Position = v_position;
}
)";

const std::string frag = R"(
#ifdef GL_ES
precision mediump float;
#endif

uniform vec2    u_resolution;
uniform float   u_time;

varying vec2    v_texcoord;

void main(void) {
    vec3 color = vec3(1.0);
    vec2 st = gl_FragCoord.xy/u_resolution.xy;
    st = v_texcoord;
    
    color = vec3(st.x,st.y,abs(sin(u_time)));
    
    gl_FragColor = vec4(color, 1.0);
}
)";

ada::Vbo* billboard_vbo = rect(0.0,0.0,1.0,1.0).getVbo();
ada::Shader shader;

static void generate_frame() {
    // Update
        ada::updateGL();

    shader.setUniform("u_resolution", (float)ada::getWindowWidth(), (float)ada::getWindowHeight() );
    shader.setUniform("u_time", (float)ada::getTime());

    billboard_vbo->render( &shader );

    ada::renderGL();
    ada::updateViewport();
}


int main(int argc, char **argv) {
    // Set the size and type of window
    glm::ivec4 window_viewport = glm::ivec4(0);
    window_viewport.z = 512;
    window_viewport.w = 512;

    #if defined(DRIVER_BROADCOM) || defined(DRIVER_GBM) 
    // RASPBERRYPI default windows size (fullscreen)
    glm::ivec2 screen = ada::getScreenSize();
    window_viewport.z = screen.x;
    window_viewport.w = screen.y;
    #endif

    ada::WindowProperties window_properties;
    for (int i = 1; i < argc ; i++) {
        std::string argument = std::string(argv[i]);

        if (        std::string(argv[i]) == "-x" ) {
            if (++i < argc)
                window_viewport.x = ada::toInt(std::string(argv[i]));
            else
                std::cout << "Argument '" << argument << "' should be followed by a <pixels>. Skipping argument." << std::endl;
        }
        else if (   std::string(argv[i]) == "-y" ) {
            if (++i < argc)
                window_viewport.y = ada::toInt(std::string(argv[i]));
            else
                std::cout << "Argument '" << argument << "' should be followed by a <pixels>. Skipping argument." << std::endl;
        }
        else if (   std::string(argv[i]) == "-w" ||
                    std::string(argv[i]) == "--width" ) {
            if (++i < argc)
                window_viewport.z = ada::toInt(std::string(argv[i]));
            else
                std::cout << "Argument '" << argument << "' should be followed by a <pixels>. Skipping argument." << std::endl;
        }
        else if (   std::string(argv[i]) == "-h" ||
                    std::string(argv[i]) == "--height" ) {
            if (++i < argc)
                window_viewport.w = ada::toInt(std::string(argv[i]));
            else
                std::cout << "Argument '" << argument << "' should be followed by a <pixels>. Skipping argument." << std::endl;
        }
        #if defined(DRIVER_GBM) 
        else if (   std::string(argv[i]) == "--display") {
            if (++i < argc)
                window_properties.display = std::string(argv[i]);
            else
                std::cout << "Argument '" << argument << "' should be followed by a the display address. Skipping argument." << std::endl;
        }
        #endif
        #if !defined(DRIVER_GLFW)
        else if (   std::string(argv[i]) == "--mouse") {
            if (++i < argc)
                window_properties.mouse = std::string(argv[i]);
            else
                std::cout << "Argument '" << argument << "' should be followed by a the mouse address. Skipping argument." << std::endl;
        }
        #endif
        else if (   std::string(argv[i]) == "--headless" ) {
            window_properties.style = ada::HEADLESS;
        }
        else if (   std::string(argv[i]) == "-f" ||
                    std::string(argv[i]) == "--fullscreen" ) {
            window_properties.style = ada::FULLSCREEN;
        }
        else if (   std::string(argv[i]) == "--holoplay") {
            window_properties.style = ada::HOLOPLAY;
        }
        else if (   std::string(argv[i]) == "-ss" ||
                    std::string(argv[i]) == "--screensaver") {
            window_properties.style = ada::FULLSCREEN;
        }
        else if (   std::string(argv[i]) == "--msaa") {
            window_properties.msaa = 4;
        }
        else if (   std::string(argv[i]) == "--major") {
            if (++i < argc)
                window_properties.major = ada::toInt(std::string(argv[i]));
            else
                std::cout << "Argument '" << argument << "' should be followed by a the OPENGL MAJOR version. Skipping argument." << std::endl;
        }
        else if (   std::string(argv[i]) == "--minor") {
            if (++i < argc)
                window_properties.minor = ada::toInt(std::string(argv[i]));
            else
                std::cout << "Argument '" << argument << "' should be followed by a the OPENGL MINOR version. Skipping argument." << std::endl;
        }
    }

    // Initialize openGL context
    ada::initGL(window_viewport, window_properties);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // ada::Vbo* billboard_vbo = rect(0.0,0.0,1.0,1.0).getVbo();
    // ada::Shader shader;
    shader.load(frag, vert);
    shader.use();

      // Run the loop correctly for the target environmen

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(generate_frame, 0, false);
#else
    
    // Render Loop
    while ( ada::isGL() ) {
        generate_frame();
    }

    ada::closeGL();

#endif

    return 1;
}

// Events
//============================================================================
void ada::onKeyPress (int _key) {
}

void ada::onMouseMove(float _x, float _y) {
}

void ada::onMouseClick(float _x, float _y, int _button) {
}

void ada::onScroll(float _yoffset) {
}

void ada::onMouseDrag(float _x, float _y, int _button) {
}

void ada::onViewportResize(int _newWidth, int _newHeight) {
}