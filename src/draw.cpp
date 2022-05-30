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

Shader*     shaderPtr     = nullptr;
bool        shaderChange  = true; 

glm::vec4   fill_color      = glm::vec4(1.0f);
Shader*     fill_shader   = nullptr;
bool        fill_enabled    = true;

float       points_size     = 10.0f;
int         points_shape    = 0.0;
Shader*     points_shader   = nullptr;

glm::vec4   stroke_color    = glm::vec4(1.0f);
bool        stroke_enabled  = true;

ada::Font*  font;

glm::mat4   matrix_model          = glm::mat4(1.0f);
std::stack<glm::mat4> matrix_stack;

CameraPtr   cameraPtr       = nullptr;
LightPtrs   lights;

void resetMatrix() { matrix_model = glm::mat4(1.0f); }

void applyMatrix(const glm::mat3& _mat ) { matrix_model = _mat; }
void applymatrix_model(const glm::mat4& _mat ) { matrix_model = glm::mat4(_mat); };

void rotate(float _rad) { matrix_model = glm::rotate(matrix_model, _rad, glm::vec3(0.0f, 0.0f, 1.0f) ); }
void rotateX(float _rad) { matrix_model = glm::rotate(matrix_model, _rad, glm::vec3(1.0f, 0.0f, 0.0f) ); }
void rotateY(float _rad) { matrix_model = glm::rotate(matrix_model, _rad, glm::vec3(0.0f, 1.0f, 0.0f) ); }
void rotateZ(float _rad) { matrix_model = glm::rotate(matrix_model, _rad, glm::vec3(0.0f, 0.0f, 1.0f) ); }

void scale(float _s) { matrix_model = glm::scale(matrix_model, glm::vec3(_s) ); }
void scale(float _x, float _y, float _z) { matrix_model = glm::scale(matrix_model, glm::vec3(_x, _y, _z) ); }
void scale(const glm::vec2& _s) { matrix_model = glm::scale(matrix_model, glm::vec3(_s, 1.0f) ) ; }
void scale(const glm::vec3& _s) { matrix_model = glm::scale(matrix_model, _s) ; }

void translate(float _x, float _y, float _z) { matrix_model = glm::translate(matrix_model, glm::vec3(_x, _y, _z) ); }
void translate(const glm::vec2& _t) { matrix_model = glm::translate(matrix_model, glm::vec3(_t, 0.0f) ); }
void translate(const glm::vec3& _t) { matrix_model = glm::translate(matrix_model, _t ); }

void push() { matrix_stack.push(matrix_model); }
void pop() { 
    matrix_model = matrix_stack.top(); 
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

void perspective(float _fovy, float _aspect, float _near, float _far) {
    cameraPtr = std::make_shared<Camera>();
    // cameraPtr->setProjection( glm::perspective(_fovy, _aspect, _near, _far) );
    cameraPtr->setAspect(_aspect);
    cameraPtr->setFOV(_fovy);
    cameraPtr->setClipping(_near, _far);
    glEnable(GL_DEPTH_TEST);
}

void ortho(float _left, float _right, float _bottom, float _top,  float _near, float _far) {
    cameraPtr = std::make_shared<Camera>();
    cameraPtr->setProjection( glm::ortho(  _left , _right, _bottom, _top, _near, _top) );
    glEnable(GL_DEPTH_TEST);
}

CameraPtr createCamera() {
    cameraPtr = std::make_shared<Camera>();
    cameraPtr->setViewport(getWindowWidth(), getWindowHeight());
    glEnable(GL_DEPTH_TEST);
    return getCamera();
}

glm::mat4 getMatrixModelProjectionView() {
    if (cameraPtr)
        return cameraPtr->getProjectionViewMatrix() * matrix_model; 
    else
        return getOrthoMatrix() * matrix_model;
}

glm::mat4 getMatrixProjectionView() {
    if (cameraPtr)
        return cameraPtr->getProjectionViewMatrix(); 
    else
        return getOrthoMatrix();
}

glm::mat4 getMatrixProjection() {
    if (cameraPtr)
        return cameraPtr->getProjectionMatrix(); 
    else
        return getOrthoMatrix();
}

glm::mat4 getMatrixView() {
    if (cameraPtr)
        return cameraPtr->getViewMatrix(); 
    else
        return getOrthoMatrix();
}

glm::mat4 getMatrixModel() { 
    return matrix_model;
}


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
    if (_program == nullptr) {
        points_shader = getPointShader();
        points_shader->use();
        points_shader->setUniform("u_size", points_size);
        points_shader->setUniform("u_shape", points_shape);
        points_shader->setUniform("u_color", fill_color);
        points_shader->setUniform("u_modelViewProjectionMatrix", getMatrixModelProjectionView() );
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
        points_shader = getPointShader();
        points_shader->use();
        points_shader->setUniform("u_size", points_size);
        points_shader->setUniform("u_shape", points_shape);
        points_shader->setUniform("u_color", fill_color);
        points_shader->setUniform("u_modelViewProjectionMatrix", getMatrixModelProjectionView() );

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
    if (_program == nullptr) {
        fill_shader = getFillShader();
        fill_shader->use();
        fill_shader->setUniform("u_color", stroke_color);
        fill_shader->setUniform("u_modelViewProjectionMatrix", getMatrixModelProjectionView() );
        _program = fill_shader;
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
        fill_shader = getFillShader();
        fill_shader->use();
        fill_shader->setUniform("u_color", stroke_color);
        fill_shader->setUniform("u_modelViewProjectionMatrix", getMatrixModelProjectionView() );
        _program = fill_shader;
    }

    const GLint location = _program->getAttribLocation("a_position");
    if (location != -1) {
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, 3, GL_FLOAT, false, 0,  &_positions[0].x);
        glDrawArrays(GL_LINE_STRIP, 0, _positions.size());
        glDisableVertexAttribArray(location);
    }
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

// SHAPES
// 
void triangles(const std::vector<glm::vec2>& _positions, Shader* _program) {
    if (_program == nullptr) {

        if (shaderPtr == nullptr) {
            shaderPtr = getFillShader();
            shaderChange = true;
        }

        shaderPtr->use();
        shaderPtr->setUniform("u_color", stroke_color);
        shaderPtr->setUniform("u_modelViewProjectionMatrix", getMatrixModelProjectionView() );

        _program = shaderPtr;
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

void addLight(Light& _light, const std::string& _name) { lights[_name] = LightPtr(&_light);  }
void addLight(Light* _light, const std::string& _name) { lights[_name] = LightPtr(_light);  }

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

void shader(Shader& _shader) { shader(&_shader); }
void shader(Shader* _shader) {
    if (shaderPtr == nullptr || shaderPtr != _shader) {
        shaderPtr = _shader; 
        shaderChange = true;
    }

    shaderPtr->textureIndex = 0;
    shaderPtr->use();

    shaderPtr->setUniform("u_date", getDate() );
    shaderPtr->setUniform("u_resolution", (float)getWindowWidth(), (float)getWindowHeight() );
    shaderPtr->setUniform("u_mouse", (float)getMouseX(), (float)getMouseY() );
    shaderPtr->setUniform("u_time", (float)getTimeSec() );
    shaderPtr->setUniform("u_delta", (float)getDelta() );

    if (cameraPtr) {
        shaderPtr->setUniform("u_modelViewProjectionMatrix", cameraPtr->getProjectionViewMatrix() * matrix_model );
        shaderPtr->setUniform("u_projectionMatrix", cameraPtr->getProjectionMatrix());
        shaderPtr->setUniform("u_normalMatrix", cameraPtr->getNormalMatrix());
        shaderPtr->setUniform("u_viewMatrix", cameraPtr->getViewMatrix() );

        shaderPtr->setUniform("u_camera", -cameraPtr->getPosition() );
        shaderPtr->setUniform("u_cameraDistance", cameraPtr->getDistance());
        shaderPtr->setUniform("u_cameraNearClip", cameraPtr->getNearClip());
        shaderPtr->setUniform("u_cameraFarClip", cameraPtr->getFarClip());
        shaderPtr->setUniform("u_cameraEv100", cameraPtr->getEv100());
        shaderPtr->setUniform("u_cameraExposure", cameraPtr->getExposure());
        shaderPtr->setUniform("u_cameraAperture", cameraPtr->getAperture());
        shaderPtr->setUniform("u_cameraShutterSpeed", cameraPtr->getShutterSpeed());
        shaderPtr->setUniform("u_cameraSensitivity", cameraPtr->getSensitivity());
        shaderPtr->setUniform("u_cameraChange", cameraPtr->bChange);
        shaderPtr->setUniform("u_iblLuminance", 30000.0f * cameraPtr->getExposure());
    }
    else
        shaderPtr->setUniform("u_modelViewProjectionMatrix", getOrthoMatrix() * matrix_model );

    shaderPtr->setUniform("u_color", fill_color );

    // Pass Light Uniforms
    if (lights.size() == 1) {
        LightPtrs::iterator it = lights.begin();

        shaderPtr->setUniform("u_lightColor", it->second->color);
        shaderPtr->setUniform("u_lightIntensity", it->second->intensity);

        if (it->second->getType() != ada::LIGHT_DIRECTIONAL)
            shaderPtr->setUniform("u_light", it->second->getPosition());
        if (it->second->getType() == ada::LIGHT_DIRECTIONAL || it->second->getType() == ada::LIGHT_SPOT)
            shaderPtr->setUniform("u_lightDirection", it->second->direction);
        if (it->second->falloff > 0)
            shaderPtr->setUniform("u_lightFalloff", it->second->falloff);

        // shaderPtr->setUniform("u_lightMatrix", it->second->getBiasMVPMatrix() );
        // shaderPtr->setUniformDepthTexture("u_lightShadowMap", it->second->getShadowMap(), shaderPtr->textureIndex++ );
    }
    else {
        for (LightPtrs::iterator it = lights.begin(); it != lights.end(); ++it) {
            std::string name = "u_" + it->first;

            shaderPtr->setUniform(name + "Color", it->second->color);
            shaderPtr->setUniform(name + "Intensity", it->second->intensity);
            if (it->second->getType() != ada::LIGHT_DIRECTIONAL)
                shaderPtr->setUniform(name, it->second->getPosition());
            if (it->second->getType() == ada::LIGHT_DIRECTIONAL || it->second->getType() == ada::LIGHT_SPOT)
                shaderPtr->setUniform(name + "Direction", it->second->direction);
            if (it->second->falloff > 0)
                shaderPtr->setUniform(name +"Falloff", it->second->falloff);

            shaderPtr->setUniform(name + "Matrix", it->second->getBiasMVPMatrix() );
            // shaderPtr->setUniformDepthTexture(name + "ShadowMap", it->second->getShadowMap(), _shader->textureIndex++ );
        }
    } 
}

void resetShader() { shaderPtr = nullptr; }

void model(Vbo& _vbo, Shader* _program) { model(&_vbo, _program); }
void model(Vbo* _vbo, Shader* _program) {
    if (_program == nullptr) {
        if (shaderPtr == nullptr) {
            shaderPtr = getFillShader();
            shaderChange = true;
        }
        _program = shaderPtr;
    }

    if (shaderChange) {
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

    _vbo->render(_program);
}

void model(Mesh& _mesh, Shader* _program) { model(&_mesh, _program); }
void model(Mesh* _mesh, Shader* _program) {
        if (_program == nullptr) {
        if (shaderPtr == nullptr) {
            shaderPtr = getFillShader();
            shaderChange = true;
        }
        _program = shaderPtr;
    }

    bool bColor = (_mesh->haveColors() && _mesh->getColorsTotal()  == _mesh->getVerticesTotal());
    bool bNormals = (_mesh->haveNormals() && _mesh->getNormalsTotal() == _mesh->getVerticesTotal());
    bool bTexCoords = (_mesh->haveTexCoords()  && _mesh->getTexCoordsTotal() == _mesh->getVerticesTotal());
    bool bTangents = (_mesh->haveTangents() && _mesh->getTangentsTotal() == _mesh->getVerticesTotal());

    std::vector<GLfloat> data;
    for (unsigned int i = 0; i < _mesh->getVerticesTotal(); i++) {
        data.push_back(_mesh->getVertex(i).x);
        data.push_back(_mesh->getVertex(i).y);
        data.push_back(_mesh->getVertex(i).z);
        if (bColor) {
            data.push_back(_mesh->getColor(i).r);
            data.push_back(_mesh->getColor(i).g);
            data.push_back(_mesh->getColor(i).b);
            data.push_back(_mesh->getColor(i).a);
        }
        if (bNormals) {
            data.push_back(_mesh->getNormal(i).x);
            data.push_back(_mesh->getNormal(i).y);
            data.push_back(_mesh->getNormal(i).z);
        }
        if (bTexCoords) {
            data.push_back(_mesh->getTexCoord(i).x);
            data.push_back(_mesh->getTexCoord(i).y);
        }
        if (bTangents) {
            data.push_back(_mesh->getTangent(i).x);
            data.push_back(_mesh->getTangent(i).y);
            data.push_back(_mesh->getTangent(i).z);
            data.push_back(_mesh->getTangent(i).w);
        }
    }


    // Create Vertex Layout
    //
    std::vector<VertexAttrib> attribs;
    attribs.push_back({"position", 3, GL_FLOAT, false, &data[0]});
    int  nBits = 3;

    VertexLayout* vertexLayout = new VertexLayout(attribs);
    if (bColor) {
        _program->addDefine("MODEL_VERTEX_COLOR", "v_color");
        attribs.push_back({"color", 4, GL_FLOAT, false, 0});
        nBits += 4;
    }

    if (bNormals) {
        _program->addDefine("MODEL_VERTEX_NORMAL", "v_normal");
        attribs.push_back({"normal", 3, GL_FLOAT, false, 0});
        nBits += 3;
    }

    if (bTexCoords) {
        _program->addDefine("MODEL_VERTEX_TEXCOORD", "v_texcoord");
        attribs.push_back({"texcoord", 2, GL_FLOAT, false, 0});
        nBits += 2;
    }

    if (bTangents) {
        _program->addDefine("MODEL_VERTEX_TANGENT", "v_tangent");
        attribs.push_back({"tangent", 4, GL_FLOAT, false, 0});
        nBits += 4;
    }

    std::vector<INDEX_TYPE_GL> indices;
    if (!_mesh->haveIndices()) {
        if ( _mesh->getDrawMode() == LINES ) {
            for (size_t i = 0; i < _mesh->getVerticesTotal(); i++)
                indices.push_back(i);
        }
        else if ( _mesh->getDrawMode() == LINE_STRIP ) {
            for (size_t i = 1; i < _mesh->getVerticesTotal(); i++) {
                indices.push_back(i-1);
                indices.push_back(i);
            }
        }
    }
    else
        for (size_t i = 0; i < _mesh->getIndicesTotal(); i++)
            indices.push_back((INDEX_TYPE_GL)_mesh->getIndex(i));

    shader(_program);

    _program->setUniform("u_color", stroke_color);

    vertexLayout->bind(_program);

    if (_mesh->getDrawMode() == POINTS) {
#if !defined(PLATFORM_RPI) && !defined(DRIVER_GBM) && !defined(_WIN32) && !defined(__EMSCRIPTEN__)
        glEnable(GL_POINT_SPRITE);
        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
#endif
    }

    GLenum mode = GL_POINTS;
    if (_mesh->getDrawMode() == TRIANGLES)          mode = GL_TRIANGLES;
    else if (_mesh->getDrawMode() == LINE_STRIP)    mode = GL_LINE_STRIP;
    else if (_mesh->getDrawMode() == LINES)         mode = GL_LINES;
    else if (_mesh->getDrawMode() == TRIANGLE_FAN)  mode = GL_TRIANGLE_FAN;
    else if (_mesh->getDrawMode() == TRIANGLE_STRIP)mode = GL_TRIANGLE_STRIP;

    // Draw as elements or arrays
    if (!indices.empty() && mode != GL_POINTS) {
        #if defined(PLATFORM_RPI) || defined(DRIVER_GBM) || defined(__EMSCRIPTEN__)
        glDrawElements(mode, indices.size(), GL_UNSIGNED_SHORT, indices.data());
        #else
        glDrawElements(mode, indices.size(), GL_UNSIGNED_INT, indices.data());
        #endif
    } else if (!data.empty())
        glDrawArrays(mode, 0, data.size());

    vertexLayout->unbind(_program);

    if (_mesh->getDrawMode() == POINTS) {
#if !defined(PLATFORM_RPI) && !defined(DRIVER_GBM) && !defined(_WIN32) && !defined(__EMSCRIPTEN__)
        glDisable(GL_POINT_SPRITE);
        glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
#endif
    }
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

};