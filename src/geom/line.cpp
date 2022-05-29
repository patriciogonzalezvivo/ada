#include "ada/geom/line.h"

namespace ada {

Line::Line() : m_direction(0.0) { 

}

Line::Line(const glm::vec3& _p0, const glm::vec3& _p1) {
    set(_p0,_p1);
}
    
void Line::set(const glm::vec3& _p0, const glm::vec3& _p1) {
    m_points[0] = _p0;
    m_points[1] = _p1;
    m_direction = m_points[1] - m_points[0];
}

void Line::setColor(const glm::vec4 &_color) {
    m_colors.resize(2);
    m_colors[0] = _color;
    m_colors[1] = _color;
}

void Line::setColor(float _r, float _g, float _b, float _a) {
    setColor(glm::vec4(_r, _g, _b, _a));
}

void Line::setColor(size_t _index, const glm::vec4& _color) {
    if (m_colors.empty()) {
        m_colors.resize(2);
        std::fill(m_colors.begin(),m_colors.begin()+2, _color);
    }
    else
        m_colors[_index] = _color;
}

glm::vec4 Line::getColorAt(float _t) const {
    if (haveColors())
        return m_colors[0] * (1.0f-_t) + m_colors[1] * _t;
    else
        return glm::vec4(0.0); 
}

}