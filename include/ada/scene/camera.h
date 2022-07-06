#pragma once

#include "node.h"
#include "../geom/boundingBox.h"

#include <map>
#include <vector>

namespace ada {

enum ProjectionType {
    ORTHO, 
    PERSPECTIVE,
    PERSPECTIVE_VIRTUAL_OFFSET,
    CUSTOM
};

class Camera: public Node {
public:
    Camera();
    virtual ~Camera();

    virtual void        setProjection(ProjectionType cam_mode);
    virtual void        setProjection(const glm::mat4& _M );

    virtual void        setFOV(double _fov);
    virtual void        setAspect(float _aspect) { m_aspect = _aspect; }
    virtual void        setViewport(int _width, int _height);
    virtual void        setClipping(double _near_clip_distance, double _far_clip_distance);
    virtual void        setDistance(float _distance);
    virtual void        setTarget(glm::vec3 _target);
    virtual void        setVirtualOffset(float _scale, int _currentViewIndex, int _totalViews, float aspect = 1.0f);

    virtual void        setExposure(float _aperture, float _shutterSpeed, float _sensitivity);

    virtual glm::vec3   worldToCamera(const glm::vec3* _world, glm::mat4* _model = nullptr) const;
    virtual glm::vec3   worldToCamera(const glm::vec3& _world, glm::mat4* _model = nullptr) const;
    virtual glm::vec3   worldToScreen(const glm::vec3* _world, glm::mat4* _model = nullptr) const;
    virtual glm::vec3   worldToScreen(const glm::vec3& _world, glm::mat4* _model = nullptr) const;
    virtual BoundingBox worldToScreen(const BoundingBox& _bbox, glm::mat4* _model = nullptr) const;

    //Getting Functions
    virtual const ProjectionType&   getProjectionType() const { return m_projectionType;};

    virtual const float         getFOV() const { return m_fov; }
    virtual const float         getAspect() const { return m_aspect; }
    virtual const float         getFarClip() const { return m_farClip; }
    virtual const float         getNearClip() const { return m_nearClip; }
    virtual const float         getDistance() const;

    virtual const float         getEv100() const { return m_ev100; }
    virtual const float         getExposure() const { return m_exposure; }
    virtual const float         getAperture() const { return m_aperture; }          //! returns this camera's aperture in f-stops
    virtual const float         getShutterSpeed() const { return m_shutterSpeed; }  //! returns this camera's shutter speed in seconds
    virtual const float         getSensitivity() const { return m_sensitivity; }    //! returns this camera's sensitivity in ISO
    virtual const glm::vec3&    getTarget() const { return m_target; }
    
    virtual const glm::vec3&    getPosition() const;
    virtual const glm::mat4&    getViewMatrix() const;
    virtual const glm::mat3&    getNormalMatrix() const { return m_normalMatrix; }
    virtual const glm::mat4&    getProjectionMatrix() const { return m_projectionMatrix; }
    virtual const glm::mat4&    getProjectionViewMatrix() const { return m_projectionViewMatrix; }

protected:

    virtual void        onPositionChanged();
    virtual void        onOrientationChanged();
    virtual void        onScaleChanged();

    virtual void        updateCameraSettings();
    virtual void        updateProjectionViewMatrix();

private:
    glm::mat4   m_projectionViewMatrix;

    glm::mat4   m_projectionMatrix;
    glm::mat4   m_viewMatrix;
    glm::mat3   m_normalMatrix;

    glm::vec3   m_target;
    glm::vec3   m_position_offset;

    double      m_aspect;
    double      m_fov;
    double      m_nearClip;
    double      m_farClip;

    float       m_exposure; 
    float       m_ev100;
    float       m_aperture;
    float       m_shutterSpeed;
    float       m_sensitivity;

    ProjectionType m_projectionType;
};

}