#pragma once

#include <vector>
#include "shader.h"

namespace ada {

void render(const std::vector<glm::vec3>& _polyline, Shader* _program);

};