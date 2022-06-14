#pragma once

#include <string>
#include <functional>

#include "ada/scene/camera.h"
#include "ada/scene/model.h"
#include "ada/font.h"

namespace ada {

enum LabelType {
    LABEL_CENTER = 0,
    LABEL_DOWN, LABEL_TOP, LABEL_LEFT, LABEL_RIGHT
};

class Label : public BoundingBox {
public:
    Label();

    Label(const std::string& _text, glm::vec3* _position, LabelType _type = LABEL_CENTER);
    Label(const std::string& _text, Node* _position, LabelType _type = LABEL_CENTER);
    Label(const std::string& _text, Model* _position, LabelType _type = LABEL_CENTER );
    
    Label(std::function<std::string(Label*)> _func, glm::vec3* _position, LabelType _type = LABEL_CENTER);
    Label(std::function<std::string(Label*)> _func, Node* _position, LabelType _type = LABEL_CENTER);
    Label(std::function<std::string(Label*)> _func, Model* _position, LabelType _type = LABEL_CENTER );

    void setType(LabelType _type) { m_type = _type; };
    
    void setText(std::function<std::string(Label*)> _func) { m_func = _func; }
    void setText(const std::string& _text) { m_text = _text; }

    void linkTo(glm::vec3* _position);
    void linkTo(Node* _position);
    void linkTo(Model* _position);

    virtual glm::vec3   getScreenPosition() const { return m_screenPos; }
    
    void update(Camera* _cam = nullptr, Font *_font = nullptr);
    void render(Font *_font = nullptr);

    bool            bVisible;

private:
    std::function<std::string(Label*)> m_func;    

    std::string     m_text;
    BoundingBox     m_screenBox;
    glm::vec3       m_screenPos;
    LabelType       m_type;
    BoundingBox*    m_bbox;
    glm::vec3*      m_worldPos;

};

}
