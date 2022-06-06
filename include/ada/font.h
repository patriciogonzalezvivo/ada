#pragma once

#include <string>
#include <vector>
#include "glm/glm.hpp"

namespace ada {

enum FontHorizontalAlign {
    ALIGN_LEFT 	    = 1<<0,	// Default
    ALIGN_CENTER 	= 1<<1,
    ALIGN_RIGHT 	= 1<<2
};

enum FontVerticalAlign {
    ALIGN_TOP 	    = 1<<3,
    ALIGN_MIDDLE 	= 1<<4,
    ALIGN_BOTTOM 	= 1<<5,
    ALIGN_BASELINE  = 1<<6
};

enum FontEffect {
    EFFECT_NONE = 0,
    EFFECT_BLUR,
    EFFECT_GROW,
    EFFECT_DISTANCE_FIELD
};

class Font {
public:
    Font();
    Font(const std::string &_filepath, const std::string &_name = "");
    virtual ~Font();

    virtual bool load(const std::string &_filepath, std::string _name = "");
    virtual bool load(unsigned char* _data, size_t _size, std::string _name = "");
    virtual bool load(std::vector<unsigned char>& _ttf_data, std::string _name = "");
    virtual bool loadDefault();

    virtual void setSize(float _size) { m_size = _size; }

    virtual void setAlign(FontHorizontalAlign _align) { m_hAlign = _align; }
    virtual void setAlign(FontVerticalAlign _align) { m_vAlign = _align; }

    virtual void setEffect(FontEffect _effect) { m_effect = _effect; }
    virtual void setBlurAmount(float _blur) { m_effect = EFFECT_BLUR; m_blur = _blur; }

    virtual void setColor(float _brightness) { setColor(glm::vec4(glm::vec3(_brightness), 1.0f) ); }
    virtual void setColor(float _r, float _g, float _b) { setColor(glm::vec4(glm::vec3(_r, _g, _b), 1.0f) ); }
    virtual void setColor(float _r, float _g, float _b, float _a) { setColor(glm::vec4(_r, _g, _b, _a) ); }
    virtual void setColor(const glm::vec3 &_color) { setColor(glm::vec4(_color, 1.0f) ); }
    virtual void setColor(const glm::vec4 &_color);

    virtual void setAngle(float _angle) { m_angle = _angle; }

    // virtual GLint getAtlasTexture();
    virtual glm::vec4 getBoundingBox(const std::string &_text, float _x = 0, float _y = 0);
    virtual glm::vec4 getBoundingBox(const std::string &_text, const glm::vec2 &_pos) { return getBoundingBox(_text, _pos.x, _pos.y); }

    virtual void render(const std::string &_text, float _x, float _y);
    virtual void render(const std::string &_text, const glm::vec2 &_pos) { render(_text, _pos.x, _pos.y); }

private:

    FontHorizontalAlign m_hAlign;
    FontVerticalAlign   m_vAlign;

    FontEffect          m_effect;
    float               m_blur;

    float               m_angle;
    float               m_size;
    int                 m_color;
    int                 m_id;
};

}