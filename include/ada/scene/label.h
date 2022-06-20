#pragma once

#include <string>
#include <functional>

#include "ada/scene/camera.h"
#include "ada/scene/model.h"
#include "ada/window.h"
#include "ada/font.h"

namespace ada {

enum LabelType {
    LABEL_CENTER = 0,
    LABEL_UP, LABEL_DOWN, LABEL_LEFT, LABEL_RIGHT,
    LABEL_LINE_TO_WINDOW_BORDER
};

class Label : public BoundingBox {
public:
    Label();

    Label(const std::string& _text, glm::vec3* _position, LabelType _type = LABEL_CENTER, float _margin = 0.0f);
    Label(const std::string& _text, Node* _node, LabelType _type = LABEL_CENTER, float _margin = 0.0f);
    Label(const std::string& _text, Model* _model, LabelType _type = LABEL_CENTER, float _margin = 0.0f );
    
    Label(std::function<std::string(void)> _func, glm::vec3* _position, LabelType _type = LABEL_CENTER, float _margin = 0.0f);
    Label(std::function<std::string(void)> _func, Node* _node, LabelType _type = LABEL_CENTER, float _margin = 0.0f);
    Label(std::function<std::string(void)> _func, Model* _model, LabelType _type = LABEL_CENTER, float _margin = 0.0f );

    void setText(std::function<std::string(void)> _func) { m_textFunc = _func; }
    void setText(const std::string& _text) { m_text = _text; }

    void linkTo(glm::vec3* _position);
    void linkTo(Node* _position);
    void linkTo(Model* _position);

    void setType(LabelType _type) { m_type = _type; };
    void setMargin(float _margin) { m_margin = _margin * ada::getPixelDensity(); };

    virtual glm::vec3   getScreenPosition() const { return m_screenPos; }
    virtual float       getMargin() const { return m_margin; }
    virtual std::string getText();
    
    void update(Camera* _cam = nullptr, Font *_font = nullptr);
    void render(Font *_font = nullptr);

    bool            bVisible;

private:
    std::function<std::string(void)> m_textFunc;

    std::string     m_text;
    BoundingBox     m_screenBox;
    glm::vec3       m_screenPos;
    glm::vec2       m_line_points[3]; 
    LabelType       m_type;
    float           m_margin;

    BoundingBox*    m_bbox;
    glm::vec3*      m_worldPos;
};

}
