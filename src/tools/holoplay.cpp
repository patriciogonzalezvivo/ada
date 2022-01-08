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

void holoplayFeedUniforms(Shader& _shader) {
    _shader.setUniform("u_holoPlayTile", float(holoplay.columns), float(holoplay.rows), float(holoplay.totalViews));
    _shader.setUniform("u_holoPlayCalibration", holoplay.dpi, holoplay.pitch, holoplay.slope, holoplay.center);
    _shader.setUniform("u_holoPlayRB", float(holoplay.ri), float(holoplay.bi));
}

}