#include "ada/tools/holoplay.h"
#include "ada/gl/gl.h"

namespace ada {

static HoloplayProperties holoplay;

void setHoloplayProperties(const HoloplayProperties& _holoplay) {
    holoplay = _holoplay;
}

void setHoloplayResolution(int _holoplay) {
    if (_holoplay == 0) {
        holoplay.width = 2048;
        holoplay.height = 2048;
        holoplay.columns = 4;
        holoplay.rows = 8;
        holoplay.totalViews = 32;
    }
    else if (_holoplay == 1) {
        holoplay.width = 4096;
        holoplay.height = 4096;
        holoplay.columns = 5;
        holoplay.rows = 9;
        holoplay.totalViews = 45;
    }
    else if (_holoplay == 2) {
        holoplay.width = 4096 * 2;
        holoplay.height = 4096 * 2;
        holoplay.columns = 5;
        holoplay.rows = 9;
        holoplay.totalViews = 45;
    }
    else if (_holoplay == 3) {
        holoplay.width = 3360;
        holoplay.height = 3360;
        holoplay.columns = 8;
        holoplay.rows = 6;
        holoplay.totalViews = 48;
    }
    else if (_holoplay == 4) {
        holoplay.width = 4026;
        holoplay.height = 4096;
        holoplay.columns = 11;
        holoplay.rows = 8;
        holoplay.totalViews = 88;
    }
    else if (_holoplay == 5) {
        holoplay.width = 4225;
        holoplay.height = 4095;
        holoplay.columns = 13;
        holoplay.rows = 7;
        holoplay.totalViews = 91;
    }
    else if (_holoplay == 6) {
        holoplay.width = 4224;
        holoplay.height = 4096;
        holoplay.columns = 12;
        holoplay.rows = 8;
        holoplay.totalViews = 96;
    }
    else if (_holoplay == 7) {
        holoplay.width = 4224;
        holoplay.height = 4230;
        holoplay.columns = 12;
        holoplay.rows = 9;
        holoplay.totalViews = 108;
    }
}

int getHoloplayWidth() { return holoplay.width; }
int getHoloplayHeight() { return holoplay.height; }

void holoplayQuilt(std::function<void(const HoloplayProperties&, glm::vec4&, int&)> _renderFnc) {

    // save the viewport for the total quilt
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    // get quilt view dimensions
    int qs_viewWidth = int(float(holoplay.width) / float(holoplay.columns));
    int qs_viewHeight = int(float(holoplay.height) / float(holoplay.rows));

    // render views and copy each view to the quilt
    for (int viewIndex = 0; viewIndex < holoplay.totalViews; viewIndex++) {
        // get the x and y origin for this view
        int x = (viewIndex % holoplay.columns) * qs_viewWidth;
        int y = int(float(viewIndex) / float(holoplay.columns)) * qs_viewHeight;

        // get the x and y origin for this view
        // set the viewport to the view to control the projection extent
        glViewport(x, y, qs_viewWidth, qs_viewHeight);

        // // set the scissor to the view to restrict calls like glClear from making modifications
        glEnable(GL_SCISSOR_TEST);
        glScissor(x, y, qs_viewWidth, qs_viewHeight);
        glm::vec4 vp = glm::vec4(x, y, qs_viewWidth, qs_viewHeight);

        _renderFnc(holoplay, vp, viewIndex);

        // reset viewport
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

        // // restore scissor
        glDisable(GL_SCISSOR_TEST);
        glScissor(viewport[0], viewport[1], viewport[2], viewport[3]);
    }
}

const std::string holoplay_frag = R"(
#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D   u_scene;
uniform vec2        u_resolution;

uniform vec3        u_holoPlayTile;
uniform vec4        u_holoPlayCalibration;  // dpi, pitch, slope, center
uniform vec2        u_holoPlayRB;           // ri, bi

// GET CORRECT VIEW
vec2 quilt_map(vec3 tile, vec2 pos, float a) {
    vec2 tile2 = vec2(tile.x - 1.0, tile.y - 1.0);
    vec2 dir = vec2(-1.0, -1.0);

    a = fract(a) * tile.y;
    tile2.y += dir.y * floor(a);
    a = fract(a) * tile.x;
    tile2.x += dir.x * floor(a);
    return (tile2 + pos) / tile.xy;
}

void main (void) {
    vec3 color = vec3(0.0);
    vec2 st = gl_FragCoord.xy/u_resolution.xy;

    float pitch = -u_resolution.x / u_holoPlayCalibration.x  * u_holoPlayCalibration.y * sin(atan(abs(u_holoPlayCalibration.z)));
    float tilt = u_resolution.y / (u_resolution.x * u_holoPlayCalibration.z);
    float subp = 1.0 / (3.0 * u_resolution.x);
    float subp2 = subp * pitch;

    float a = (-st.x - st.y * tilt) * pitch - u_holoPlayCalibration.w;
    color.r = texture2D(u_scene, quilt_map(u_holoPlayTile, st, a-u_holoPlayRB.x*subp2)).r;
    color.g = texture2D(u_scene, quilt_map(u_holoPlayTile, st, a-subp2)).g;
    color.b = texture2D(u_scene, quilt_map(u_holoPlayTile, st, a-u_holoPlayRB.y*subp2)).b;

    #if defined(HOLOPLAY_DEBUG_CENTER)
    // Mark center line only in central view
    color.r = color.r * 0.001 + (st.x>0.49 && st.x<0.51 && fract(a)>0.48&&fract(a)<0.51 ?1.0:0.0);
    color.g = color.g * 0.001 + st.x;
    color.b = color.b * 0.001 + st.y;

    #elif defined(HOLOPLAY_DEBUG)
    // use quilt texture
    color = texture2D(u_scene, st).rgb;
    #endif

    gl_FragColor = vec4(color,1.0);
}
)";

const std::string holoplay_frag_300 = R"(
#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D   u_scene;
uniform vec2        u_resolution;

uniform vec3        u_holoPlayTile;
uniform vec4        u_holoPlayCalibration;  // dpi, pitch, slope, center
uniform vec2        u_holoPlayRB;           // ri, bi

out     vec4        fragColor;

// GET CORRECT VIEW
vec2 quilt_map(vec3 tile, vec2 pos, float a) {
    vec2 tile2 = vec2(tile.x - 1.0, tile.y - 1.0);
    vec2 dir = vec2(-1.0, -1.0);

    a = fract(a) * tile.y;
    tile2.y += dir.y * floor(a);
    a = fract(a) * tile.x;
    tile2.x += dir.x * floor(a);
    return (tile2 + pos) / tile.xy;
}

void main (void) {
    vec3 color = vec3(0.0);
    vec2 st = gl_FragCoord.xy/u_resolution.xy;

    float pitch = -u_resolution.x / u_holoPlayCalibration.x  * u_holoPlayCalibration.y * sin(atan(abs(u_holoPlayCalibration.z)));
    float tilt = u_resolution.y / (u_resolution.x * u_holoPlayCalibration.z);
    float subp = 1.0 / (3.0 * u_resolution.x);
    float subp2 = subp * pitch;

    float a = (-st.x - st.y * tilt) * pitch - u_holoPlayCalibration.w;
    color.r = texture(u_scene, quilt_map(u_holoPlayTile, st, a-u_holoPlayRB.x*subp2)).r;
    color.g = texture(u_scene, quilt_map(u_holoPlayTile, st, a-subp2)).g;
    color.b = texture(u_scene, quilt_map(u_holoPlayTile, st, a-u_holoPlayRB.y*subp2)).b;

    #if defined(HOLOPLAY_DEBUG_CENTER)
    // Mark center line only in central view
    color.r = color.r * 0.001 + (st.x>0.49 && st.x<0.51 && fract(a)>0.48&&fract(a)<0.51 ?1.0:0.0);
    color.g = color.g * 0.001 + st.x;
    color.b = color.b * 0.001 + st.y;

    #elif defined(HOLOPLAY_DEBUG)
    // use quilt texture
    color = texture(u_scene, st).rgb;
    #endif

    fragColor = vec4(color,1.0);
}
)";

std::string getHoloplayFragShader(size_t _versionNumber) {
    std::string rta = "";

    if (_versionNumber < 130)
        rta += holoplay_frag;
    else if (_versionNumber >= 130) 
        rta += holoplay_frag_300;

    return rta;
}

void holoplayFeedUniforms(Shader& _shader) {
    _shader.setUniform("u_holoPlayTile", float(holoplay.columns), float(holoplay.rows), float(holoplay.totalViews));
    _shader.setUniform("u_holoPlayCalibration", holoplay.dpi, holoplay.pitch, holoplay.slope, holoplay.center);
    _shader.setUniform("u_holoPlayRB", float(holoplay.ri), float(holoplay.bi));
}

}