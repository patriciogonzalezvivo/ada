#pragma once

#include <string>

namespace ada
{

enum DefaultShaders {
    VERT_DEFAULT = 0, FRAG_DEFAULT, FRAG_DEFAULT_TEXTURE,
    VERT_DEFAULT_SCENE, FRAG_DEFAULT_SCENE,
    VERT_BILLBOARD, VERT_DYNAMIC_BILLBOARD, FRAG_DYNAMIC_BILLBOARD,
    VERT_CUBEMAP, FRAG_CUBEMAP,
    VERT_ERROR, FRAG_ERROR,
    VERT_LIGHT, FRAG_LIGHT,
    VERT_FILL, FRAG_FILL,
    VERT_POINTS, FRAG_POINTS,
    FRAG_PLOT, FRAG_FXAA, FRAG_POISSON
};

void    setVersionFromCode(const std::string& _src);
int     getVersion();

std::string getDefaultSrc(DefaultShaders _type);
    
} // namespace ada
