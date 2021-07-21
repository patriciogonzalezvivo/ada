#pragma once

#include "texture.h"

namespace ada {

class TextureStream : public Texture {
public:
    
    virtual int     getTotalFrames() { return 1; };
    virtual int     getCurrentFrame() { return 1; };

    virtual bool    update() { return false; };
};

}