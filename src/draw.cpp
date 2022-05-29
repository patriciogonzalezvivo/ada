#include "ada/draw.h"
#include "ada/fs.h"
#include "ada/font.h"
#include "ada/string.h"
#include "ada/window.h"

#include "ada/shaders/defaultShaders.h"

#include "glm/gtc/matrix_transform.hpp"
#include <stack>
#include <memory>

namespace ada {

ShaderPtr   shaderPtr     = nullptr;

glm::vec4   fill_color      = glm::vec4(1.0f);
bool        fill_enabled    = true;

float       points_size     = 10.0f;
int         points_shape    = 0.0;
ShaderPtr   points_shader   = nullptr;

glm::vec4   stroke_color    = glm::vec4(1.0f);
bool        stroke_enabled  = true;

ada::Font*  font;

glm::mat4   matrix          = glm::mat4(1.0f);
std::stack<glm::mat4> matrix_stack;

CameraPtr   cameraPtr       = nullptr;
LightPtrs   lights;

void resetMatrix() { matrix = glm::mat4(1.0f); }

void applyMatrix(const glm::mat3& _mat ) { matrix = _mat; }
void applyMatrix(const glm::mat4& _mat ) { matrix = glm::mat4(_mat); };

void rotate(float _rad) { matrix = glm::rotate(matrix, _rad, glm::vec3(0.0f, 0.0f, 1.0f) ); }
void rotateX(float _rad) { matrix = glm::rotate(matrix, _rad, glm::vec3(1.0f, 0.0f, 0.0f) ); }
void rotateY(float _rad) { matrix = glm::rotate(matrix, _rad, glm::vec3(0.0f, 1.0f, 0.0f) ); }
void rotateZ(float _rad) { matrix = glm::rotate(matrix, _rad, glm::vec3(0.0f, 0.0f, 1.0f) ); }

void scale(float _s) { matrix = glm::scale(matrix, glm::vec3(_s) ); }
void scale(float _x, float _y, float _z) { matrix = glm::scale(matrix, glm::vec3(_x, _y, _z) ); }
void scale(const glm::vec2& _s) { matrix = glm::scale(matrix, glm::vec3(_s, 1.0f) ) ; }
void scale(const glm::vec3& _s) { matrix = glm::scale(matrix, _s) ; }

void translate(float _x, float _y, float _z) { matrix = glm::translate(matrix, glm::vec3(_x, _y, _z) ); }
void translate(const glm::vec2& _t) { matrix = glm::translate(matrix, glm::vec3(_t, 0.0f) ); }
void translate(const glm::vec3& _t) { matrix = glm::translate(matrix, _t ); }

void push() { matrix_stack.push(matrix); }
void pop() { 
    matrix = matrix_stack.top(); 
    matrix_stack.pop();
}

void setCamera(Camera &_camera) { 
    cameraPtr = CameraPtr(&_camera); 
    glEnable(GL_DEPTH_TEST);
};

CameraPtr getCamera() {
    if (cameraPtr)
        return cameraPtr;
    return nullptr;
}

glm::mat4 getMatrix() { 
    if (cameraPtr)
        return cameraPtr->getProjectionViewMatrix() * matrix; 
    else
        return getOrthoMatrix() * matrix;
}

void addLight(Light& _light, const std::string& _name) { lights[_name] = LightPtr(&_light);  }
void addLight(Light* _light, const std::string& _name) { lights[_name] = LightPtr(_light);  }

Shader loadShader(std::string& _fragFile, std::string& _vertFile) {
    Shader s;
    s.load(loadGlslFrom(_fragFile), loadGlslFrom(_vertFile));
    return s;
}

Shader createShader(std::string& _fragSrc, std::string& _vertSrc) {
    Shader s;
    s.load(_fragSrc, _vertSrc);
    return s;
}

Shader createShader(DefaultShaders _frag, DefaultShaders _vert) {
    Shader s;
    s.load(getDefaultSrc(_frag), getDefaultSrc(_vert));
    return s;
}

void shader(Shader& _shader) { shader(&_shader); }
void shader(Shader* _shader) {

    if (shaderPtr == nullptr)
        shaderPtr = ShaderPtr(_shader); 
    else if (shaderPtr.get() != _shader)
        shaderPtr = ShaderPtr(_shader);

    _shader->use();

    if (cameraPtr) {
        _shader->setUniform("u_modelViewProjectionMatrix", cameraPtr->getProjectionViewMatrix() );
        _shader->setUniform("u_projectionMatrix", cameraPtr->getProjectionMatrix());
        _shader->setUniform("u_normalMatrix", cameraPtr->getNormalMatrix());
        _shader->setUniform("u_viewMatrix", cameraPtr->getViewMatrix() );

        _shader->setUniform("u_camera", -cameraPtr->getPosition() );
        _shader->setUniform("u_cameraDistance", cameraPtr->getDistance());
        _shader->setUniform("u_cameraNearClip", cameraPtr->getNearClip());
        _shader->setUniform("u_cameraFarClip", cameraPtr->getFarClip());
        _shader->setUniform("u_cameraEv100", cameraPtr->getEv100());
        _shader->setUniform("u_cameraExposure", cameraPtr->getExposure());
        _shader->setUniform("u_cameraAperture", cameraPtr->getAperture());
        _shader->setUniform("u_cameraShutterSpeed", cameraPtr->getShutterSpeed());
        _shader->setUniform("u_cameraSensitivity", cameraPtr->getSensitivity());
        _shader->setUniform("u_cameraChange", cameraPtr->bChange);
        _shader->setUniform("u_iblLuminance", 30000.0f * cameraPtr->getExposure());
    }

    // Pass Light Uniforms
    if (lights.size() == 1) {
        LightPtrs::iterator it = lights.begin();

        _shader->setUniform("u_lightColor", it->second->color);
        _shader->setUniform("u_lightIntensity", it->second->intensity);

        if (it->second->getType() != ada::LIGHT_DIRECTIONAL)
            _shader->setUniform("u_light", it->second->getPosition());
        if (it->second->getType() == ada::LIGHT_DIRECTIONAL || it->second->getType() == ada::LIGHT_SPOT)
            _shader->setUniform("u_lightDirection", it->second->direction);
        if (it->second->falloff > 0)
            _shader->setUniform("u_lightFalloff", it->second->falloff);

        // _shader->setUniform("u_lightMatrix", it->second->getBiasMVPMatrix() );
        // _shader->setUniformDepthTexture("u_lightShadowMap", it->second->getShadowMap(), _shader->textureIndex++ );
    }
    else {
        for (LightPtrs::iterator it = lights.begin(); it != lights.end(); ++it) {
            std::string name = "u_" + it->first;

            _shader->setUniform(name + "Color", it->second->color);
            _shader->setUniform(name + "Intensity", it->second->intensity);
            if (it->second->getType() != ada::LIGHT_DIRECTIONAL)
                _shader->setUniform(name, it->second->getPosition());
            if (it->second->getType() == ada::LIGHT_DIRECTIONAL || it->second->getType() == ada::LIGHT_SPOT)
                _shader->setUniform(name + "Direction", it->second->direction);
            if (it->second->falloff > 0)
                _shader->setUniform(name +"Falloff", it->second->falloff);

            _shader->setUniform(name + "Matrix", it->second->getBiasMVPMatrix() );
            // _shader->setUniformDepthTexture(name + "ShadowMap", it->second->getShadowMap(), _shader->textureIndex++ );
        }
    } 
}

void resetShader() { shaderPtr = nullptr; }

void clear() { clear( glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) ); }
void clear( float _brightness ) { clear( glm::vec4(_brightness, _brightness, _brightness, 1.0f) ); }
void clear( const glm::vec3& _color ) { clear( glm::vec4(_color, 1.0f) ); }
void clear( const glm::vec4& _color ) {
    glClearColor(_color.r, _color.g, _color.b, _color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void noFill() { fill_enabled = false; }
void fill( float _brightness ) { fill( glm::vec4( _brightness, _brightness, _brightness, fill_color.a ) ); }
void fill( float _red, float _green, float _blue) { fill( glm::vec4( _red, _green, _blue, fill_color.a ) );  }
void fill( float _red, float _green, float _blue, float _alpha) { fill( glm::vec4( _red, _green, _blue, _alpha ) );  }
void fill( const glm::vec3& _color ) { fill( glm::vec4( _color, fill_color.a ) ); }
void fill( const glm::vec4& _color ) { 
    fill_color = _color;
    fill_enabled = true;
}

void noStroke() { stroke_enabled = false;}
void stroke( float _brightness ) { stroke( glm::vec4( _brightness, _brightness, _brightness, stroke_color.a ) ); }
void stroke( float _red, float _green, float _blue) { stroke( glm::vec4( _red, _green, _blue, stroke_color.a ) );  }
void stroke( float _red, float _green, float _blue, float _alpha) { stroke( glm::vec4( _red, _green, _blue, _alpha ) );  }
void stroke( const glm::vec3& _color ) { stroke( glm::vec4( _color, stroke_color.a ) ); }
void stroke( const glm::vec4& _color ) { 
    stroke_color = _color;
    stroke_enabled = true;
}
void strokeWeight( float _weight) { glLineWidth(_weight); }

void pointSize( float _size ) { points_size = _size; }
void pointShape( PointShape _shape) { points_shape = _shape; }

void points(const std::vector<glm::vec2>& _positions, Shader* _program) {
    if (_program == nullptr) {
        if (points_shader == nullptr) {
            points_shader = ShaderPtr();
            points_shader->load( getDefaultSrc(FRAG_POINTS), getDefaultSrc(VERT_POINTS) );
        }

        points_shader->use();
        points_shader->setUniform("u_size", points_size);
        points_shader->setUniform("u_shape", points_shape);
        points_shader->setUniform("u_color", fill_color);
        points_shader->setUniform("u_modelViewProjectionMatrix", getMatrix() );
        _program = points_shader.get();
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
            points_shader = ShaderPtr();
            points_shader->load( getDefaultSrc(FRAG_POINTS), getDefaultSrc(VERT_POINTS) );
        }

        points_shader->use();
        points_shader->setUniform("u_size", points_size);
        points_shader->setUniform("u_shape", points_shape);
        points_shader->setUniform("u_color", fill_color);
        points_shader->setUniform("u_modelViewProjectionMatrix", getMatrix() );

        _program = points_shader.get();
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
        if (shaderPtr == nullptr) {
            shaderPtr = ShaderPtr();
            shaderPtr->load( getDefaultSrc(FRAG_FILL), getDefaultSrc(VERT_FILL) );
        }

        shaderPtr->use();
        shaderPtr->setUniform("u_color", stroke_color);
        shaderPtr->setUniform("u_modelViewProjectionMatrix", getMatrix() );

        _program = shaderPtr.get();
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
        if (shaderPtr == nullptr) {
            shaderPtr = ShaderPtr();
            shaderPtr->load( getDefaultSrc(FRAG_FILL), getDefaultSrc(VERT_FILL) );
        }

        shaderPtr->use();
        shaderPtr->setUniform("u_color", stroke_color);
        shaderPtr->setUniform("u_modelViewProjectionMatrix", getMatrix() );

        _program = shaderPtr.get();
    }

    const GLint location = _program->getAttribLocation("a_position");
    if (location != -1) {
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, 3, GL_FLOAT, false, 0,  &_positions[0].x);
        glDrawArrays(GL_LINE_STRIP, 0, _positions.size());
        glDisableVertexAttribArray(location);
    }
};

void points(Vbo* _vbo) {
    if (points_shader == nullptr) {
        points_shader = ShaderPtr();
        points_shader->load( getDefaultSrc(FRAG_POINTS), getDefaultSrc(VERT_POINTS) );
    }

    points_shader->use();
    points_shader->setUniform("u_size", points_size);
    points_shader->setUniform("u_shape", points_shape);
    points_shader->setUniform("u_color", fill_color);
    points_shader->setUniform("u_modelViewProjectionMatrix", getMatrix() );

#if !defined(PLATFORM_RPI) && !defined(DRIVER_GBM) && !defined(_WIN32) && !defined(__EMSCRIPTEN__)
    glEnable(GL_POINT_SPRITE);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
#endif

    _vbo->render(points_shader.get());
}

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

void textAlign(FontAlign _align, Font* _font) {
    if (_font == nullptr) {
        if (font == nullptr)
            font = new Font();

        _font = font;
    }

    font->setAlign( _align );
}
void textSize(float _size, Font* _font) { 
    if (_font == nullptr) {
        if (font == nullptr)
            font = new Font();

        _font = font;
    }
    font->setSize(_size);
}

void text(const std::string& _text, const glm::vec2& _pos, Font* _font) { text(_text, _pos.x, _pos.y, _font); }

void text(const std::string& _text, float _x, float _y, Font* _font) {
    if (_font == nullptr) {
        if (font == nullptr)
            font = new Font();

        _font = font;
    }
    font->setColor( fill_color );
    font->render(_text, _x, _y);
}

void triangles(const std::vector<glm::vec2>& _positions, Shader* _program) {
    if (_program == nullptr) {
        if (shaderPtr == nullptr) {
            shaderPtr = ShaderPtr();
            shaderPtr->load( getDefaultSrc(FRAG_FILL), getDefaultSrc(VERT_FILL) );
        }

        shaderPtr->use();
        shaderPtr->setUniform("u_color", stroke_color);
        shaderPtr->setUniform("u_modelViewProjectionMatrix", getMatrix() );

        _program = shaderPtr.get();
    }

    const GLint location = _program->getAttribLocation("a_position");
    if (location != -1) {
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, 2, GL_FLOAT, false, 0,  &_positions[0].x);
        glDrawArrays(GL_TRIANGLES, 0, _positions.size());
        glDisableVertexAttribArray(location);
    }
}

void rect(const glm::vec2& _pos, const glm::vec2& _size, Shader* _program) {
    rect(_pos.x, _pos.y, _size.x, _size.y, _program);
}

void rect(float _x, float _y, float _w, float _h, Shader* _program) {

    // _x = _x * 2.0f - 1.0f;
    // _y = _y * 2.0f - 1.0f;
    // _w = _w * 2.0f;
    // _h = _h * 2.0f;
    
    _x -= _w * 0.5f;
    _y -= _h * 0.5f;
    std::vector<glm::vec2> coorners = { glm::vec2(_x, _y),     glm::vec2(_x + _w, _y), 
                                        glm::vec2(_x + _w, _y + _h), glm::vec2(_x, _y + _h),
                                        glm::vec2(_x, _y) };

    std::vector<glm::vec2> tris = {     coorners[0], coorners[1], coorners[2],
                                        coorners[2], coorners[3], coorners[0] };

    if (_program == nullptr) {
        if (fill_enabled) triangles(tris);
        if (stroke_enabled) line(coorners);
    }
    else
        triangles(tris, _program);
}

void model(Vbo& _vbo, Shader* _program) { model(&_vbo, _program); }
void model(Vbo* _vbo, Shader* _program) {
    if (_program == nullptr) {
        if (shaderPtr == nullptr) {
            shaderPtr = ShaderPtr();
            shaderPtr->load( getDefaultSrc(FRAG_FILL), getDefaultSrc(VERT_FILL) );
        }
        _program = shaderPtr.get();
    }

    shader(_program);
    _program->setUniform("u_color", stroke_color);

    _vbo->render(_program);
}


};