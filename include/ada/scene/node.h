#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

namespace ada {

enum NodeType {
    EMPTY_NODE = 0, 
    MODEL_NODE,
    LIGHT_NODE,
    CAMERA_NODE
};

class Node {
public:

    Node();
    virtual ~Node() { };

    // SET
    virtual void        setProperties(const Node& _other);

    virtual void        setScale(const glm::vec3& _scale);

    virtual void        setPosition(const glm::vec3& _pos);
    
    virtual void        setOrientation(const glm::vec3& _ori);
    virtual void        setOrientation(const glm::quat& _ori);
    virtual void        setOrientation(const glm::mat3& _ori);
    virtual void        setOrientation(const glm::mat4& _ori);
    
    virtual void        setTransformMatrix(const glm::mat4& _m);

    // GET 
    // virtual NodeType            getType() const { return m_type; }
    virtual const glm::vec3&    getScale() const { return m_scale; };
    virtual const glm::vec3&    getPosition() const { return m_position; };

    virtual const glm::vec3&    getXAxis() const { return m_axis[0]; }
    virtual const glm::vec3&    getYAxis() const { return m_axis[1]; };
    virtual const glm::vec3&    getZAxis() const { return m_axis[2]; };

    virtual const glm::vec3&    getRightDir() const { return getXAxis(); }
    virtual const glm::vec3&    getUpDir() const { return getYAxis(); }
    virtual glm::vec3           getForwardDir() const { return -getZAxis(); }

    virtual const glm::quat&    getOrientationQuat() const { return m_orientation; }
    virtual glm::vec3           getOrientationEuler() const { return glm::eulerAngles(m_orientation); }
    virtual glm::mat4           getOrientationMatrix() const { return glm::toMat4(m_orientation); };
    
    virtual float               getPitch() const { return getOrientationEuler().x; }
    virtual float               getHeading() const { return getOrientationEuler().y; }
    virtual float               getRoll() const { return getOrientationEuler().z; };

    virtual const glm::mat4&    getTransformMatrix() const { return m_transformMatrix; };

    // ACTIONS
    virtual void        scale(const glm::vec3& _scale);

    virtual void        translate(const glm::vec3& _offset);
    virtual void        truck(float _amount) { translate(getXAxis() * _amount); };
    virtual void        boom(float _amount) { translate(getYAxis() * _amount); };
    virtual void        dolly(float _amount) { translate(getZAxis() * _amount); };

    virtual void        orbit(float _azimuth, float _elevation, float _distance, const glm::vec3& _centerPoint = glm::vec3(0.0));

    virtual void        tilt(float _degrees) { rotate(glm::angleAxis(glm::radians(_degrees), getXAxis())); };
    virtual void        pan(float _degrees) { rotate(glm::angleAxis(glm::radians(_degrees), getYAxis())); };
    virtual void        roll(float _degrees) { rotate(angleAxis(glm::radians(_degrees), getZAxis())); };

    virtual void        rotate(const glm::quat& _q);
    virtual void        rotateAround(const glm::quat& _q, const glm::vec3& _point);
    virtual void        lookAt(const glm::vec3& _lookAtPosition, glm::vec3 _upVector = glm::vec3(0.0, 1.0, 0.0));
    
    virtual void        apply(const glm::mat4& _m);

    virtual void        reset();

    std::vector<Node*>  childs;

    bool                bChange;

protected:
    virtual void        createMatrix();
    virtual void        updateAxis();

    virtual void        onPositionChanged() {};
    virtual void        onOrientationChanged() {};
    virtual void        onScaleChanged() {};

    glm::mat4           m_transformMatrix;
    glm::vec3           m_axis[3];

    glm::vec3           m_position;
    glm::quat           m_orientation;
    glm::vec3           m_scale;

    // NodeType            m_type;

    friend class        Label;
};

}