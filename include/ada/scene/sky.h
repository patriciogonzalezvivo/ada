#pragma once

#include <string>
#include <functional>

#include "glm/glm.hpp"
#include "camera.h"

namespace ada {

// ENVIROMENT CUBEMAPS
// -------------------------------------------------------------- 

///
///              +----------+
///              | +---->+x |
///              | |        |
///              | |  +y    |
///              |+z      2 |
///   +----------+----------+----------+----------+
///   | +---->+z | +---->+x | +---->-z | +---->-x |
///   | |        | |        | |        | |        |
///   | |  -x    | |  +z    | |  +x    | |  -z    |
///   |-y      1 |-y      4 |-y      0 |-y      5 |
///   +----------+----------+----------+----------+
///              | +---->+x |
///              | |        |
///              | |  -y    |
///              |-z      3 |
///              +----------+
///

template <typename T> 
struct CubemapFace {

    void    flipHorizontal();
    void    flipVertical();
    int     calculateSH(glm::vec3 *_sh);

    void    upload();

    int     id;
	int     width;
	int     height;

	// for mem copy purposes only
	int     currentOffset;
    T       *data;
};

glm::vec3   getFaceDirection(size_t _id);

template <typename T> 
void        splitFacesFromVerticalCross(T *_data, int _width, int _height, CubemapFace<T> **_faces );

template <typename T> 
void        splitFacesFromHorizontalCross(T *_data, int _width, int _height, CubemapFace<T> **_faces );

template <typename T> 
void        splitFacesFromHorizontalRow(T *_data, int _width, int _height, CubemapFace<T> **_faces );

template <typename T> 
void        splitFacesFromVerticalRow(T *_data, int _width, int _height, CubemapFace<T> **_faces );

template <typename T> 
void        splitFacesFromEquirectangular(T *_data, unsigned int _width, unsigned int _height, CubemapFace<T> **_faces );


//  DYNAMIC ENVIROMENT CUBEMAP
// -------------------------------------------------------------- 
void        dynamicCubemap(std::function<void(Camera&, glm::vec4&, int&)> _renderFnc, glm::vec3 _pos = glm::vec3(0.0), int _viewSize = 512);
void        dynamicCubemap(std::function<void(Camera&, glm::vec4&, int&)> _renderFnc, const std::string& _file, glm::vec3 _pos = glm::vec3(0.0), int _viewSize = 512);


// SKY BOX GENERATOR
// -------------------------------------------------------------- 
struct SkyData {
    glm::vec3   groundAlbedo    = glm::vec3(0.25f);
    float       elevation       = 0.3f;
    float       azimuth         = 0.0f;
    float       turbidity       = 4.0f;
    bool        change          = false;
};

float*                  skyEquirectangular(SkyData* _sky, int _width, int _height);
CubemapFace<float>**    skyCubemap(SkyData* _sky, int _width);

}