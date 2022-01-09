#include "ada/scene/light.h"

#include <iostream>

namespace ada {

Light::Light(): 
    color(1.0), 
    direction(0.0),
    intensity(1.0),
    falloff(-1.0),
    m_mvp_biased(1.0), 
    m_mvp(1.0), 
    m_type(LIGHT_DIRECTIONAL) {
    std::fill(m_viewport, m_viewport+4, 0);
}

Light::Light(glm::vec3 _dir): m_mvp_biased(1.0), m_mvp(1.0) {
    color = glm::vec3(1.0); 
    intensity = 1.0;
    falloff = -1.0; 
    m_type = LIGHT_DIRECTIONAL;
    direction = normalize(_dir);
    std::fill(m_viewport, m_viewport+4, 0);
}

Light::Light(glm::vec3 _pos, float _falloff): m_mvp_biased(1.0), m_mvp(1.0) {
    color = glm::vec3(1.0); 
    intensity = 1.0;
    falloff = -1.0;
    m_type = LIGHT_POINT;
    setPosition(_pos);
    falloff = _falloff;
    std::fill(m_viewport, m_viewport+4, 0);
}

Light::Light(glm::vec3 _pos, glm::vec3 _dir, float _falloff): m_mvp_biased(1.0), m_mvp(1.0) {
    color = glm::vec3(1.0); 
    intensity = 1.0;
    falloff = -1.0;
    m_type = LIGHT_SPOT;
    direction = _dir;
    setPosition(_pos);
    falloff = _falloff;
}

Light::~Light() {
}

glm::mat4 Light::getMVPMatrix( const glm::mat4 &_model, float _area) {
    // TODO:
    //      - Extend this to match different light types and not just directional

    if (bChange) {
        // From http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping
        _area *= 2.0;

        // Compute the MVP matrix from the light's point of view
        glm::mat4 depthProjectionMatrix = glm::ortho<float>(-_area, _area, -_area, _area, -_area * 2.0, _area * 2.0);
        glm::mat4 depthViewMatrix = glm::lookAt(glm::normalize(getPosition()), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
        m_mvp = depthProjectionMatrix * depthViewMatrix * _model;

        // Make biased MVP matrix (0 ~ 1) instad of (-1 to 1)
        glm::mat4 biasMatrix(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0
        );
        m_mvp_biased = biasMatrix * m_mvp;

        bChange = false;
    }

    return m_mvp;
}

glm::mat4 Light::getBiasMVPMatrix() {
    return m_mvp_biased;
}

void Light::bindShadowMap() {
    glGetIntegerv(GL_VIEWPORT, m_viewport);

    if (m_shadowMap.getDepthTextureId() == 0)
        m_shadowMap.allocate(1024, 1024, COLOR_DEPTH_TEXTURES);

    m_shadowMap.bind();
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
}

void Light::unbindShadowMap() {
    // glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    m_shadowMap.unbind();
    glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);
}

}
