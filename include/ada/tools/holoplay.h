#pragma once

#include <functional>
#include <string>

#include "glm/glm.hpp"
#include "ada/gl/shader.h"
#include "ada/scene/camera.h"

namespace ada {

// HOLO PLAY
// 
struct HoloplayProperties {

    // This values will change based on the resolution
    int width       = 2048;
    int height      = 2048;
    int columns     = 4;
    int rows        = 8;
    int totalViews  = 32;

    // This are hardcoded values for the Portrait HoloPlay by LGF.
    //  in order to render correctly make sure this values match your calibration file on your device
    // 
    float dpi       = 324.0;
    float pitch     = 52.58737671470091;
    float slope     = -7.196136200157333;
    float center    = 0.4321881363063158;
    int   ri        = 0;
    int   bi        = 2;
};

void        setHoloplayProperties(const HoloplayProperties& _holoplay);
void        setHoloplayResolution(int _holoplay);
int         getHoloplayWidth();
int         getHoloplayHeight();
std::string getHoloplayFragShader(size_t _versionNumber = 100);

void        holoplayQuilt(std::function<void(const HoloplayProperties&, glm::vec4&, int&)> _renderFnc );
void        holoplayFeedUniforms(Shader& _shader);

}