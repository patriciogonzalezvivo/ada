#pragma once

#include "glm/glm.hpp"

#include "../gl/shader.h"
#include "../gl/defines.h"

namespace ada {

class Material : public HaveDefines {
public:
    Material();
    virtual ~Material();

    void feedProperties(Shader& _shader) const;
    
    std::string name;

};

}
