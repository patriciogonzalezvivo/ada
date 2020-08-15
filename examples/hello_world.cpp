#include <string>
#include "glm/glm.hpp"

#include "ada/window.h"
#include "ada/gl/gl.h"
#include "ada/gl/mesh.h"
#include "ada/gl/shader.h"

Mesh rect (float _x, float _y, float _w, float _h) {
    float x = _x-1.0f;
    float y = _y-1.0f;
    float w = _w*2.0f;
    float h = _h*2.0f;

    Mesh mesh;
    mesh.addVertex(glm::vec3(x, y, 0.0));
    mesh.addColor(glm::vec4(1.0));
    mesh.addNormal(glm::vec3(0.0, 0.0, 1.0));
    mesh.addTexCoord(glm::vec2(0.0, 0.0));

    mesh.addVertex(glm::vec3(x+w, y, 0.0));
    mesh.addColor(glm::vec4(1.0));
    mesh.addNormal(glm::vec3(0.0, 0.0, 1.0));
    mesh.addTexCoord(glm::vec2(1.0, 0.0));

    mesh.addVertex(glm::vec3(x+w, y+h, 0.0));
    mesh.addColor(glm::vec4(1.0));
    mesh.addNormal(glm::vec3(0.0, 0.0, 1.0));
    mesh.addTexCoord(glm::vec2(1.0, 1.0));

    mesh.addVertex(glm::vec3(x, y+h, 0.0));
    mesh.addColor(glm::vec4(1.0));
    mesh.addNormal(glm::vec3(0.0, 0.0, 1.0));
    mesh.addTexCoord(glm::vec2(0.0, 1.0));

    mesh.addIndex(0);   mesh.addIndex(1);   mesh.addIndex(2);
    mesh.addIndex(2);   mesh.addIndex(3);   mesh.addIndex(0);

    return mesh;
}


static const std::string vert = R"(
#ifdef GL_ES
precision mediump float;
#endif

attribute vec4 a_position;
attribute vec2 a_texcoord;

varying vec4 v_position;
// varying vec4 v_color;
// varying vec3 v_normal;
varying vec2 v_texcoord;

void main(void) {
    v_position =  a_position;
    // v_color = vec4(1.0);
    // v_normal = vec3(0.0,0.0,1.0);
    v_texcoord = a_texcoord;
    
    gl_Position = v_position;
}
)";

const std::string frag = R"(
#ifdef GL_ES
precision mediump float;
#endif

uniform vec2    u_resolution;
uniform vec2    u_mouse;
uniform float   u_time;

varying vec2    v_texcoord;

void main(void) {
    vec3 color = vec3(1.0);
    vec2 st = gl_FragCoord.xy/u_resolution.xy;
    st.x *= u_resolution.x/u_resolution.y;
    st = v_texcoord;
    
    color = vec3(st.x,st.y,abs(sin(u_time)));
    
    gl_FragColor = vec4(color, 1.0);
}
)";



int main(int argc, char **argv) {

    glm::ivec4 windowPosAndSize = glm::ivec4(0);
    #if defined(DRIVER_VC) || defined(DRIVER_GBM) 
        // RASPBERRYPI default windows size (fullscreen)
        glm::ivec2 screen = getScreenSize();
        windowPosAndSize.z = screen.x;
        windowPosAndSize.w = screen.y;
    #else
        // OSX/LINUX default windows size
        windowPosAndSize.z = 500;
        windowPosAndSize.w = 500;
    #endif

    // Initialize openGL context
    initGL ("triangle", windowPosAndSize);

    Vbo* billboard_vbo = rect(0.0,0.0,1.0,1.0).getVbo();
    Shader shader;
    shader.load(frag, vert);
    shader.use();

    // Render Loop
    while ( isGL() ) {
        // Update
        updateGL();

        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        shader.setUniform("u_resolution", getWindowWidth(), getWindowHeight() );
        shader.setUniform("u_time", (float)getTime());

        billboard_vbo->render( &shader );

        renderGL();
    }

    return 1;
}

// Events
//============================================================================
void onKeyPress (int _key) {
}

void onMouseMove(float _x, float _y) {
}

void onMouseClick(float _x, float _y, int _button) {
}

void onScroll(float _yoffset) {
}

void onMouseDrag(float _x, float _y, int _button) {
}

void onViewportResize(int _newWidth, int _newHeight) {
}

void onExit() {
    // clear screen
    glClear( GL_COLOR_BUFFER_BIT );

    // close openGL instance
    closeGL();
}