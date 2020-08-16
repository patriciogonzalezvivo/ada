
#pragma once

#include "texture.h"
#include "glm/glm.hpp"

namespace ada {

struct SkyBox {
    SkyBox() {
        groundAlbedo = glm::vec3(0.25);
        azimuth = 0.0f;
        elevation = 0.3;
        turbidity = 4.0f;
    }

    glm::vec3   groundAlbedo;
    float       elevation;
    float       azimuth;
    float       turbidity;
    bool        change;
};

class TextureCube : public Texture {
public:
    TextureCube();
    virtual ~TextureCube();

    virtual bool    load(const std::string &_fileName, bool _vFlip = true);
    virtual bool    generate(SkyBox* _skybox, int _width = 512 );

    virtual void    bind();

    glm::vec3       SH[9];
};

}