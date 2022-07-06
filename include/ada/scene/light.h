#pragma once

#include "node.h"
#include "../gl/fbo.h"

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

    virtual void        setPosition(const glm::vec3& _pos);
    void                setType(LightType _type) { m_lightType = _type; }

    const LightType&    getLightType() const { return m_lightType; }
    glm::mat4           getMVPMatrix( const glm::mat4 &_model, float _area );
    glm::mat4           getBiasMVPMatrix();

    const Fbo*          getShadowMap() const { return &m_shadowMap; }
    float               getShadowMapNear() const { return m_near; }
    float               getShadowMapFar() const { return m_far; }

    void                bindShadowMap();
    void                unbindShadowMap();

    glm::vec3           color;
    glm::vec3           direction;
    float               intensity;
    float               falloff;

    bool                bUpdateShadowMap;

protected:
    virtual void        onPositionChanged() { bChange = true; };
    virtual void        onOrientationChanged() { bChange = true; };
    virtual void        onScaleChanged() { bChange = true; };

    Fbo                 m_shadowMap;
    glm::mat4           m_mvp_biased;
    glm::mat4           m_mvp;

    float               m_near;
    float               m_far;

    LightType           m_lightType;
    
    GLint               m_viewport[4];
};

}