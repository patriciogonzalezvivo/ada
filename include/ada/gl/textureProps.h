#pragma once

#include <string>
#include "gl.h"

namespace ada {

enum TextureFilter {
    LINEAR = 0,     // Bilinear, no mipmap.
    NEAREST        // Nearest neighbour, no mipmap.
    // NEAREST_NEAREST,// Nearest neighbour, closest mipmap.`
    // LINEAR_NEAREST, // Bilinear, closest mipmap.
    // NEAREST_LINEAR, // Nearest neighbour, linear blend of mipmaps.
    // LINEAR_LINEAR   // Bilinear, linear blend of mipmaps.
};

enum TextureWrap {
    REPEAT = 0,     // Repeat the texture.
    CLAMP          // Clamp to the edges of the texture.
    // MIRROR
};

GLenum  getFilter( TextureFilter _filter );
GLenum  getMagnificationFilter( TextureFilter _filter );
GLenum  getMinificationFilter( TextureFilter _filter );
GLenum  getWrap( TextureWrap _wrap );

#ifndef __EMSCRIPTEN__
bool    screenshot(const std::string& _filename );
bool    screenshot( const std::string& _filename, int _width, int _height); 
#endif

}