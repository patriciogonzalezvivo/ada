#pragma once

#include <vector>
#include "shader.h"
#include "vbo.h"
#include "ada/tools/font.h"

namespace ada {

enum PointShape {
    SQUARE_SHAPE = 0,
    SQUARE_OUTLINE_SHAPE,
    DOT_SHAPE,
    DOT_OUTLINE_SHAPE,
    CROSS_SHAPE,
    X_SHAPE
};

enum BlendMode {
    BLEND_NONE = 0,
    BLEND_ALPHA,
    BLEND_ADD,
    BLEND_MULTIPLY,
    BLEND_SCREEN,
    BLEND_SUBSTRACT
};

enum CullingMode {
    CULL_NONE = 0,
    CULL_FRONT,
    CULL_BACK,
    CULL_BOTH
};

// GENERAL GL STATE
// ---------------------------------

// background()
void clear();
void clear( float _brightness );
void clear( const glm::vec3& _color );
void clear( const glm::vec4& _color );

void blendMode( BlendMode _mode );
void cullingMode( CullingMode _mode );

// colorMode()

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

// erase()
// noErase()

void pointSize( float _size );
void pointShape( PointShape _shape);

// 2D Shapes
// -----------------------------------
// arc()
// ellipse()
// circle()
// quad()
// square()
// triangle()
void triangles(const std::vector<glm::vec2>& _positions, Shader* _program = nullptr);

void rect(float _x, float _y, float _w, float _h, Shader* _program = nullptr);
void rect(const glm::vec2& _pos, const glm::vec2& _size, Shader* _program = nullptr);

void points(const std::vector<glm::vec2>& _positions, Shader* _program = nullptr);

void line(float _x1, float _y1, float _x2, float _y2, Shader* _program = nullptr);
void line(const glm::vec2& _a, const glm::vec2& _b, Shader* _program = nullptr);
void line(const std::vector<glm::vec2>& _positions, Shader* _program = nullptr);

// 2D Text
// -----------------------------------
void textAlign(FontAlign _align, Font* _font = nullptr);
// textLeading()
void textSize(float _size, Font* _font = nullptr);
// textStyle()
// textWidth()
// textAscent()
// textDescent()
// textWrap()
// loadFont()
void text(const std::string& _text, const glm::vec2& _pos, Font* _font = nullptr );
void text(const std::string& _text, float _x, float _y, Font* _font = nullptr);
// textFont()

// 3D Shapes
// ---------------------------------------------------
void points(const std::vector<glm::vec3>& _positions, Shader* _program = nullptr);
void points(Vbo* _vbo);

void line(float _x1, float _y1, float _z1, float _x2, float _y2, float _z2, Shader* _program = nullptr);
void line(const glm::vec3& _a, const glm::vec3& _b, Shader* _program = nullptr);
void line(const std::vector<glm::vec3>& _positions, Shader* _program = nullptr);
void line(Vbo* _vbo);

void pointsBoundingBox(const glm::vec4& _bbox, Shader* _program = nullptr);
void lineBoundingBox(const glm::vec4& _bbox, Shader* _program = nullptr);

};