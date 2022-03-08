#pragma once

#include "texture.h"

namespace ada {

class TextureStream : public Texture {
public:

    virtual void    setSpeed( float _speed ) {};
    
    virtual float   getTotalSeconds() { return 0.0; }
    virtual float   getCurrentSecond() const { return 1.0; }

    virtual int     getTotalFrames() { return 0; };
    virtual int     getCurrentFrame() const { return 1; };

    virtual void    restart() {};

    virtual bool    update() { return false; };
};

}