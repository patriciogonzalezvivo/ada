#include "ada/scene/label.h"
#include "ada/window.h"
#include "ada/draw.h"

#include <iostream>

namespace ada {

Label::Label() : m_text(""), m_type(LABEL_CENTER), m_bbox(nullptr), m_worldPos(nullptr) { 

}

Label::Label(const std::string& _text, glm::vec3* _position, LabelType _type) : m_bbox(nullptr){ 
    setType(_type);
    setText(_text);
    linkTo(_position);
}

Label::Label(const std::string& _text, Node* _node, LabelType _type) : m_bbox(nullptr) { 
    setType(_type);
    setText(_text);
    linkTo(_node);
}

Label::Label(const std::string& _text, Model* _model, LabelType _type) { 
    setType(_type);
    setText(_text);
    linkTo(_model);
}

void Label::setText(const std::string& _text) { m_text = _text; }

void Label::linkTo(glm::vec3* _position) { m_worldPos = _position; }
void Label::linkTo(Node* _node) { m_worldPos = &(_node->m_position); }
void Label::linkTo(Model* _model) { 
    m_worldPos = &(_model->m_position);
    m_bbox = &(_model->m_bbox);
}

void Label::update(Camera* _cam, Font *_font) { 
    // _cam->bChange
    if (_cam == nullptr)
        _cam = getCamera();

    if (m_bbox) {
        m_screenBox = _cam->worldToScreen(*m_bbox, getWorldMatrixPtr());
        m_screenBox.min.x *= ada::getWindowWidth(); 
        m_screenBox.max.x *= ada::getWindowWidth();
        m_screenBox.min.y *= ada::getWindowHeight(); 
        m_screenBox.max.y *= ada::getWindowHeight(); 
        m_screenPos = m_screenBox.getCenter();
        // m_screenPos.x *= ada::getWindowWidth();
        // m_screenPos.y *= ada::getWindowHeight();
        if (m_type == LABEL_TOP)
            m_screenPos.y -= m_screenBox.getHeight() * 0.5;
        else if (m_type == LABEL_DOWN)
            m_screenPos.y += m_screenBox.getHeight() * 0.5;
        else if (m_type == LABEL_LEFT)
            m_screenPos.y -= m_screenBox.getWidth() * 0.5;
        else if (m_type == LABEL_RIGHT)
            m_screenPos.x += m_screenBox.getWidth() * 0.5;
    }
    else {
        m_screenPos = _cam->worldToScreen(m_worldPos, getWorldMatrixPtr());
        m_screenPos.x *= ada::getWindowWidth();
        m_screenPos.y *= ada::getWindowHeight();
    }

    if (_font == nullptr)
        _font = getFont();

    set( _font->getBoundingBox( m_text, m_screenPos.x, m_screenPos.y) );
    max.z = min.z = m_screenPos.z;
}

void Label::render(Font *_font) {
    if (_font == nullptr)
        _font = getFont();

    if (m_type == LABEL_CENTER) {
        _font->setAlign(ada::ALIGN_CENTER);
        _font->setAlign(ada::ALIGN_MIDDLE);
    }
    else if (m_type == LABEL_TOP) {
        _font->setAlign(ada::ALIGN_CENTER);
        _font->setAlign(ada::ALIGN_BOTTOM);
    }
    else {
        _font->setAlign(ada::ALIGN_CENTER);
        _font->setAlign(ada::ALIGN_TOP);
    }

    if (m_bbox) {
        if (m_type == LABEL_LEFT)
            _font->setAngle(M_PI_2);
        else if (m_type == LABEL_RIGHT)
            _font->setAngle(-M_PI_2);
    }

    if (m_worldPos != nullptr)
        _font->render( m_text, m_screenPos.x, m_screenPos.y );
}


};