#pragma once

#include "textureStream.h"
#include <vector>

namespace ada {

class TextureStreamSequence : public TextureStream {
public:
    TextureStreamSequence();
    virtual ~TextureStreamSequence();

    // virtual void            setSpeed( float _speed ) {};
    virtual const GLuint    getPrevTextureId() const { return m_idPrev; };

    virtual float   getDuration();
    virtual float   getTime() const;

    virtual float   getTotalFrames() { return m_frames.size(); };
    virtual float   getCurrentFrame() const { return m_currentFrame; };

    virtual bool    load(const std::string& _filepath, bool _vFlip);
    virtual bool    update();
    virtual void    restart();
    virtual void    clear();

private:

    std::vector<void*>  m_frames;
    size_t              m_currentFrame;
    size_t              m_bits;
    GLuint              m_idPrev;

};

}