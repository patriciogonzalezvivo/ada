#pragma once

#include <string>

#include "ada/geom/boundingBox.h"
#include "ada/scene/camera.h"
#include "ada/font.h"

namespace ada {

class Label : public BoundingBox {
public:
    Label();
    Label(const std::string& _text, glm::vec3* _position);

    void setText(const std::string& _text);
    void linkTo(glm::vec3* _position);
    void update(Camera* _cam);

    void render(Font *_font);

private:
    std::string m_text;
    glm::vec3*  m_worldPos;
    glm::vec3   m_screenPos;

};

}