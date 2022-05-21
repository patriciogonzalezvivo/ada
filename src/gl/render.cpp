#include "ada/gl/render.h"

namespace ada {

void render(const std::vector<glm::vec3>& _polyline, Shader* _program) {
    const GLint location = _program->getAttribLocation("a_position");
    if (location != -1) {
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, 3, GL_FLOAT, false, 0,  &_polyline[0].x);
        glDrawArrays(GL_LINE_STRIP, 0, _polyline.size());
        glDisableVertexAttribArray(location);
    }
};

};