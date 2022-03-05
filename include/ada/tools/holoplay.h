#pragma once

#include <functional>
#include <string>

#include "glm/glm.hpp"
#include "ada/gl/shader.h"

namespace ada {

// QUILT
//
struct QuiltProperties {
    QuiltProperties();
    QuiltProperties(int _width, int _height, int _cols, int _rows);

    int width       = 2048;
    int height      = 2048;
    int columns     = 4;
    int rows        = 8;
    int totalViews  = 32;
};

void setQuiltProperties(const QuiltProperties& _quilt);
void setQuiltProperties(size_t _index);

int  getQuiltWidth();
int  getQuiltHeight();
int  getQuiltColumns();
int  getQuiltRows();
int  getQuiltTotalViews();

void renderQuilt(std::function<void(const QuiltProperties&, glm::vec4&, int&)> _renderFnc );

// LENTICULAR Display
//

// This are hardcoded values for the Portrait HoloPlay by LGF.
//  in order to render correctly make sure this values match your calibration file on your device
// 
struct LenticularProperties {
    float dpi       = 324.0;
    float pitch     = 52.58737671470091;
    float slope     = -7.196136200157333;
    float center    = 0.4321881363063158;
    int   ri        = 0;
    int   bi        = 2;
};

void        setLenticularProperties(const LenticularProperties& _holoplay);
void        setLenticularProperties(const std::string& _path);
std::string getLenticularFragShader(size_t _versionNumber = 100);
void        feedLenticularUniforms(Shader& _shader);

}