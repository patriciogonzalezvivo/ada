#pragma once

#include "ada/scene.h"

namespace ada {

bool loadGLTF( const std::string& _filename, ada::Scene& _scene, bool _verbose);

}