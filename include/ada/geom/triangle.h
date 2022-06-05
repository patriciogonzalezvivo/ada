#pragma once

#include <vector>

#include "glm/glm.hpp"

// #include "ada/scene/material.h"

namespace ada {

class Triangle {
public :
    
    Triangle();
    Triangle(const glm::vec3 &_p0, const glm::vec3 &_p1, const glm::vec3 &_p2);
    virtual ~Triangle();

    void set(const glm::vec3 &_p0, const glm::vec3 &_p1, const glm::vec3 &_p2);
    void setColors(const glm::vec4 &_p0, const glm::vec4 &_p1, const glm::vec4 &_p2);
    void setNormals(const glm::vec3 &_p0, const glm::vec3 &_p1, const glm::vec3 &_p2);
    void setTexCoords(const glm::vec2 &_p0, const glm::vec2 &_p1, const glm::vec2 &_p2);
    void setTangents(const glm::vec4 &_p0, const glm::vec4 &_p1, const glm::vec4 &_p2);

    bool containsPoint(const glm::vec3 &_p);
    
    // Getters:
    const glm::vec3&    operator[](size_t _index) const { return m_vertices[_index]; }
    void                setVertex(size_t _index, const glm::vec3& _vertex);
    const glm::vec3&    getVertex(size_t _index) const { return m_vertices[_index]; }
    glm::vec3           getVertex(const glm::vec3& _barycenterCoord ) const;

    glm::vec3           getCentroid() const { return (m_vertices[0] + m_vertices[1] + m_vertices[2]) * 0.3333333333333f; }
    glm::vec3           getBarycentricOf( const glm::vec3& _p ) const;
    
    bool                haveColors() const { return !m_colors.empty(); }
    void                setColor(const glm::vec4 &_color);
    void                setColor(float _r, float _g, float _b, float _a = 1.0f);
    void                setColor(size_t _index, const glm::vec4& _color);
    const glm::vec4&    getColor(size_t _index) const { return m_colors[_index]; }
    glm::vec4           getColor(const glm::vec3& _barycenterCoord ) const;

    bool                haveNormals() const { return !m_normals.empty(); }
    void                setNormal(size_t _index, const glm::vec3& _normal);
    const glm::vec3&    getNormal() const { return m_normal; }
    const glm::vec3&    getNormal(size_t _index) const { return m_normals[_index]; }
    glm::vec3           getNormal(const glm::vec3& _barycenterCoord ) const;

    bool                haveTexCoords() const { return !m_texCoords.empty(); }
    void                setTexCoord(size_t _index, const glm::vec2& _texcoord);
    const glm::vec2&    getTexCoord(size_t _index) const { return m_texCoords[_index]; }
    glm::vec2           getTexCoord(const glm::vec3& _barycenterCoord ) const;

    bool                haveTangents() const { return !m_tangents.empty(); }
    void                setTangent(size_t _index, const glm::vec4& _tangent);
    const glm::vec4&    getTangent(size_t _index) const { return m_tangents[_index]; }
    glm::vec4           getTangent(const glm::vec3& _barycenterCoord ) const;
    
    // MaterialConstPtr    material = nullptr;

    static bool         compare(const Triangle& a, const Triangle& b, int axis) {
        return a.getCentroid()[axis] < b.getCentroid()[axis];
    }

    static bool         compareX (const Triangle& a, const Triangle& b) { return compare(a, b, 0); }
    static bool         compareY (const Triangle& a, const Triangle& b) { return compare(a, b, 1); }
    static bool         compareZ (const Triangle& a, const Triangle& b) { return compare(a, b, 2); }
    
private:
    glm::vec3               m_vertices[3];
    glm::vec3               m_normal;
    float                   m_area;

    std::vector<glm::vec4>  m_colors;
    std::vector<glm::vec3>  m_normals;
    std::vector<glm::vec2>  m_texCoords;
    std::vector<glm::vec4>  m_tangents;
    
};


}