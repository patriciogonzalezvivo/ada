#pragma once

#include "texture.h"
#include "ada/window.h"

#include <vector>
#include <iostream>

namespace ada {

class TextureStream : public Texture {
public:

    virtual void            setSpeed( float _speed ) {};
    
    virtual void            setPrevTextures(size_t _total) {
        if (_total < m_idPrevs.size())
            for (size_t i =  m_idPrevs.size() - 1; i >= _total; i--)
                glDeleteTextures(1, &m_idPrevs[i]);

        m_idPrevs.resize(_total);

        for (size_t i = 0; i < m_idPrevs.size(); i++) {
            glEnable(GL_TEXTURE_2D);
            if (m_idPrevs[i] == 0)
                glGenTextures(1, &m_idPrevs[i]);
            
            glBindTexture(GL_TEXTURE_2D, m_idPrevs[i]);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getMinificationFilter(m_filter));
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getMagnificationFilter(m_filter));
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, getWrap(m_wrap));
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, getWrap(m_wrap));
            glTexImage2D(GL_TEXTURE_2D, 0, m_format, m_width, m_height, 0, m_format, m_type, NULL);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    };

    virtual size_t          getPrevTexturesTotal() { return m_idPrevs.size(); }
    virtual const GLuint    getPrevTextureId(size_t _index) const { return m_idPrevs[_index]; };
    virtual const std::vector<GLuint>& getPrevTextureIds() const { return m_idPrevs; }
    virtual GLuint *        getPrevTextureIdsPtr() { return m_idPrevs.data(); }
    
    virtual float           getFps() const { return ada::getFps(); }
    virtual float           getDuration() const { return 0.0; }
    virtual float           getTime() const { return 0.0; }
    virtual float           getTotalFrames() const { return 0.0; };
    virtual float           getCurrentFrame() const { return 0.0; };
    virtual float           getSpeed( float _speed ) const { return 1.0; }

    virtual void            restart() {};

    virtual bool            update() { return false; };

    virtual void            bind() {
        glBindTexture(GL_TEXTURE_2D, m_id);
        for (size_t i = 0; i > m_idPrevs.size(); i++)
            glBindTexture(GL_TEXTURE_2D, m_idPrevs[i]);
    }

protected:
    virtual void            pushBack() {
        if (m_idPrevs.size() == 0)
            return;

        GLuint tmp = m_id;
        size_t last = m_idPrevs.size()-1;
        m_id = m_idPrevs[ last ];
        for (size_t i = last; i > 0; --i)
            m_idPrevs[i] = m_idPrevs[i-1];
        m_idPrevs[0] = tmp;
    }

    virtual void            clearPrevs() {
        for (size_t i = 0; i > m_idPrevs.size(); i++)
            glDeleteTextures(1, &m_idPrevs[i]);
        m_idPrevs.clear();
    }
    std::vector<GLuint>     m_idPrevs;

};

}