#pragma once

#include "glm/glm.hpp"

// #include "line.h"
#include "triangle.h"

namespace ada {

class BoundingBox {
public:

    glm::vec3 min;
    glm::vec3 max;
    
    BoundingBox(): min(std::numeric_limits<float>::max()), max(std::numeric_limits<float>::min()) {}
    
    void        set(const glm::vec2& _center) { set(glm::vec3(_center, 0.0f)); }
    void        set(const glm::vec3& _center) { min = _center; max = _center; }
    void        set(const glm::vec4& _bbox) { min.x = _bbox.x; min.y = _bbox.y; max.x = _bbox.z; max.y = _bbox.w; }
    void        operator = (const glm::vec4& _bbox) { set(_bbox); }

    float       getWidth() const { return fabs(max.x - min.x); }
    float       getHeight() const { return fabs(max.y - min.y); }
    float       getDepth() const { return fabs(max.z - min.z); }
    
    glm::vec3   getCenter() const { return (min + max) * 0.5f; }
    glm::vec3   getDiagonal() const { return max - min; }
    glm::vec4   get2DBoundingBox() const { return glm::vec4(min.x, min.y, max.x, max.y); }
    
    bool        containsX(float _x) const { return _x >= min.x && _x <= max.x; }
    bool        containsY(float _y) const { return _y >= min.y && _y <= max.y; }
    bool        containsZ(float _z) const { return _z >= min.z && _z <= max.z; }

    bool        contains(float _x, float _y) const { return containsX(_x) && containsY(_y); }
    bool        contains(const glm::vec2& _v) const { return containsX(_v.x) && containsY(_v.y); }

    bool        contains(float _x, float _y, float _z) const { return containsX(_x) && containsY(_y) && containsZ(_z); }
    bool        contains(const glm::vec3& _v) const { return containsX(_v.x) && containsY(_v.y) && containsZ(_v.z); }

    void        expand(float _value) {
        min -= _value;
        max += _value;
    }
    void        expand(float _x, float _y) {
        min.x = std::min(min.x, _x);
        max.x = std::max(max.x, _x);
        
        min.y = std::min(min.y, _y);
        max.y = std::max(max.y, _y);
    }

    void        expand(float _x, float _y, float _z) {
        expand(_x, _y);
        min.z = std::min(min.z, _z);
        max.z = std::max(max.z, _z);
    }

    void        expand(const glm::vec2& _v) { expand(_v.x, _v.y); }
    void        expand(const glm::vec3& _v) { expand(_v.x, _v.y, _v.z); }
    // void    expand(const Line& _l) { expand(_l[0]); expand(_l[1]); }
    void        expand(const Triangle& _t) { expand(_t[0]); expand(_t[1]); expand(_t[2]); }
    void        expand(const BoundingBox& _b) { expand(_b.min); expand(_b.max); }

    void        clean() { 
        min = glm::vec3(std::numeric_limits<float>::max()); 
        max = glm::vec3(std::numeric_limits<float>::min()); 
    }
    
};

}