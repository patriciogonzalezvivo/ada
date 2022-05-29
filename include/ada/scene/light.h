#pragma once

#include "node.h"
#include "ada/gl/fbo.h"

namespace ada {

enum LightType {
    LIGHT_DIRECTIONAL, LIGHT_POINT, LIGHT_SPOT
};

class Light : public Node {
public:
    Light();
    Light(glm::vec3 _dir);
    Light(glm::vec3 _pos, float _falloff);
    Light(glm::vec3 _pos, glm::vec3 _dir, float _falloff = -1.0);
    virtual ~Light();

    const LightType&    getType() const { return m_type; }
    glm::mat4           getMVPMatrix( const glm::mat4 &_model, float _area );
    glm::mat4           getBiasMVPMatrix();

    const Fbo*          getShadowMap() const { return &m_shadowMap; }

    void                bindShadowMap();
    void                unbindShadowMap();

    glm::vec3           color;
    glm::vec3           direction;
    float               intensity;
    float               falloff;

protected:
    virtual void        onPositionChanged() { bChange = true; };
    virtual void        onOrientationChanged() { bChange = true; };
    virtual void        onScaleChanged() { bChange = true; };

    Fbo                 m_shadowMap;
    glm::mat4           m_mvp_biased;
    glm::mat4           m_mvp;

    LightType           m_type;
    
    GLint               m_viewport[4];
};

}