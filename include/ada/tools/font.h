#pragma once

#include <string>
#include "glm/glm.hpp"

namespace ada {

enum FontAlign {
    ALIGN_LEFT 	    = 1<<0,	// Default
    ALIGN_CENTER 	= 1<<1,
    ALIGN_RIGHT 	= 1<<2
};

class Font {
public:
    Font();
    Font(const std::string &_filepath, const std::string &_name = "");
    virtual ~Font();

    virtual bool load(const std::string &_filepath, std::string _name = "");

    virtual void setSize(float _size) { m_size = _size; }

    virtual void setAlign(FontAlign _align) { m_align = _align; }

    virtual void setColor(float _brightness) { setColor(glm::vec4(glm::vec3(_brightness), 1.0f) ); }
    virtual void setColor(float _r, float _g, float _b) { setColor(glm::vec4(glm::vec3(_r, _g, _b), 1.0f) ); }
    virtual void setColor(float _r, float _g, float _b, float _a) { setColor(glm::vec4(_r, _g, _b, _a) ); }
    virtual void setColor(const glm::vec3 &_color) { setColor(glm::vec4(_color, 1.0f) ); }
    virtual void setColor(const glm::vec4 &_color);

    // virtual GLint getAtlasTexture();
    virtual glm::vec4   getBoundingBox(const std::string &_text, float _x = 0, float _y = 0);

    virtual void render(const std::string &_text, float _x, float _y);
    virtual void render(const std::string &_text, const glm::vec2 &_pos) { render(_text, _pos.x, _pos.y); }

private:

    FontAlign   m_align;
    float       m_size;
    int         m_color;
    int         m_id;
};

}