#include "ada/scene/material.h"
#include "ada/string.h"

namespace ada {

Material::Material():name("default") {
}

Material::~Material() {
}

void Material::feedProperties(Shader& _shader) const {
    _shader.addDefine( "MATERIAL_NAME_" + toUpper(name) );
    _shader.mergeDefines( m_defines );
}

}