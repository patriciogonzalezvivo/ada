#include "ada/scene/label.h"
#include "ada/window.h"
#include "ada/draw.h"

#include <iostream>

namespace ada {

Label::Label() : m_text(""), m_worldPos(nullptr) { 

}

Label::Label(const std::string& _text, glm::vec3* _position) { 
    m_text = _text; 
    m_worldPos = _position;
}

void Label::setText(const std::string& _text) { m_text = _text; }
void Label::linkTo(glm::vec3* _position) { m_worldPos = _position; }

void Label::update(Camera* _cam, Font *_font) { 
    // _cam->bChange
    if (_cam == nullptr)
        _cam = getCamera();

    m_screenPos = _cam->worldToScreen(m_worldPos);
    m_screenPos.x *= ada::getWindowWidth();
    m_screenPos.y *= ada::getWindowHeight();

    if (_font == nullptr)
        _font = getFont();

    set( _font->getBoundingBox( m_text, m_screenPos.x, m_screenPos.y) );
    max.z = min.z = m_screenPos.z;
}

void Label::render(Font *_font) {
    if (_font == nullptr)
        _font = getFont();

    if (m_worldPos != nullptr)
        _font->render(  m_text, m_screenPos.x, m_screenPos.y );

}


};