#pragma once

#include <vector>
#include "shader.h"
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
    ENABLE_ALPHA = 0,       // Alpha is the default
    ENABLE_ADD,
    ENABLE_MULTIPLY,
    ENABLE_SCREEN,
    ENABLE_SUBSTRACT,
    DISABLE_BLEND
};

// GENERAL GL STATE
// ---------------------------------

// background()
void clear();
void clear( float _brightness );
void clear( const glm::vec3& _color );
void clear( const glm::vec4& _color );

void blendMode( BlendMode _mode );

// colorMode()

void fill( float _brightness );
void fill( float _red, float _green, float _blue);
void fill( float _red, float _green, float _blue, float _alpha);
void fill( const glm::vec3& _color );
void fill( const glm::vec4& _color );

// noFill()
// noStroke()

void stroke( float _brightness );
void stroke( float _red, float _green, float _blue);
void stroke( float _red, float _green, float _blue, float _alpha);
void stroke( const glm::vec3& _color );
void stroke( const glm::vec4& _color );

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
// rect()
// square()
// triangle()

void points(const std::vector<glm::vec2>& _positions, Shader* _program = nullptr);

void line(float _x1, float _y1, float _x2, float _y2, Shader* _program = nullptr);
void line(const glm::vec2& _a, const glm::vec2& _b, Shader* _program = nullptr);
void line(const std::vector<glm::vec2>& _positions, Shader* _program = nullptr);

// 2D Text
// -----------------------------------
void textAlign(FontAlign _align);
// textLeading()
void textSize(float _size);
// textStyle()
// textWidth()
// textAscent()
// textDescent()
// textWrap()
// loadFont()
void text(const std::string& _text, const glm::vec2& _pos);
void text(const std::string& _text, float _x, float _y);
// textFont()

// 3D Shapes
// ---------------------------------------------------
void points(const std::vector<glm::vec3>& _positions, Shader* _program = nullptr);

void line(float _x1, float _y1, float _z1, float _x2, float _y2, float _z2, Shader* _program = nullptr);
void line(const glm::vec3& _a, const glm::vec3& _b, Shader* _program = nullptr);
void line(const std::vector<glm::vec3>& _positions, Shader* _program = nullptr);

void pointsBoundingBox(const glm::vec4& _bbox, Shader* _program = nullptr);
void lineBoundingBox(const glm::vec4& _bbox, Shader* _program = nullptr);

};