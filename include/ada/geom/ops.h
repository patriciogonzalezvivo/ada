#pragma once

#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

#include "mesh.h"
#include "line.h"
#include "triangle.h"
#include "boundingBox.h"

namespace ada {

// 2D
void                    simplify(std::vector<glm::vec2> &_pts, float _tolerance=0.3f);
float                   getArea(const std::vector<glm::vec2>& _pts);
glm::vec2               getCentroid(const std::vector<glm::vec2>& _pts);
std::vector<glm::vec2>  getSimplify(const std::vector<glm::vec2> &_pts, float _tolerance=0.3f);
std::vector<glm::vec2>  getConvexHull(const std::vector<glm::vec2> &_pts);
BoundingBox             getBoundingBox(const std::vector<glm::vec2>& _points);

// 3D
glm::vec3   getCentroid(const std::vector<glm::vec3>& _pts);
BoundingBox getBoundingBox(const Mesh& _mesh);
BoundingBox getBoundingBox(const std::vector<glm::vec3>& _points);
BoundingBox getBoundingBox(const std::vector<Line>& _lines);
BoundingBox getBoundingBox(const std::vector<Triangle>& _triangles);

void        calcNormal(const glm::vec3& _v0, const glm::vec3& _v1, const glm::vec3& _v2, glm::vec3& _N);

// points
//
void        transform(std::vector<glm::vec3>& _points, const glm::quat& _mat);
void        transform(std::vector<glm::vec3>& _points, const glm::mat3& _mat);
void        transform(std::vector<glm::vec3>& _points, const glm::mat4& _mat);

void        scale(std::vector<glm::vec3>& _points, float _v);
void        scaleX(std::vector<glm::vec3>& _points, float _x);
void        scaleY(std::vector<glm::vec3>& _points, float _y);
void        scaleZ(std::vector<glm::vec3>& _points, float _z);
void        scale(std::vector<glm::vec3>& _points, const glm::vec3& _v);
void        scale(std::vector<glm::vec3>& _points, float _x, float _y, float _z = 1.0f);

void        translateX(std::vector<glm::vec3>& _points, float _x);
void        translateY(std::vector<glm::vec3>& _points, float _y);
void        translateZ(std::vector<glm::vec3>& _points, float _z);
void        translate(std::vector<glm::vec3>& _points, const glm::vec3& _v);
void        translate(std::vector<glm::vec3>& _points, float _x, float _y, float _z = 0.0f);

void        rotateX(std::vector<glm::vec3>& _points, float _rad);
void        rotateY(std::vector<glm::vec3>& _points, float _rad);
void        rotateZ(std::vector<glm::vec3>& _points, float _rad);
void        rotate(std::vector<glm::vec3>& _points, float _rad, const glm::vec3& _axis );
void        rotate(std::vector<glm::vec3>& _points, float _rad, float _x, float _y, float _z );

void        center(std::vector<glm::vec3>& _points);

// Mesh
//
inline void transform(Mesh& _mesh, const glm::quat& _mat) { transform(_mesh.m_vertices, _mat); }
inline void transform(Mesh& _mesh, const glm::mat3& _mat) { transform(_mesh.m_vertices, _mat); }
inline void transform(Mesh& _mesh, const glm::mat4& _mat) { transform(_mesh.m_vertices, _mat); }

inline void scale(Mesh& _mesh, float _v) { scale(_mesh.m_vertices, _v); };
inline void scaleX(Mesh& _mesh, float _x) { scaleX(_mesh.m_vertices, _x); };
inline void scaleY(Mesh& _mesh, float _y) { scaleY(_mesh.m_vertices, _y); };
inline void scaleZ(Mesh& _mesh, float _z) { scaleZ(_mesh.m_vertices, _z); };
inline void scale(Mesh& _mesh, const glm::vec3& _v) { scale(_mesh.m_vertices, _v); };
inline void scale(Mesh& _mesh, float _x, float _y, float _z = 1.0f) { scale(_mesh.m_vertices, _x, _y, _z); };

inline void translateX(Mesh& _mesh, float _x) { translateX(_mesh.m_vertices, _x); };
inline void translateY(Mesh& _mesh, float _y) { translateY(_mesh.m_vertices, _y); };
inline void translateZ(Mesh& _mesh, float _z) { translateZ(_mesh.m_vertices, _z); };
inline void translate(Mesh& _mesh, const glm::vec3& _v) { translate(_mesh.m_vertices, _v); };
inline void translate(Mesh& _mesh, float _x, float _y, float _z = 0.0f) { translate(_mesh.m_vertices, _x, _y, _z);};

void        rotateX(Mesh& _mesh, float _rad);
void        rotateY(Mesh& _mesh, float _rad);
void        rotateZ(Mesh& _mesh, float _rad);
void        rotate(Mesh& _mesh, float _rad, const glm::vec3& _axis );
void        rotate(Mesh& _mesh, float _rad, float _x, float _y, float _z );

inline void center(Mesh& _mesh) { center(_mesh.m_vertices); };

}
