#include "ada/geom/triangle.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/norm.hpp"
#include "glm/glm.hpp"

namespace ada {

Triangle::Triangle() { 
}

Triangle::Triangle(const glm::vec3 &_p0, const glm::vec3 &_p1, const glm::vec3 &_p2) {
    set(_p0,_p1, _p2);
}

Triangle::~Triangle() {
}

void Triangle::set(const glm::vec3 &_p0, const glm::vec3 &_p1, const glm::vec3 &_p2) {
    m_vertices[0] = _p0;
    m_vertices[1] = _p1;
    m_vertices[2] = _p2;
    m_normal = glm::cross(m_vertices[0] - m_vertices[2], m_vertices[1] - m_vertices[0]);
    m_area = glm::length( m_normal );
    m_normal = m_normal / m_area;
    // m_normal = glm::normalize( m_normal );
}

glm::vec3 Triangle::getBarycentricOf(const glm::vec3& _p) const {
    const glm::vec3 f0 = m_vertices[0] - _p;
    const glm::vec3 f1 = m_vertices[1] - _p;
    const glm::vec3 f2 = m_vertices[2] - _p;
    // calculate the areas and factors (order of parameters doesn't matter):
    // float a = glm::length(glm::cross(m_vertices[0] - m_vertices[1], m_vertices[0] - m_vertices[2])); // main triangle area a
    return glm::vec3(   glm::length(glm::cross(f1, f2)),        // p1's triangle area / a
                        glm::length(glm::cross(f2, f0)),        // p2's triangle area / a 
                        glm::length(glm::cross(f0, f1))) / m_area;   // p3's triangle area / a
}

bool Triangle::containsPoint(const glm::vec3 &_p){
    const glm::vec3 v0 = m_vertices[2] - m_vertices[0];
    const glm::vec3 v1 = m_vertices[1] - m_vertices[0];
    const glm::vec3 v2 = _p - m_vertices[0];
    
    // Compute dot products
    float dot00 = glm::dot(v0, v0);
    float dot01 = glm::dot(v0, v1);
    float dot02 = glm::dot(v0, v2);
    float dot11 = glm::dot(v1, v1);
    float dot12 = glm::dot(v1, v2);
    
    // Compute barycentric coordinates
    float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    float v = (dot00 * dot12 - dot01 * dot02) * invDenom;
    
    // Check if point is in triangle
    return (u >= 0.0) && (v >= 0.0) && (u + v <= 1.0);
}

void Triangle::setVertex(size_t _index, const glm::vec3& _vertex) {
    m_vertices[_index] = _vertex;
}

void Triangle::setColor(const glm::vec4 &_color) {
    if (m_colors.empty())
        m_colors.resize(3);

    std::fill(m_colors.begin(),m_colors.begin()+3, _color);
}

void Triangle::setColor(float _r, float _g, float _b, float _a) {
    setColor(glm::vec4(_r, _g, _b, _a));
}

void Triangle::setColor(size_t _index, const glm::vec4& _color) {
    if (m_colors.empty()) {
        m_colors.resize(3);
        std::fill(m_colors.begin(),m_colors.begin()+3, _color);
    }
    else
        m_colors[_index] = _color;
}

void Triangle::setNormal(size_t _index, const glm::vec3& _normal) {
        if (m_normals.empty()) {
        m_normals.resize(3);
        std::fill(m_normals.begin(),m_normals.begin()+3, _normal);
    }
    else
        m_normals[_index] = _normal;
}

void Triangle::setTexCoord(size_t _index, const glm::vec2& _texcoord) {
    if (m_texCoords.empty()) {
        m_texCoords.resize(3);
        std::fill(m_texCoords.begin(),m_texCoords.begin()+3, _texcoord);
    }
    else
        m_texCoords[_index] = _texcoord;
}

void Triangle::setTangent(size_t _index, const glm::vec4& _tangent) {
        if (m_tangents.empty()) {
        m_tangents.resize(3);
        std::fill(m_tangents.begin(), m_tangents.begin()+3, _tangent);
    }
    else
        m_tangents[_index] = _tangent;
}

void Triangle::setColors(const glm::vec4 &_p0, const glm::vec4 &_p1, const glm::vec4 &_p2) {
    m_colors.resize(3);
    m_colors[0] = _p0;
    m_colors[1] = _p1;
    m_colors[2] = _p2;
}

void Triangle::setNormals(const glm::vec3 &_p0, const glm::vec3 &_p1, const glm::vec3 &_p2) {
    m_normals.resize(3);
    m_normals[0] = _p0;
    m_normals[1] = _p1;
    m_normals[2] = _p2;
}

void Triangle::setTexCoords(const glm::vec2 &_p0, const glm::vec2 &_p1, const glm::vec2 &_p2) {
    m_texCoords.resize(3);
    m_texCoords[0] = _p0;
    m_texCoords[1] = _p1;
    m_texCoords[2] = _p2;
}

void Triangle::setTangents(const glm::vec4 &_p0, const glm::vec4 &_p1, const glm::vec4 &_p2) {
    m_tangents.resize(3);
    m_tangents[0] = _p0;
    m_tangents[1] = _p1;
    m_tangents[2] = _p2;
}

glm::vec3 Triangle::getVertex(const glm::vec3& _barycenter) const {
    return  getVertex(0) * _barycenter.x +
            getVertex(1) * _barycenter.y +
            getVertex(2) * _barycenter.z;
}

glm::vec4 Triangle::getColor(const glm::vec3& _barycenter) const {

    // if (material != nullptr) {
    //     if ( material->haveProperty("diffuse") ) {
    //         if (havem_TexCoords()) {
    //             glm::vec2 uv = getTexCoord(_barycenter);
    //             return material->getColor("diffuse", uv);
    //         }
    //         else
    //             return material->getColor("diffuse");
    //     }
    // }

    if (haveColors())
        return  getColor(0) * _barycenter.x +
                getColor(1) * _barycenter.y +
                getColor(2) * _barycenter.z;
    else
        return glm::vec4(1.0f);
}

glm::vec3 Triangle::getNormal(const glm::vec3& _barycenter) const {
    glm::vec3 n;
    if (haveNormals())
        n = getNormal(0) * _barycenter.x +
            getNormal(1) * _barycenter.y +
            getNormal(2) * _barycenter.z;
    else
        n = getNormal();

    // if (material != nullptr && havem_TexCoords() && haveTangents()) {
    //     if ( material->haveProperty("normalmap") ) {
    //         glm::vec2 uv = getTexCoord(_barycenter);
    //         glm::vec4 t = getTangent(_barycenter);
    //         glm::vec3 b = glm::cross( n, glm::vec3(t.x, t.y, t.z) ) * t.w;
    //         glm::mat3 tbn = glm::mat3( t, b, n );

    //         return tbn * ( material->getColor("normalmap", uv) * 2.0f - 1.0f);
    //     }
    // }

    return n;
}

glm::vec2 Triangle::getTexCoord(const glm::vec3& _barycenter) const {
    glm::vec2 uv =  getTexCoord(0) * _barycenter.x +
                    getTexCoord(1) * _barycenter.y +
                    getTexCoord(2) * _barycenter.z;
    
    uv.x = 1.0 - uv.x;

    return uv;
}

glm::vec4 Triangle::getTangent(const glm::vec3& _barycenter ) const {
    return  getTangent(0) * _barycenter.x +
            getTangent(1) * _barycenter.y +
            getTangent(2) * _barycenter.z;
}

}