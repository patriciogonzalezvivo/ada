#pragma once

#include <string>

#include "gl.h"
#include "textureProps.h"

namespace ada {

enum TextureType {
    JUST_TEXTURE = 0, BUMP_TEXTURE, 
    CUBE_TEXTURE, 
    STREAM_SEQUENCE_TEXTURE, STREAM_VIDEO_TEXTURE, 
    STREAM_AUDIO_TEXTURE, 
};

class Texture {
public:
    Texture();
    virtual ~Texture();

    virtual bool    load(const std::string& _filepath, bool _vFlip = false, TextureFilter _filter = LINEAR, TextureWrap _wrap = REPEAT);
    virtual bool    load(int _width, int _height, int _component, int _bits, const void* _data, TextureFilter _filter = LINEAR, TextureWrap _wrap = REPEAT);
    virtual bool    update(int _x, int _y, int _width, int _height, const void* _data);

    virtual void    clear();

    virtual bool            isLoaded() const { return m_id != 0; };
    virtual const GLuint    getTextureId() const { return m_id; };
    virtual std::string     getFilePath() const { return m_path; };
    virtual int             getWidth() const { return m_width; };
    virtual int             getHeight() const { return m_height; };

    /* Bind/Unbind the texture to GPU */
    virtual void    bind();
    virtual void    unbind();

protected:
    std::string     m_path;

    int             m_width;
    int             m_height;
    GLenum          m_format;
    GLenum          m_type;
    TextureFilter   m_filter; 
    TextureWrap     m_wrap;
    bool	        m_vFlip;

    GLuint          m_id;
};

}