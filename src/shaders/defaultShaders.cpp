
#include "ada/shaders/defaultShaders.h"

#include "ada/shaders/default.h"
#include "ada/shaders/default_error.h"
#include "ada/shaders/default_scene.h"

#include "ada/shaders/billboard.h"
#include "ada/shaders/dynamic_billboard.h"

#include "ada/shaders/plot.h"
#include "ada/shaders/fxaa.h"
#include "ada/shaders/poissonfill.h"

// 3D SCENE
#include "ada/shaders/light_ui.h"
#include "ada/shaders/cubemap.h"
#include "ada/shaders/draw.h"

#include "ada/string.h"

namespace ada {

static size_t versionNumber = 100;
static std::string versionLine = "";

void setVersionFromCode(const std::string& _src) {
    versionLine = getVersion(_src, versionNumber);
}

int getVersion() {
    return versionNumber;
}

std::string getDefaultSrc( DefaultShaders _type ) {
    std::string rta = versionLine;

    if (_type == VERT_DEFAULT) {
        if (versionNumber < 130)
            rta += default_vert;
        else if (versionNumber >= 130) 
            rta += default_vert_300;
    }
    else if (_type == FRAG_DEFAULT) {
        rta += default_frag;
    }
    else if (_type == FRAG_DEFAULT_TEXTURE) {
        rta += default_texture_frag;
    }
    else if (_type == VERT_DEFAULT_SCENE) {
        if (versionNumber < 130)
            rta += default_scene_vert;
        else if (versionNumber >= 130) 
            rta += default_scene_vert_300;
    }
    else if (_type == FRAG_DEFAULT_SCENE) {
        rta += default_scene_frag0 + default_scene_frag1 + default_scene_frag2 + default_scene_frag3;
    }
    else if (_type == VERT_BILLBOARD) {
        if (versionNumber < 130)
            rta += billboard_vert;
        else if (versionNumber >= 130) 
            rta += billboard_vert_300;
    }
    else if (_type == VERT_DYNAMIC_BILLBOARD) {
        if (versionNumber < 130)
            rta += dynamic_billboard_vert;
        else if (versionNumber >= 130) 
            rta += dynamic_billboard_vert_300;
    }
    else if (_type == FRAG_DYNAMIC_BILLBOARD) {
        if (versionNumber < 130)
            rta += dynamic_billboard_frag;
        else if (versionNumber >= 130) 
            rta += dynamic_billboard_frag_300;
    }
    else if (_type == VERT_CUBEMAP) {
        if (versionNumber < 130)
            rta += cube_vert;
        else if (versionNumber >= 130) 
            rta += cube_vert_300;
    }
    else if (_type == FRAG_CUBEMAP) {
        if (versionNumber < 130)
            rta += cube_frag;
        else if (versionNumber >= 130) 
            rta += cube_frag_300;
    }
    else if (_type == VERT_ERROR) {
        if (versionNumber < 130)
            rta += error_vert;
        else if (versionNumber >= 130) 
            rta += error_vert_300;
    }
    else if (_type == FRAG_ERROR) {
        if (versionNumber < 130)
            rta += error_frag;
        else if (versionNumber >= 130) 
            rta += error_frag_300;
    }
    else if (_type == VERT_LIGHT) {
        if (versionNumber < 130)
            rta += light_vert;
        else if (versionNumber >= 130) 
            rta += light_vert_300;
    }
    else if (_type == FRAG_LIGHT) {
        if (versionNumber < 130)
            rta += light_frag;
        else if (versionNumber >= 130) 
            rta += light_frag_300;
    }

    else if (_type == VERT_FILL) {
        if (versionNumber < 130)
            rta += fill_vert;
        else if (versionNumber >= 130) 
            rta += fill_vert_300;
    }
    else if (_type == FRAG_FILL) {
        if (versionNumber < 130)
            rta += fill_frag;
        else if (versionNumber >= 130) 
            rta += fill_frag_300;
    }

    else if (_type == VERT_POINTS) {
        if (versionNumber < 130)
            rta += points_vert;
        else if (versionNumber >= 130) 
            rta += points_vert_300;
    }
    else if (_type == FRAG_POINTS) {
        if (versionNumber < 130)
            rta += points_frag;
        else if (versionNumber >= 130) 
            rta += points_frag_300;
    }

    else if (_type == FRAG_PLOT) {
        if (versionNumber < 130)
            rta += plot_frag;
        else if (versionNumber >= 130) 
            rta += plot_frag_300;
    }
    else if (_type == FRAG_FXAA) {
        if (versionNumber < 130)
            rta += fxaa_frag;
        else if (versionNumber >= 130) 
            rta += fxaa_frag_300;
    }
    else if (_type == FRAG_POISSON) {
        if (versionNumber < 130)
            rta += poissonfill_frag;
        else if (versionNumber >= 130) 
            rta += poissonfill_frag_300;
    } 

    return rta;
}

}