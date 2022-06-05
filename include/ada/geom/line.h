#pragma once 

#include <vector>
#include "glm/glm.hpp"

namespace ada {

class Line{
public:
    Line();
    Line(const glm::vec3& _p0, const glm::vec3& _p1);
    
    void set(const glm::vec3& _p0, const glm::vec3& _p1);
    
    float               getMagintude() const { return glm::length(m_direction); }
    float               getMagintude2() const { return glm::dot(m_direction, m_direction); }
    glm::vec3           getCentroid() const { return (m_points[0] + m_points[1]) * 0.5f; }
    const glm::vec3&    getDireciton() const { return m_direction; }

    glm::vec3           getAt(float _t) const { return m_points[0] + m_direction * _t; }
    const glm::vec3&    getPoint(size_t _index) const { return m_points[_index]; }
    const glm::vec3&    operator[](size_t _index) const { return m_points[_index]; }

    bool                haveColors() const { return !m_colors.empty(); }
    void                setColor(const glm::vec4 &_color);
    void                setColor(float _r, float _g, float _b, float _a = 1.0f);
    void                setColor(size_t _index, const glm::vec4& _color);
    const glm::vec4&    getColor(size_t _index) const { return m_colors[_index]; }
    glm::vec4           getColorAt(float _t) const;
    
    static bool         compare(const Line& a, const Line& b, int axis) {
        return a.getCentroid()[axis] < b.getCentroid()[axis];
    }

    static bool         compareX (const Line& a, const Line& b) { return compare(a, b, 0); }
    static bool         compareY (const Line& a, const Line& b) { return compare(a, b, 1); }
    static bool         compareZ (const Line& a, const Line& b) { return compare(a, b, 2); }
    
private:
    std::vector<glm::vec4>  m_colors;
    glm::vec3               m_points[2];
    glm::vec3               m_direction;
};

}