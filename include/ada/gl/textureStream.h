#pragma once

#include "texture.h"

namespace ada {

class TextureStream : public Texture {
public:

    virtual void            setSpeed( float _speed ) {};
    
    virtual const GLuint    getPrevTextureId() const { return 0; };
    
    virtual float           getDuration() { return 0.0; }
    virtual float           getTime() const { return 0.0; }

    virtual float           getTotalFrames() { return 0; };
    virtual float           getCurrentFrame() const { return 0; };

    virtual void            restart() {};

    virtual bool            update() { return false; };

protected:

};

}