#pragma once

#include <string>
#include <functional>

#include "ada/scene/camera.h"
#include "ada/scene/model.h"
#include "ada/font.h"

namespace ada {

enum LabelType {
    LABEL_CENTER = 0,
    LABEL_UP, LABEL_DOWN, LABEL_LEFT, LABEL_RIGHT,
    LABEL_TOP_BUTTOM
};

class Label : public BoundingBox {
public:
    Label();

    Label(const std::string& _text, glm::vec3* _position, LabelType _type = LABEL_CENTER);
    Label(const std::string& _text, Node* _node, LabelType _type = LABEL_CENTER);
    Label(const std::string& _text, Model* _model, LabelType _type = LABEL_CENTER );
    
    Label(std::function<std::string(void)> _func, glm::vec3* _position, LabelType _type = LABEL_CENTER);
    Label(std::function<std::string(void)> _func, Node* _node, LabelType _type = LABEL_CENTER);
    Label(std::function<std::string(void)> _func, Model* _model, LabelType _type = LABEL_CENTER );

    Label(std::function<glm::vec4(Label*)> _func, glm::vec3* _position);
    Label(std::function<glm::vec4(Label*)> _func, Node* _node);

    void setType(LabelType _type) { m_type = _type; };

    void setText(std::function<std::string(void)> _func) { m_textFunc = _func; }
    void setText(const std::string& _text) { m_text = _text; }

    void setRender(std::function<glm::vec4(Label*)> _func) { m_renderFunc = _func; }

    void linkTo(glm::vec3* _position);
    void linkTo(Node* _position);
    void linkTo(Model* _position);

    virtual glm::vec3   getScreenPosition() const { return m_screenPos; }
    virtual std::string getText();
    
    void update(Camera* _cam = nullptr, Font *_font = nullptr);
    void render(Font *_font = nullptr);

    bool            bVisible;

private:
    std::function<std::string(void)> m_textFunc;
    std::function<glm::vec4(Label*)> m_renderFunc;

    std::string     m_text;
    BoundingBox     m_screenBox;
    glm::vec3       m_screenPos;
    LabelType       m_type;
    BoundingBox*    m_bbox;
    glm::vec3*      m_worldPos;

};

}
