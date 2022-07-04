#pragma once

#include "node.h"
#include "material.h"

#include "../gl/vbo.h"
#include "../gl/shader.h"

#include "../geom/mesh.h"
#include "../geom/boundingBox.h"

namespace ada {

class Model : public Node {
public:
    Model();
    Model(const std::string& _name, const Mesh& _mesh, const Material& _mat);
    virtual ~Model();

    bool            loadGeom(const Mesh& _mesh);
    bool            loadShader(const std::string& _fragStr, const std::string& _vertStr, bool verbose);
    bool            loadMaterial(const Material& _material);

    bool            loaded() const { return m_model_vbo != nullptr; }
    void            clear();

    void            setName(const std::string& _str);
    const std::string& getName() const { return m_name; }

    void            addDefine(const std::string& _define, const std::string& _value = "");
    void            delDefine(const std::string& _define);
    void            printDefines();
    void            printVboInfo();

    float           getArea() const { return m_area; }
    const BoundingBox& getBoundingBox() const { return m_bbox; }

    Shader*         getShader() { return &m_shader; }
    
    void            render();
    void            render(Shader* _shader);
    void            renderBbox(Shader* _shader);

    Vbo*            getVbo() { return m_model_vbo; }
    Vbo*            getVboBbox() { return m_bbox_vbo; }

protected:
    Shader          m_shader;
    
    Vbo*            m_model_vbo;
    Vbo*            m_bbox_vbo;

    BoundingBox     m_bbox;

    std::string     m_name;
    float           m_area;

    friend class    Label;
};

}
