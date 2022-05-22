#include "ada/gl/draw.h"
#include "ada/window.h"
#include "ada/tools/text.h"
#include "ada/tools/font.h"
#include "ada/shaders/defaultShaders.h"

namespace ada {

bool        fill_enabled    = false;
Shader*     fill_shader     = nullptr;
glm::vec4   fill_color      = glm::vec4(1.0f);

Shader*     points_shader   = nullptr;
int         points_shape    = 0.0;
float       points_size     = 10.0f;

bool        stroke_enabled  = true;
Shader*     stroke_shader   = nullptr;
float       stroke_weight   = 1.0f;
glm::vec4   stroke_color    = glm::vec4(1.0f);

ada::Font   font;

void clear() { clear( glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) ); }
void clear( float _brightness ) { clear( glm::vec4(_brightness, _brightness, _brightness, 1.0f) ); }
void clear( const glm::vec3& _color ) { clear( glm::vec4(_color, 1.0f) ); }
void clear( const glm::vec4& _color ) {
    glClearColor(_color.r, _color.g, _color.b, _color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void blendMode( BlendMode _mode ) {
    switch (_mode) {
        case ENABLE_ALPHA:
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;

        case ENABLE_ADD:
            glEnable(GL_BLEND);
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            break;

        case ENABLE_MULTIPLY:
            glEnable(GL_BLEND);
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA /* GL_ZERO or GL_ONE_MINUS_SRC_ALPHA */);
            break;

        case ENABLE_SCREEN:
            glEnable(GL_BLEND);
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
            break;

        case ENABLE_SUBSTRACT:
            glEnable(GL_BLEND);
            glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            break;

        case DISABLE_BLEND:
            glDisable(GL_BLEND);
            break;

        default:
            break;
    }
}

void fill( float _brightness ) { fill_color = glm::vec4( _brightness, _brightness, _brightness, fill_color.a ); }
void fill( float _red, float _green, float _blue) { fill_color = glm::vec4( _red, _green, _blue, fill_color.a );  }
void fill( float _red, float _green, float _blue, float _alpha) { fill_color = glm::vec4( _red, _green, _blue, _alpha );  }
void fill( const glm::vec3& _color ) { fill_color = glm::vec4( _color, fill_color.a ); }
void fill( const glm::vec4& _color ) { fill_color = _color; }

void stroke( float _brightness ) { stroke_color = glm::vec4( _brightness, _brightness, _brightness, stroke_color.a ); }
void stroke( float _red, float _green, float _blue) { stroke_color = glm::vec4( _red, _green, _blue, stroke_color.a );  }
void stroke( float _red, float _green, float _blue, float _alpha) { stroke_color = glm::vec4( _red, _green, _blue, _alpha );  }
void stroke( const glm::vec3& _color ) { stroke_color = glm::vec4( _color, stroke_color.a ); }
void stroke( const glm::vec4& _color ) { stroke_color = _color; }

void pointSize( float _size ) { points_size = _size; }
void pointShape( PointShape _shape) { points_shape = _shape; }

void points(const std::vector<glm::vec2>& _positions, Shader* _program) {
    if (_program == nullptr) {
        if (points_shader == nullptr) {
            points_shader = new Shader();
            points_shader->load( getDefaultSrc(FRAG_POINTS), getDefaultSrc(VERT_POINTS) );
        }

        points_shader->use();
        points_shader->setUniform("u_size", points_size);
        points_shader->setUniform("u_shape", points_shape);
        points_shader->setUniform("u_resolution", (float)getWindowWidth(), (float)getWindowHeight());
        points_shader->setUniform("u_color", fill_color);
        points_shader->setUniform("u_modelViewProjectionMatrix", getOrthoMatrix() );
        _program = points_shader;
    }

#if !defined(PLATFORM_RPI) && !defined(DRIVER_GBM) && !defined(_WIN32) && !defined(__EMSCRIPTEN__)
    glEnable(GL_POINT_SPRITE);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
#endif

    const GLint location = _program->getAttribLocation("a_position");
    if (location != -1) {
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, 2, GL_FLOAT, false, 0,  &_positions[0].x);
        glDrawArrays(GL_POINTS, 0, _positions.size());
        glDisableVertexAttribArray(location);
    }
}

void points(const std::vector<glm::vec3>& _positions, Shader* _program) {
    if (_program == nullptr) {
        if (points_shader == nullptr) {
            points_shader = new Shader();
            points_shader->load( getDefaultSrc(FRAG_POINTS), getDefaultSrc(VERT_POINTS) );
        }

        points_shader->use();
        points_shader->setUniform("u_size", points_size);
        points_shader->setUniform("u_color", fill_color);
        points_shader->setUniform("u_modelViewProjectionMatrix", getOrthoMatrix() );
        _program = points_shader;
    }

#if !defined(PLATFORM_RPI) && !defined(DRIVER_GBM) && !defined(_WIN32) && !defined(__EMSCRIPTEN__)
    glEnable(GL_POINT_SPRITE);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
#endif

    const GLint location = _program->getAttribLocation("a_position");
    if (location != -1) {
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, 3, GL_FLOAT, false, 0,  &_positions[0].x);
        glDrawArrays(GL_POINTS, 0, _positions.size());
        glDisableVertexAttribArray(location);
    }
}

void line(float _x1, float _y1, float _x2, float _y2, Shader* _program) {
    std::vector<glm::vec2> pts = {glm::vec2(_x1,_y1), glm::vec2(_x2, _y2)};
    line(pts, _program);
}

void line(float _x1, float _y1, float _z1, float _x2, float _y2, float _z2, Shader* _program) {
    std::vector<glm::vec3> pts = {glm::vec3(_x1,_y1, _z1), glm::vec3(_x2, _y2, _z2) };
    line(pts, _program);
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

void pointsBoundingBox(const glm::vec4& _bbox, Shader* _program) {
    std::vector<glm::vec2> positions;
    positions.push_back( glm::vec2(_bbox.x, _bbox.y) );
    positions.push_back( glm::vec2(_bbox.z, _bbox.y) );
    positions.push_back( glm::vec2(_bbox.z, _bbox.w) );
    positions.push_back( glm::vec2(_bbox.x, _bbox.w) );
    
    points(positions, _program);
}

void lineBoundingBox(const glm::vec4& _bbox, Shader* _program) {
    std::vector<glm::vec2> positions;
    positions.push_back( glm::vec2(_bbox.x, _bbox.y) );
    positions.push_back( glm::vec2(_bbox.z, _bbox.y) );
    positions.push_back( glm::vec2(_bbox.z, _bbox.w) );
    positions.push_back( glm::vec2(_bbox.x, _bbox.w) );
    positions.push_back( glm::vec2(_bbox.x, _bbox.y) );
    
    line(positions, _program);
}

void textAlign(FontAlign _align) { font.setAlign( _align ); }
void textSize(float _size) { font.setSize(_size); }

void text(const std::string& _text, const glm::vec2& _pos) { text(_text, _pos.x, _pos.y); }

void text(const std::string& _text, float _x, float _y) {
    font.setColor( fill_color );
    font.render(_text, _x, _y);
}



};