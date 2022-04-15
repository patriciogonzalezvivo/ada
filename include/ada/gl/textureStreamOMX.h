#pragma once

#include "ada/gl/textureStream.h"

#if defined(DRIVER_BROADCOM) && defined(SUPPORT_OMAX)

#include <thread>

namespace ada {

class TextureStreamOMX: public TextureStream {
public:
    TextureStreamOMX();
    virtual ~TextureStreamOMX();

    virtual bool    load(const std::string& _filepath, bool _vFlip, TextureFilter _filter = LINEAR, TextureWrap _wrap = REPEAT);

    virtual float   getFPS() const { return m_fps; };
    virtual float   getDuration() const { return m_duration; };
    virtual float   getTotalFrames() const { return m_totalFrames; };

    virtual bool    update() { return true; }
    virtual void    clear();

protected:
    static void* decode_video(const char* filename, void* _streamTexture);

    void*       m_eglImage;
    double      m_fps;
    double      m_duration;
    long        m_totalFrames;

    std::thread m_thread;

};

}

#endif