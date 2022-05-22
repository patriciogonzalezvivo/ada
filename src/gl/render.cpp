#include "ada/gl/render.h"
#include "ada/window.h"
#include "ada/shaders/defaultShaders.h"

namespace ada {

bool        fill_enabled    = false;
Shader*     fill_shader     = nullptr;
glm::vec4   fill_color      = glm::vec4(1.0f);

bool        stroke_enabled  = true;
Shader*     stroke_shader   = nullptr;
float       stroke_weight   = 1.0f;
glm::vec4   stroke_color    = glm::vec4(1.0f);

void clear() { clear( glm::vec4(0.0f, 0.0f, 0.0f, 1.0) ); }
void clear( float _brightness ) { clear( glm::vec4(_brightness, _brightness, _brightness, 1.0) ); }
void clear( const glm::vec3& _color ) { clear( glm::vec4(_color, 1.0) ); }
void clear( const glm::vec4& _color ) {
    glClearColor(_color.r, _color.g, _color.b, _color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void stroke( float _brightness ) { stroke_color = glm::vec4( _brightness, _brightness, _brightness, stroke_color.a ); }
void stroke( const glm::vec3& _color ) { stroke_color = glm::vec4( _color, stroke_color.a ); }
void stroke( const glm::vec4& _color ) { stroke_color = _color; }

void renderBoundingBox(const glm::vec4& _bbox, Shader* _program) {
    std::vector<glm::vec2> positions;
    positions.push_back( glm::vec2(_bbox.x, _bbox.y) );
    positions.push_back( glm::vec2(_bbox.z, _bbox.y) );
    positions.push_back( glm::vec2(_bbox.z, _bbox.w) );
    positions.push_back( glm::vec2(_bbox.x, _bbox.w) );
    positions.push_back( glm::vec2(_bbox.x, _bbox.y) );
    
    line(positions, _program);
}

void line(const glm::vec2& _a, const glm::vec2& _b, Shader* _program) {
    std::vector<glm::vec2> pts = {_a, _b};
    line(pts, _program);
}

void line(const std::vector<glm::vec2>& _positions, Shader* _program) {

    if (_program == nullptr) {
        if (stroke_shader == nullptr) {
            stroke_shader = new Shader();
            stroke_shader->load( getDefaultSrc(FRAG_WIREFRAME_3D), getDefaultSrc(VERT_WIREFRAME_3D) );
        }

        stroke_shader->use();
        stroke_shader->setUniform("u_color", stroke_color);
        stroke_shader->setUniform("u_modelViewProjectionMatrix", getOrthoMatrix() );
        _program = stroke_shader;
    }

    const GLint location = _program->getAttribLocation("a_position");
    if (location != -1) {
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, 2, GL_FLOAT, false, 0,  &_positions[0].x);
        glDrawArrays(GL_LINE_STRIP, 0, _positions.size());
        glDisableVertexAttribArray(location);
    }
};

void line(const glm::vec3& _a, const glm::vec3& _b, Shader* _program) {
    std::vector<glm::vec3> pts = {_a, _b};
    line(pts, _program);
}

void line(const std::vector<glm::vec3>& _positions, Shader* _program) {

    if (_program == nullptr) {
        if (stroke_shader == nullptr) {
            stroke_shader = new Shader();
            stroke_shader->load( getDefaultSrc(FRAG_WIREFRAME_3D), getDefaultSrc(VERT_WIREFRAME_3D) );
        }

        stroke_shader->use();
        stroke_shader->setUniform("u_color", stroke_color);
        stroke_shader->setUniform("u_modelViewProjectionMatrix", getOrthoMatrix() );
        _program = stroke_shader;
    }

    const GLint location = _program->getAttribLocation("a_position");
    if (location != -1) {
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, 3, GL_FLOAT, false, 0,  &_positions[0].x);
        glDrawArrays(GL_LINE_STRIP, 0, _positions.size());
        glDisableVertexAttribArray(location);
    }
};

};