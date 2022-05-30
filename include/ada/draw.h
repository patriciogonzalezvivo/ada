#pragma once

#include <vector>

#include "ada/gl/vbo.h"

#include "ada/gl/shader.h"
#include "ada/shaders/defaultShaders.h"

#include "ada/scene/camera.h"
#include "ada/scene/light.h"

#include "ada/geom/mesh.h"
#include "ada/geom/line.h"
#include "ada/geom/triangle.h"
#include "ada/geom/boundingBox.h"

#include "ada/font.h"

namespace ada {

enum PointShape {
    SQUARE_SHAPE = 0,
    SQUARE_OUTLINE_SHAPE,
    DOT_SHAPE,
    DOT_OUTLINE_SHAPE,
    CROSS_SHAPE,
    X_SHAPE
};

// GENERAL GL STATE
// ---------------------------------

// colorMode()

void clear();
void clear( float _brightness );
void clear( const glm::vec3& _color );
void clear( const glm::vec4& _color );

void fill( float _brightness );
void fill( float _red, float _green, float _blue);
void fill( float _red, float _green, float _blue, float _alpha);
void fill( const glm::vec3& _color );
void fill( const glm::vec4& _color );
void noFill();

void stroke( float _brightness );
void stroke( float _red, float _green, float _blue);
void stroke( float _red, float _green, float _blue, float _alpha);
void stroke( const glm::vec3& _color );
void stroke( const glm::vec4& _color );
void noStroke();
void strokeWeight(float _weight);

void resetMatrix();
void applyMatrix(const glm::mat3& _mat );
void applyMatrix(const glm::mat4& _mat );
const glm::mat4& getProjectionViewModelMatrix();
const glm::mat4& getProjectionViewMatrix();
const glm::mat4& getProjectionMatrix();
const glm::mat4& getViewMatrix();
const glm::mat4& getWorldMatrix();

void rotate(float _rad);
void rotateX(float _rad);
void rotateY(float _rad);
void rotateZ(float _rad);

void scale(float _s);
void scale(float _x, float _y, float _z = 1.0f);
void scale(const glm::vec2& _s);
void scale(const glm::vec3& _s);

void translate(float _x, float _y, float _z = 0.0f);
void translate(const glm::vec2& _t);
void translate(const glm::vec3& _t);

void push();
void pop();

// erase()
// noErase()

// POINTS
void pointSize( float _size );
void pointShape( PointShape _shape );
void points(const std::vector<glm::vec2>& _positions, Shader* _program = nullptr);
void points(const std::vector<glm::vec3>& _positions, Shader* _program = nullptr);
void points(const Line& _line, Shader* _program = nullptr);
void points(const Triangle& _triangle, Shader* _program = nullptr);
void points(const BoundingBox& _bbox, Shader* _program = nullptr);
void pointsBoundingBox(const glm::vec4& _bbox, Shader* _program = nullptr);

// arc()
// ellipse()
// circle()
// quad()
// square()
// triangle()

void line(float _x1, float _y1, float _x2, float _y2, Shader* _program = nullptr);
void line(const glm::vec2& _a, const glm::vec2& _b, Shader* _program = nullptr);
void line(const std::vector<glm::vec2>& _positions, Shader* _program = nullptr);
void line(float _x1, float _y1, float _z1, float _x2, float _y2, float _z2, Shader* _program = nullptr);
void line(const glm::vec3& _a, const glm::vec3& _b, Shader* _program = nullptr);
void line(const std::vector<glm::vec3>& _positions, Shader* _program = nullptr);
void line(const Line& _line, Shader* _program = nullptr);
void line(const Triangle& _triangle, Shader* _program = nullptr);
void line(const BoundingBox& _bbox, Shader* _program = nullptr);
void lineBoundingBox(const glm::vec4& _bbox, Shader* _program = nullptr);

void triangles(const std::vector<glm::vec2>& _positions, Shader* _program = nullptr);

void rect(float _x, float _y, float _w, float _h, Shader* _program = nullptr);
void rect(const glm::vec2& _pos, const glm::vec2& _size, Shader* _program = nullptr);

// 2D Text
// -----------------------------------
// textLeading()
// textStyle()
// textWidth()
// textAscent()
// textDescent()
// textWrap()
// loadFont()
// textFont()
void textAlign(FontAlign _align, Font* _font = nullptr);
void textSize(float _size, Font* _font = nullptr);
void text(const std::string& _text, const glm::vec2& _pos, Font* _font = nullptr );
void text(const std::string& _text, float _x, float _y, Font* _font = nullptr);

// plane()
// box()
// sphere()
// cylinder()
// cone()
// ellipsoid()
// torus()
// p5.Geometry

// Material
Shader loadShader(const std::string& _fragFile, const std::string& _vertFile);
Shader createShader(const std::string& _fragSrc = "", const std::string& _vertSrc = "");
Shader createShader(DefaultShaders _frag, DefaultShaders _vert);
void shader(Shader& _shader);
void shader(Shader* _shader);
Shader* getShader();
void resetShader();

void texture(Texture& _texture, const std::string _name = "");
void texture(Texture* _texture, const std::string _name = "");

// textureMode()
// textureWrap()
// normalMaterial()
// ambientMaterial()
// emissiveMaterial()
// specularMaterial()
// shininess()
// p5.Shader

// Camera
// camera()
void perspective(float _fovy, float _aspect, float _near, float _far);
void ortho(float _left, float _right, float _bottom, float _top,  float _near, float _far);
// frustum()
CameraPtr createCamera();

// p5.Camera
void setCamera(Camera &_camera);
CameraPtr getCamera();

// Interaction
// orbitControl()
// debugMode()
// noDebugMode()

// Lights
// ambientLight()
// specularColor()
// directionalLight()
// pointLight()
// lights()
// lightFalloff()
// spotLight()
// noLights()

void addLight(Light& _light, const std::string& _name = "default");
void addLight(Light* _light, const std::string& _name = "default");

// void loadModel( const std::string& _filename );
void model(Vbo& _vbo, Shader* _program = nullptr);
void model(Vbo* _vbo, Shader* _program = nullptr);

void model(Mesh& _mesh, Shader* _program = nullptr);
void model(Mesh* _mesh, Shader* _program = nullptr);

};