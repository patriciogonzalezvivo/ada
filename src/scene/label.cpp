#include "ada/scene/label.h"
#include "ada/window.h"

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

void Label::update(Camera* _cam) { 
    // _cam->bChange
    if (_cam != nullptr)
        m_screenPos = _cam->worldToScreen(m_worldPos); 

    std::cout << m_screenPos.x << "x" << m_screenPos.y << std::endl;
}

void Label::render(Font *_font) {
    if (_font != nullptr && m_worldPos != nullptr)
        _font->render(  m_text, 
                        m_screenPos.x * ada::getWindowWidth(), 
                        m_screenPos.y * ada::getWindowHeight());
}


};