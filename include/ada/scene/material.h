#pragma once

#include <map>
#include <string>
#include "glm/glm.hpp"

#include "ada/fs.h"
#include "ada/gl/shader.h"

namespace ada {

class Material : public HaveDefines {
public:
    Material();
    virtual ~Material();

    void feedProperties(Shader& _shader) const;
    
    std::string name;

};

typedef std::map<std::string,Material> Materials;

}
