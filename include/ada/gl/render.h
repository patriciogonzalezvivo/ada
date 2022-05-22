#pragma once

#include <vector>
#include "shader.h"

namespace ada {

// background()
void clear();
void clear( float _brightness );
void clear( const glm::vec3& _color );
void clear( const glm::vec4& _color );
// colorMode()
// fill()
// noFill()
// noStroke()
void stroke( float _brightness );
void stroke( const glm::vec3& _color );
void stroke( const glm::vec4& _color );
// erase()
// noErase()

// 2D
// -----------------------------------
// arc()
// ellipse()
// circle()
// point()
// quad()
// rect()
// square()
// triangle()
void renderBoundingBox(const glm::vec4& _bbox, Shader* _program = nullptr);
void line(const glm::vec2& _a, const glm::vec2& _b, Shader* _program = nullptr);
void line(const std::vector<glm::vec2>& _positions, Shader* _program = nullptr);

// 3D
void line(const glm::vec3& _a, const glm::vec3& _b, Shader* _program = nullptr);
void line(const std::vector<glm::vec3>& _positions, Shader* _program = nullptr);

};