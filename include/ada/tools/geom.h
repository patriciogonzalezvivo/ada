#pragma once

#include <vector>

#include "glm/glm.hpp"
#include "ada/gl/mesh.h"

namespace ada {

float getArea(const std::vector<glm::vec3>& _pts);
glm::vec3 getCentroid(const std::vector<glm::vec3>& _pts);
void getBoundingBox(const std::vector<glm::vec3>& _pts, glm::vec3& _min, glm::vec3& _max);
void expandBoundingBox(const std::vector<glm::vec3>& _pts, glm::vec3& _min, glm::vec3& _max);
void expandBoundingBox(const glm::vec3& _pt, glm::vec3& _min, glm::vec3& _max);

void calcNormal(const glm::vec3& _v0, const glm::vec3& _v1, const glm::vec3& _v2, glm::vec3& _N);

Mesh lineMesh(const glm::vec3 &_a, const glm::vec3 &_b);

Mesh lineToMesh(const glm::vec3 &_a, const glm::vec3 &_dir, float _size);

Mesh crossMesh(const glm::vec3 &_pos, float _width);

Mesh rectMesh(float _x, float _y, float _w, float _h);

Mesh axisMesh(float _size, float _y = 0.0);

Mesh gridMesh(float _size, int _segments, float _y = 0.0);
Mesh gridMesh(float _width, float _height, int _columns, int _rows, float _y = 0.0);

Mesh floorMesh(float _area, int _subD, float _y = 0.0);

Mesh cubeMesh(float _size = 1.0f);
Mesh cubeCornersMesh(const glm::vec3 &_min_v, const glm::vec3 &_max_v, float _size);
Mesh cubeCornersMesh(const std::vector<glm::vec3> &_pts, float _size = 1.0);

Mesh sphereMesh(int _resolution = 24, float _radius = 1.0f);

Mesh sphereHalfMesh(int _resolution = 24, float _radius = 1.0f);

}
