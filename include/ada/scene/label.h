#pragma once

#include <string>

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

    void setType(LabelType _type) { m_type = _type; };
    void setText(const std::string& _text);
    void linkTo(glm::vec3* _position);
    void linkTo(Node* _position);
    void linkTo(Model* _position);
    void update(Camera* _cam = nullptr, Font *_font = nullptr);

    void render(Font *_font = nullptr);

private:
    std::string     m_text;
    BoundingBox     m_screenBox;
    glm::vec3       m_screenPos;
    LabelType       m_type;
    BoundingBox*    m_bbox;
    glm::vec3*      m_worldPos;

};

}