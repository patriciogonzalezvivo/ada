#pragma once

#include <vector>

#include "../gl/vbo.h"
#include "../gl/mesh.h"
#include "../gl/shader.h"
#include "node.h"

#include "material.h"

// #include "../uniforms.h"

namespace ada {

class Model : public Node {
public:
    Model();
    Model(const std::string& _name, Mesh& _mesh, const Material& _mat);
    virtual ~Model();

    bool        loadGeom(Mesh& _mesh);
    bool        loadShader(const std::string& _fragStr, const std::string& _vertStr, bool verbose);
    bool        loadMaterial(const Material& _material);

    bool        loaded() const { return m_model_vbo != nullptr; }
    void        clear();

    void        setName(const std::string& _str);
    std::string getName() { return m_name; }

    void        addDefine(const std::string& _define, const std::string& _value = "");
    void        delDefine(const std::string& _define);
    void        printDefines();
    void        printVboInfo();

    float       getArea() const { return m_area; }
    glm::vec3   getMinBoundingBox() const { return m_bbmin; }
    glm::vec3   getMaxBoundingBox() const { return m_bbmax; }

    Shader*     getShader() { return &m_shader; }
    
    void        render();
    void        render(Shader* _shader);
    void        renderBbox(Shader* _shader);

    Vbo*        getVbo() { return m_model_vbo; }
    Vbo*        getVboBbox() { return m_bbox_vbo; }

protected:
    Shader      m_shader;
    
    Vbo*        m_model_vbo;
    Vbo*        m_bbox_vbo;

    glm::vec3   m_bbmin;
    glm::vec3   m_bbmax;

    std::string m_name;
    float       m_area;
};

typedef std::vector<Model*>  Models;

}
