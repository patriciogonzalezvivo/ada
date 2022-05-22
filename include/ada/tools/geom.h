#pragma once

#include <vector>

#include "glm/glm.hpp"
#include "ada/gl/mesh.h"

namespace ada {

// 2D
void simplify(std::vector<glm::vec2> &_pts, float _tolerance=0.3f);
std::vector<glm::vec2> getSimplify(const std::vector<glm::vec2> &_pts, float _tolerance=0.3f);
std::vector<glm::vec2> getConvexHull(const std::vector<glm::vec2> &_pts);
float getArea(const std::vector<glm::vec2>& _pts);

// 2D & 3D
glm::vec2 getCentroid(const std::vector<glm::vec2>& _pts);
glm::vec3 getCentroid(const std::vector<glm::vec3>& _pts);

glm::vec4 getBoundingBox(const std::vector<glm::vec2>& _pts);
void expandBoundingBox(const std::vector<glm::vec2>& _pts, glm::vec2& _min, glm::vec2& _max);
void expandBoundingBox(const glm::vec2& _pt, glm::vec2& _min, glm::vec2& _max);

void getBoundingBox(const std::vector<glm::vec3>& _pts, glm::vec3& _min, glm::vec3& _max);
void expandBoundingBox(const std::vector<glm::vec3>& _pts, glm::vec3& _min, glm::vec3& _max);
void expandBoundingBox(const glm::vec3& _pt, glm::vec3& _min, glm::vec3& _max);

void calcNormal(const glm::vec3& _v0, const glm::vec3& _v1, const glm::vec3& _v2, glm::vec3& _N);

}
