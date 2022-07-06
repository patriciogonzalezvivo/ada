#pragma once

#include "ada/scene.h"

namespace ada {

bool loadSTL(const std::string& _filename, ada::Scene& _scene, bool _verbose);

}