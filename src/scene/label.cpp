#include "ada/scene/label.h"
#include "ada/window.h"
#include "ada/draw.h"
#include "ada/math.h"

#include <iostream>

#define SCREEN_MARGIN 25.0
#define TEXT_MARGIN 10.0

namespace ada {

Label::Label() : m_text(""), m_type(LABEL_CENTER), m_bbox(nullptr), m_worldPos(nullptr) { 

}

Label::Label(const std::string& _text, glm::vec3* _position, LabelType _type, float _margin) : m_bbox(nullptr){ 
    setText(_text);
    linkTo(_position);
    setType(_type);
    setMargin(_margin);
}

Label::Label(const std::string& _text, Node* _node, LabelType _type, float _margin) : m_bbox(nullptr) { 
    setText(_text);
    linkTo(_node);
    setType(_type);
    setMargin(_margin);
}

Label::Label(const std::string& _text, Model* _model, LabelType _type, float _margin) { 
    setText(_text);
    linkTo(_model);
    setType(_type);
    setMargin(_margin);
}

Label::Label(std::function<std::string(void)> _func, glm::vec3* _position, LabelType _type, float _margin) : m_bbox(nullptr){ 
    setText(_func);
    linkTo(_position);
    setType(_type);
    setMargin(_margin);
}

Label::Label(std::function<std::string(void)> _func, Node* _node, LabelType _type, float _margin) : m_bbox(nullptr) { 
    setText(_func);
    linkTo(_node);
    setType(_type);
    setMargin(_margin);
}

Label::Label(std::function<std::string(void)> _func, Model* _model, LabelType _type, float _margin) { 
    setText(_func);
    linkTo(_model);
    setType(_type);
    setMargin(_margin);
}

void Label::linkTo(glm::vec3* _position) { m_worldPos = _position; }
void Label::linkTo(Node* _node) { m_worldPos = &(_node->m_position); }
void Label::linkTo(Model* _model) { 
    m_worldPos = &(_model->m_position);
    m_bbox = &(_model->m_bbox);
}

std::string Label::getText() {
    if (m_textFunc)
        return m_textFunc();
    return m_text;
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
    }
    else {
        m_screenPos = _cam->worldToScreen(m_worldPos, getWorldMatrixPtr());
        m_screenPos.x *= ada::getWindowWidth();
        m_screenPos.y *= ada::getWindowHeight();
        m_screenBox.set(m_screenPos);
    }

    m_screenBox.expand(m_margin);

    // Is in view? (depth and in viewport)
    if (m_screenPos.z >= 1.0  ||
        m_screenBox.max.x < 0 || m_screenBox.min.x > ada::getWindowWidth() ||
        m_screenBox.max.y < 0 || m_screenBox.min.y > ada::getWindowHeight() ) {
        bVisible = false;
        return;
    }
    else {
        bVisible = true;
    }

    if (m_type == LABEL_UP)
        m_screenPos.y -= m_screenBox.getHeight() * 0.5;
    else if (m_type == LABEL_DOWN)
        m_screenPos.y += m_screenBox.getHeight() * 0.5;
    else if (m_type == LABEL_LEFT)
        m_screenPos.x -= m_screenBox.getWidth() * 0.5;
    else if (m_type == LABEL_RIGHT)
        m_screenPos.x += m_screenBox.getWidth() * 0.5;

    if (m_type == LABEL_LINE_TO_WINDOW_BORDER) {
        _font->setAlign(ada::ALIGN_MIDDLE);
        _font->setAlign(ada::ALIGN_LEFT);
        m_line_points[0] = m_screenPos;
        m_line_points[1] = m_screenPos;
        float w = getWidth() * ada::getPixelDensity();
        if (m_screenPos.x < ada::getWindowWidth() * 0.5) {
            m_line_points[0].x -= m_screenBox.getWidth();
            m_line_points[1].x = w + TEXT_MARGIN;
            m_screenPos.x = 0.0;
        }
        else {
            m_line_points[0].x += m_screenBox.getWidth();
            m_screenPos.x = ada::getWindowWidth() - w;
            m_line_points[1].x = m_screenPos.x - TEXT_MARGIN;
        }
    }

    max.z = min.z = m_screenPos.z;

    if (m_textFunc)
        m_text = m_textFunc();

    if (m_text.size() > 0) {
        if (_font == nullptr)
            _font = getFont();

        set( _font->getBoundingBox( m_text, m_screenPos.x, m_screenPos.y) );
    }
}

void Label::render(Font *_font) {
    if (!bVisible)
        return;

    if (_font == nullptr)
        _font = getFont();

    if (m_type == LABEL_LINE_TO_WINDOW_BORDER) {
        _font->setAlign(ada::ALIGN_MIDDLE);
        _font->setAlign(ada::ALIGN_LEFT);
        _font->setAngle(0.0f);
        line(m_line_points[0], m_line_points[1]);
    }
    else {
        if (m_type == LABEL_CENTER) {
            _font->setAlign(ada::ALIGN_CENTER);
            _font->setAlign(ada::ALIGN_MIDDLE);
        }
        else if (m_type == LABEL_UP) {
            _font->setAlign(ada::ALIGN_CENTER);
            _font->setAlign(ada::ALIGN_BOTTOM);
        }
        else {
            _font->setAlign(ada::ALIGN_CENTER);
            _font->setAlign(ada::ALIGN_TOP);
        }

        if (m_bbox) {
            if (m_type == LABEL_LEFT)
                _font->setAngle(HALF_PI);
            else if (m_type == LABEL_RIGHT)
                _font->setAngle(-HALF_PI);
        }
        else 
            _font->setAngle(0.0f);
    }

    if (m_worldPos != nullptr)
        _font->render( m_text, m_screenPos.x, m_screenPos.y );
}


};
