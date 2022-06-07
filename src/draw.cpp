#include "ada/draw.h"
#include "ada/fs.h"
#include "ada/font.h"
#include "ada/string.h"
#include "ada/window.h"

#include "ada/shaders/defaultShaders.h"

#include "glm/gtc/matrix_transform.hpp"

#include <stack>

namespace ada {

Shader*     shaderPtr     = nullptr;
bool        shaderChange  = true; 

glm::vec4   fill_color      = glm::vec4(1.0f);
Shader*     fill_shader     = nullptr;
bool        fill_enabled    = true;

float       points_size     = 10.0f;
int         points_shape    = 0.0;
Shader*     points_shader   = nullptr;

glm::vec4   stroke_color    = glm::vec4(1.0f);
bool        stroke_enabled  = true;

Font*       font;
std::map<std::string, Font*>    fonts;

glm::mat4   matrix_world    = glm::mat4(1.0f);
std::stack<glm::mat4>   matrix_stack;

Camera*     cameraPtr       = nullptr;
Camera*     cameraCustomPtr = nullptr;
LightPtrs   lightsList;
bool        lights_enabled  = false;

std::vector<Label*>     labelsList;

void print(const std::string& _text) { std::cout << _text << std::endl; }
void frameRate(int _fps) { setFps(_fps); }

bool fullscreen() { return isFullscreen(); }
void fullscreen(bool _fullscreen) { setFullscreen(_fullscreen); }

void resetMatrix() { matrix_world = glm::mat4(1.0f); }
void applyMatrix(const glm::mat3& _mat ) { matrix_world = _mat; }
void applyMatrix(const glm::mat4& _mat ) { matrix_world = glm::mat4(_mat); };

void rotate(float _rad) { matrix_world = glm::rotate(matrix_world, _rad, glm::vec3(0.0f, 0.0f, 1.0f) ); }
void rotateX(float _rad) { matrix_world = glm::rotate(matrix_world, _rad, glm::vec3(1.0f, 0.0f, 0.0f) ); }
void rotateY(float _rad) { matrix_world = glm::rotate(matrix_world, _rad, glm::vec3(0.0f, 1.0f, 0.0f) ); }
void rotateZ(float _rad) { matrix_world = glm::rotate(matrix_world, _rad, glm::vec3(0.0f, 0.0f, 1.0f) ); }

void scale(float _s) { matrix_world = glm::scale(matrix_world, glm::vec3(_s) ); }
void scale(float _x, float _y, float _z) { matrix_world = glm::scale(matrix_world, glm::vec3(_x, _y, _z) ); }
void scale(const glm::vec2& _s) { matrix_world = glm::scale(matrix_world, glm::vec3(_s, 1.0f) ) ; }
void scale(const glm::vec3& _s) { matrix_world = glm::scale(matrix_world, _s) ; }

void translate(float _x, float _y, float _z) { matrix_world = glm::translate(matrix_world, glm::vec3(_x, _y, _z) ); }
void translate(const glm::vec2& _t) { matrix_world = glm::translate(matrix_world, glm::vec3(_t, 0.0f) ); }
void translate(const glm::vec3& _t) { matrix_world = glm::translate(matrix_world, _t ); }

void push() { matrix_stack.push(matrix_world); }
void pop() { 
    matrix_world = matrix_stack.top(); 
    matrix_stack.pop();
}

void setCamera(Camera& _camera) { setCamera(&_camera); }
void setCamera(Camera* _camera) { 
    cameraPtr = _camera; 
    glEnable(GL_DEPTH_TEST);
};

void resetCamera() {  
    cameraPtr = nullptr;
    glDisable(GL_DEPTH_TEST);
};

Camera* getCamera() {
    if (cameraPtr)
        return cameraPtr;
    return nullptr;
}

Camera *getCameraCustom() {
    if (cameraCustomPtr == nullptr)
        cameraCustomPtr = new Camera();
    return cameraCustomPtr;
}

void perspective(float _fovy, float _aspect, float _near, float _far) {
    cameraPtr = getCameraCustom();
    cameraPtr->setProjection( PERSPECTIVE );
    // cameraPtr->setProjection( glm::perspective(_fovy, _aspect, _near, _far) );
    cameraPtr->setAspect(_aspect);
    cameraPtr->setFOV(_fovy);
    cameraPtr->setClipping(_near, _far);
    glEnable(GL_DEPTH_TEST);
}

void ortho(float _left, float _right, float _bottom, float _top,  float _near, float _far) {
    cameraPtr = getCameraCustom();
    cameraPtr->setProjection( glm::ortho(  _left , _right, _bottom, _top, _near, _top) );
    glEnable(GL_DEPTH_TEST);
}

Camera* createCamera() {
    cameraPtr = getCameraCustom();
    cameraPtr->setViewport(getWindowWidth(), getWindowHeight());
    glEnable(GL_DEPTH_TEST);
    return cameraPtr;
}

glm::mat4 getProjectionViewWorldMatrix() {
    if (cameraPtr)
        return cameraPtr->getProjectionViewMatrix() * matrix_world; 
    else
        return getFlippedOrthoMatrix() * matrix_world;
}

const glm::mat4& getProjectionViewMatrix() {
    if (cameraPtr)
        return cameraPtr->getProjectionViewMatrix(); 
    else
        return getFlippedOrthoMatrix();
}

const glm::mat4& getProjectionMatrix() {
    if (cameraPtr)
        return cameraPtr->getProjectionMatrix(); 
    else
        return getFlippedOrthoMatrix();
}

const glm::mat4& getViewMatrix() {
    if (cameraPtr)
        return cameraPtr->getViewMatrix(); 
    else
        return getFlippedOrthoMatrix();
}

const glm::mat4& getWorldMatrix() { return matrix_world; }
glm::mat4* getWorldMatrixPtr() { return &matrix_world; }


Shader* getPointShader() {
    if (points_shader == nullptr) {
        points_shader = new Shader();
        points_shader->load( getDefaultSrc(FRAG_POINTS), getDefaultSrc(VERT_POINTS) );
    }

    return points_shader;
}

Shader* getFillShader() {
    if (fill_shader == nullptr) {
        fill_shader = new Shader();
        fill_shader->load( getDefaultSrc(FRAG_FILL), getDefaultSrc(VERT_FILL) );
    }
    
    return fill_shader;
}

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

    getFillShader();
    if (shaderPtr == nullptr || shaderPtr != fill_shader)
        shaderPtr = fill_shader;
}

void noStroke() { stroke_enabled = false;}
void stroke( float _brightness ) { stroke( glm::vec4( _brightness, _brightness, _brightness, stroke_color.a ) ); }
void stroke( float _red, float _green, float _blue) { stroke( glm::vec4( _red, _green, _blue, stroke_color.a ) );  }
void stroke( float _red, float _green, float _blue, float _alpha) { stroke( glm::vec4( _red, _green, _blue, _alpha ) );  }
void stroke( const glm::vec3& _color ) { stroke( glm::vec4( _color, stroke_color.a ) ); }
void stroke( const glm::vec4& _color ) { 
    stroke_color = _color;
    stroke_enabled = true;

    getFillShader();
    if (shaderPtr == nullptr || shaderPtr != fill_shader)
        shaderPtr = fill_shader;
}
void strokeWeight( float _weight) { glLineWidth(_weight); }

void pointSize( float _size ) { points_size = _size; }
void pointShape( PointShape _shape) { points_shape = _shape; }


// POINTS
//
void points(const std::vector<glm::vec2>& _positions, Shader* _program) {
    if (_program == nullptr)
        _program = getPointShader();
    
    shader(_program);

#if defined(__EMSCRIPTEN__)
    Vbo vbo = _positions;
    vbo.setDrawMode(GL_POINTS);
    vbo.render(_program);
#else

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
#endif
}

void points(const std::vector<glm::vec3>& _positions, Shader* _program) {
    if (_program == nullptr) 
        _program = getPointShader();
    
    shader(_program);

#if defined(__EMSCRIPTEN__)
    Vbo vbo = _positions;
    vbo.setDrawMode(GL_POINTS);
    vbo.render(_program);
#else
    #if !defined(PLATFORM_RPI) && !defined(DRIVER_GBM) && !defined(_WIN32)
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
#endif
}

void points(const Line& _line, Shader* _program) {
    std::vector<glm::vec3> positions;
    positions.push_back( _line.getPoint(0) );
    positions.push_back( _line.getPoint(1) );
    points(positions, _program);
}

void points(const Triangle& _triangle, Shader* _program) {
    std::vector<glm::vec3> positions;
    positions.push_back( _triangle.getVertex(0) );
    positions.push_back( _triangle.getVertex(1) );
    positions.push_back( _triangle.getVertex(2) );
    points(positions, _program);
}

void points(const BoundingBox& _bbox, Shader* _program) {
    std::vector<glm::vec3> positions;
    positions.push_back( _bbox.min );
    positions.push_back( _bbox.max );
    positions.push_back( glm::vec3(_bbox.min.x, _bbox.min.y, _bbox.max.z) );
    positions.push_back( glm::vec3(_bbox.min.x, _bbox.max.y, _bbox.max.z) );
    positions.push_back( glm::vec3(_bbox.max.x, _bbox.max.y, _bbox.min.z) );
    positions.push_back( glm::vec3(_bbox.max.x, _bbox.min.y, _bbox.min.z) );
    positions.push_back( glm::vec3(_bbox.min.x, _bbox.max.y, _bbox.min.z) );
    positions.push_back( glm::vec3(_bbox.max.x, _bbox.min.y, _bbox.max.z) );
    points(positions, _program);
}

void pointsBoundingBox(const glm::vec4& _bbox, Shader* _program) {
    std::vector<glm::vec2> positions;
    positions.push_back( glm::vec2(_bbox.x, _bbox.y) );
    positions.push_back( glm::vec2(_bbox.z, _bbox.y) );
    positions.push_back( glm::vec2(_bbox.z, _bbox.w) );
    positions.push_back( glm::vec2(_bbox.x, _bbox.w) );
    points(positions, _program);
}

// LINES
// 
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
    if (_program == nullptr)
        _program = getFillShader();
   
    shader(_program);
    _program->setUniform("u_color", stroke_color);

#if defined(__EMSCRIPTEN__)
    Vbo vbo = _positions;
    vbo.setDrawMode(GL_LINE_STRIP);
    vbo.render(_program);
#else
    const GLint location = _program->getAttribLocation("a_position");
    if (location != -1) {
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, 2, GL_FLOAT, false, 0,  _positions.data());
        glDrawArrays(GL_LINE_STRIP, 0, _positions.size());
        glDisableVertexAttribArray(location);
    }
#endif
};

void line(const glm::vec3& _a, const glm::vec3& _b, Shader* _program) {
    std::vector<glm::vec3> pts = {_a, _b};
    line(pts, _program);
}

void line(const std::vector<glm::vec3>& _positions, Shader* _program) {
    if (_program == nullptr)
        _program = getFillShader();

    shader(_program);
    _program->setUniform("u_color", stroke_color);

#if defined(__EMSCRIPTEN__)
    Vbo vbo = _positions;
    vbo.setDrawMode(GL_LINE_STRIP);
    vbo.render(_program);
#else
    const GLint location = _program->getAttribLocation("a_position");
    if (location != -1) {
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 0, (const void*)_positions.data());
        glDrawArrays(GL_LINE_STRIP, 0, _positions.size());
        glDisableVertexAttribArray(location);
    }

#endif
};

void line(const Line& _line, Shader* _program) {
    std::vector<glm::vec3> positions;
    positions.push_back( _line.getPoint(0) );
    positions.push_back( _line.getPoint(1) );
    line(positions, _program);
}

void line(const Triangle& _triangle, Shader* _program) {
    std::vector<glm::vec3> positions;
    positions.push_back( _triangle.getVertex(0) );
    positions.push_back( _triangle.getVertex(1) );
    positions.push_back( _triangle.getVertex(2) );
    positions.push_back( _triangle.getVertex(0) );
    line(positions, _program);
}

void line(const BoundingBox& _bbox, Shader* _program) {
    std::vector<glm::vec3> positions;
    positions.push_back( _bbox.min );
    positions.push_back( glm::vec3( _bbox.min.x, _bbox.min.y, _bbox.max.z ) );

    // TODO:
    //  - finsih this

    line(positions, _program);
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

// TEXT
//
Font* getDefaultFont() {
    if (fonts.find("default") != fonts.end() ) {
        return fonts["default"];
    }
    else {
        Font* defaultFont = new Font();
        defaultFont->setAlign( ALIGN_CENTER );
        defaultFont->setAlign( ALIGN_BOTTOM );
        defaultFont->setSize(24.0f);
        defaultFont->setColor(glm::vec4(1.0));
        fonts["default"] = defaultFont;
        return defaultFont;
    }
}

Font* getFont() {
    if (font == nullptr)
        return getDefaultFont();
    return font;
}

Font* loadFont(const std::string& _file, const std::string& _name) {
    if (fonts.find(_name) != fonts.end() ) 
        return fonts[_name];
    else {
        Font* newFont = new Font();
        newFont->load(_file);
        fonts[_name] = newFont;
        return newFont;
    }
}

Font* textFont(const std::string& _name) { 
    if (_name == "default")
        font = getDefaultFont();
    if (fonts.find(_name) != fonts.end() ) 
        font = fonts[_name];
    else {
        font = getDefaultFont();
        fonts[_name] = font;
    }
    
    return font; 
}

void textAlign(FontHorizontalAlign _align, Font* _font) {
    if (_font == nullptr)
        _font = getFont();

    _font->setAlign( _align );
}

void textAlign(FontVerticalAlign _align, Font* _font) {
    if (_font == nullptr)
        _font = getFont();

    _font->setAlign( _align );
}

void textAngle(float _angle, Font* _font) {
    if (_font == nullptr)
        _font = getFont();

    _font->setAngle( _angle );
}

void textSize(float _size, Font* _font) { 
    if (_font == nullptr)
        _font = getFont();
    
    _font->setSize(_size);
}

void text(const std::string& _text, const glm::vec2& _pos, Font* _font) { text(_text, _pos.x, _pos.y, _font); }
void text(const std::string& _text, float _x, float _y, Font* _font) {
    if (_font == nullptr)
        _font = getFont();
    
    _font->setColor( fill_color );
    _font->render(_text, _x, _y);
}

// SHAPES
// 
void triangles(const std::vector<glm::vec2>& _positions, Shader* _program) {
    if (_program == nullptr)
        _program = getFillShader();;

    shader(_program);

#if defined(__EMSCRIPTEN__)
    Vbo vbo = _positions;
    vbo.setDrawMode(GL_TRIANGLES);
    vbo.render(_program);
#else
    const GLint location = _program->getAttribLocation("a_position");
    if (location != -1) {
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, 2, GL_FLOAT, false, 0,  &_positions[0].x);
        glDrawArrays(GL_TRIANGLES, 0, _positions.size());
        glDisableVertexAttribArray(location);
    }
#endif
}

void triangles(const std::vector<glm::vec3>& _positions, Shader* _program) {
    if (_program == nullptr)
        _program = getFillShader();;

    shader(_program);

#if defined(__EMSCRIPTEN__)
    Vbo vbo = _positions;
    vbo.setDrawMode(GL_TRIANGLES);
    vbo.render(_program);
#else
    const GLint location = _program->getAttribLocation("a_position");
    if (location != -1) {
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, 3, GL_FLOAT, false, 0,  &_positions[0].x);
        glDrawArrays(GL_TRIANGLES, 0, _positions.size());
        glDisableVertexAttribArray(location);
    }
#endif
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

Shader loadShader(const std::string& _fragFile, const std::string& _vertFile) {
    Shader s;
    s.load(loadGlslFrom(_fragFile), loadGlslFrom(_vertFile));
    return s;
}

Shader createShader(const std::string& _fragSrc, const std::string& _vertSrc) {
    Shader s;
    if (!_fragSrc.empty() && _vertSrc.empty())
        s.load(_fragSrc, getDefaultSrc(VERT_DEFAULT_SCENE), false, false);
    else if (_fragSrc.empty())
        s.load(getDefaultSrc(FRAG_DEFAULT_SCENE), getDefaultSrc(VERT_DEFAULT_SCENE), false, false);
    else
        s.load(_fragSrc, _vertSrc, false, false);
    return s;
}

Shader createShader(DefaultShaders _frag, DefaultShaders _vert) {
    Shader s;
    s.load(getDefaultSrc(_frag), getDefaultSrc(_vert));
    return s;
}

Shader* getShader() { return shaderPtr; }
void resetShader() { shaderPtr = nullptr; }
void shader(Shader& _program) { shader(&_program); }
void shader(Shader* _program) {
    if (shaderPtr != fill_shader || shaderPtr != points_shader) {
        shaderPtr = _program; 
        shaderChange = true;
    }

    _program->textureIndex = 0;
    _program->use();

    _program->setUniform("u_date", getDate() );
    _program->setUniform("u_resolution", (float)getWindowWidth(), (float)getWindowHeight() );
    _program->setUniform("u_mouse", (float)getMouseX(), (float)getMouseY() );
    _program->setUniform("u_time", (float)getTimeSec() );
    _program->setUniform("u_delta", (float)getDelta() );

    if (_program == fill_shader)
        _program->setUniform("u_color", fill_color);
    else if (_program == points_shader) {
        _program->setUniform("u_size", points_size);
        _program->setUniform("u_shape", points_shape);
        _program->setUniform("u_color", fill_color);
        #if !defined(PLATFORM_RPI) && !defined(DRIVER_GBM) && !defined(_WIN32) && !defined(__EMSCRIPTEN__)
        glEnable(GL_POINT_SPRITE);
        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
        #endif
    }

    if (cameraPtr) {
        _program->setUniform("u_modelViewProjectionMatrix", cameraPtr->getProjectionViewMatrix() * matrix_world );
        _program->setUniform("u_projectionMatrix", cameraPtr->getProjectionMatrix());
        _program->setUniform("u_normalMatrix", cameraPtr->getNormalMatrix());
        _program->setUniform("u_viewMatrix", cameraPtr->getViewMatrix() );

        _program->setUniform("u_camera", -cameraPtr->getPosition() );
        _program->setUniform("u_cameraDistance", cameraPtr->getDistance());
        _program->setUniform("u_cameraNearClip", cameraPtr->getNearClip());
        _program->setUniform("u_cameraFarClip", cameraPtr->getFarClip());
        _program->setUniform("u_cameraEv100", cameraPtr->getEv100());
        _program->setUniform("u_cameraExposure", cameraPtr->getExposure());
        _program->setUniform("u_cameraAperture", cameraPtr->getAperture());
        _program->setUniform("u_cameraShutterSpeed", cameraPtr->getShutterSpeed());
        _program->setUniform("u_cameraSensitivity", cameraPtr->getSensitivity());
        _program->setUniform("u_cameraChange", cameraPtr->bChange);
        _program->setUniform("u_iblLuminance", 30000.0f * cameraPtr->getExposure());
    }
    else
        _program->setUniform("u_modelViewProjectionMatrix", getFlippedOrthoMatrix() * matrix_world );

    if (lights_enabled) {
        // Pass Light Uniforms
        if (lightsList.size() == 1) {
            LightPtrs::iterator it = lightsList.begin();

            _program->setUniform("u_lightColor", it->second->color);
            _program->setUniform("u_lightIntensity", it->second->intensity);

            if (it->second->getType() != ada::LIGHT_DIRECTIONAL)
                _program->setUniform("u_light", it->second->getPosition());
            if (it->second->getType() == ada::LIGHT_DIRECTIONAL || it->second->getType() == ada::LIGHT_SPOT)
                _program->setUniform("u_lightDirection", it->second->direction);
            if (it->second->falloff > 0)
                _program->setUniform("u_lightFalloff", it->second->falloff);

            // _program->setUniform("u_lightMatrix", it->second->getBiasMVPMatrix() );
            // _program->setUniformDepthTexture("u_lightSListhadowMap", it->second->getShadowMap(), _program->textureIndex++ );
        }
        else {
            for (LightPtrs::iterator it = lightsList.begin(); it != lightsList.end(); ++it) {
                std::string name = "u_" + it->first;

                _program->setUniform(name + "Color", it->second->color);
                _program->setUniform(name + "Intensity", it->second->intensity);
                if (it->second->getType() != ada::LIGHT_DIRECTIONAL)
                    _program->setUniform(name, it->second->getPosition());
                if (it->second->getType() == ada::LIGHT_DIRECTIONAL || it->second->getType() == ada::LIGHT_SPOT)
                    _program->setUniform(name + "Direction", it->second->direction);
                if (it->second->falloff > 0)
                    _program->setUniform(name +"Falloff", it->second->falloff);

                _program->setUniform(name + "Matrix", it->second->getBiasMVPMatrix() );
                // _program->setUniformDepthTexture(name + "ShadowMap", it->second->getShadowMap(), _shader->textureIndex++ );
            }
        } 
    }
}

void lights() { lights_enabled = true; }
void noLights() { lights_enabled = false; }
Light* createLight(const std::string& _name) {
    Light* light = new Light();
    addLight(light, _name);
    return light;
}
void addLight(Light& _light, const std::string& _name) { lightsList[_name] = &_light; }
void addLight(Light* _light, const std::string& _name) { lightsList[_name] = _light; }


void model(Vbo& _vbo, Shader* _program) { model(&_vbo, _program); }
void model(Vbo* _vbo, Shader* _program) {
    if (_program == nullptr) {
        if (shaderPtr != nullptr)
            _program = shaderPtr;
        else
            _program = getFillShader();
    }

    if (shaderChange && 
        shaderPtr != fill_shader && 
        shaderPtr != points_shader) {
        VertexLayout* vl = _vbo->getVertexLayout();
        if (vl->haveAttrib("color"))
            _program->addDefine("MODEL_VERTEX_COLOR", "v_color");
        // else
            // _program->delDefine("MODEL_VERTEX_COLOR");

        if (vl->haveAttrib("normal"))
            _program->addDefine("MODEL_VERTEX_NORMAL", "v_normal");
        // else
            // _program->delDefine("MODEL_VERTEX_COLOR");

        if (vl->haveAttrib("texcoord"))
            _program->addDefine("MODEL_VERTEX_TEXCOORD", "v_texcoord");
        // else
            // _program->delDefine("MODEL_VERTEX_TEXCOORD");

        if (vl->haveAttrib("tangent"))
            _program->addDefine("MODEL_VERTEX_TANGENT", "v_tangent");
        // else
            // _program->delDefine("MODEL_VERTEX_TEXCOORD");

        shaderChange = false;
    }

    shader(_program);

    if (_vbo->getDrawMode() == GL_LINES || _vbo->getDrawMode() == GL_LINE_LOOP || _vbo->getDrawMode() == GL_LINE_STRIP)
        _program->setUniform("u_color", stroke_color);

    _vbo->render(_program);
}

void texture(Texture& _texture, const std::string _name) { texture(&_texture, _name); }
void texture(Texture* _texture, const std::string _name) {
    if (shaderPtr == nullptr)
        shaderPtr = getFillShader();
    
    std::string name = _name;
    if (_name.size() == 0)
        name = "u_tex" + toString(shaderPtr->textureIndex);
    shaderPtr->addDefine("USE_TEXTURE", name);
    shaderPtr->setUniformTexture(name, _texture, shaderPtr->textureIndex );
    shaderPtr->setUniform(name + "Resolution", (float)_texture->getWidth(), (float)_texture->getHeight());
    shaderPtr->textureIndex++;
}

void addLabel(Label& _label) { addLabel(&_label); }
void addLabel(Label* _label) {
    if (font == nullptr)
        font = getDefaultFont();

    labelsList.push_back( _label );
}

void addLabel(const std::string& _text, glm::vec3* _position, LabelType _type) {
    addLabel( new ada::Label(_text, _position, _type) );
}

void addLabel(const std::string& _text, Node* _node, LabelType _type) {
    addLabel( new ada::Label(_text, _node, _type) );
}

void addLabel(const std::string& _text, Model* _model, LabelType _type) {
    addLabel( new ada::Label(_text, _model, _type) );
}

void addLabel(std::function<std::string(Label*)> _func, glm::vec3* _position, LabelType _type) {
    addLabel( new ada::Label(_func, _position, _type) );
}
void addLabel(std::function<std::string(Label*)> _func, Node* _node, LabelType _type) {
    addLabel( new ada::Label(_func, _node, _type) );
}
void addLabel(std::function<std::string(Label*)> _func, Model* _model, LabelType _type) {
    addLabel( new ada::Label(_func, _model, _type) );
}

void labels() {
    if (font == nullptr)
        font = getDefaultFont();

    for (size_t i = 0; i < labelsList.size(); i++)
        labelsList[i]->update( getCamera(), font );

    // font->setColor( glm::vec4(0.0f,0.0f,0.0f,1.0f) );
    // font->setBlurAmount( 10.0f );
    // font->setEffect( EFFECT_GROW );
    // for (size_t i = 0; i < labelsList.size(); i++)
    //     labelsList[i].render( font );
    
    font->setEffect( EFFECT_NONE );
    font->setColor( fill_color );
    for (size_t i = 0; i < labelsList.size(); i++)
        labelsList[i]->render( font );
}

};
