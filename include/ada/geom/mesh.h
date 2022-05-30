#pragma once

#include <vector>
#include <string>

#include "triangle.h"

#include "glm/glm.hpp"

namespace ada {

#if defined(PLATFORM_RPI) || defined(__EMSCRIPTEN__)
#define INDEX_TYPE uint16_t
#else
#define INDEX_TYPE uint32_t
#endif

enum DrawMode {
    POINTS      = 0,
    LINES,      LINE_STRIP, LINE_LOOP,
    TRIANGLES,  TRIANGLE_STRIP, TRIANGLE_FAN
    // QUADS
};

class Mesh {
public:

    Mesh();
    Mesh(const Mesh &_mother);
    virtual ~Mesh();

    void    append(const Mesh &_mesh);
    void    clear();

    // DRAWMODE
    void                setDrawMode(DrawMode _drawMode = TRIANGLES) { m_drawMode = _drawMode; }
    DrawMode            getDrawMode() const { return m_drawMode; }

    // VERTICES
    void                addVertex(const glm::vec3 &_point);
    void                addVertices(const std::vector<glm::vec3> &_verts);
    void                addVertices(const glm::vec3* _verts, int _amt);

    bool                haveVertices() const { return !m_vertices.empty(); };
    void                clearVertices() { m_vertices.clear(); }
    const size_t        getVerticesTotal() const { return m_vertices.size(); }
    const glm::vec3&    getVertex(size_t _index) const { return m_vertices[_index]; }
    const std::vector<glm::vec3>& getVertices() const { return m_vertices; }


    // COLORS
    void                setColor(const glm::vec4 &_color);
    void                addColor(const glm::vec4 &_color);
    void                addColors(const std::vector<glm::vec4> &_colors);

    const bool          haveColors() const { return !m_colors.empty(); }
    void                clearColors() { m_colors.clear(); }
    size_t              getColorsTotal() const { return m_colors.size(); }
    const glm::vec4&    getColor(size_t _index) const { return m_colors[_index]; }
    const std::vector<glm::vec4>& getColors() const { return m_colors; }
    

    // NORMALS
    void                addNormal(const glm::vec3 &_normal);
    void                addNormals(const std::vector<glm::vec3> &_normals );

    const bool          haveNormals() const { return !m_normals.empty(); }
    const glm::vec3&    getNormal(size_t _index) const { return m_normals[_index]; }
    size_t              getNormalsTotal() const { return m_normals.size(); }
    const std::vector<glm::vec3>& getNormals() const { return m_normals; }
    void                clearNormals() { m_normals.clear(); }
    bool                computeNormals();
    void                smoothNormals(float _angle);
    void                invertNormals();
    void                flatNormals();


    // TANGENTS
    void                addTangent(const glm::vec4 &_tangent);

    const bool          haveTangents() const { return !m_tangents.empty(); }
    const glm::vec4&    getTangent(size_t _index) const { return m_tangents[_index]; }
    size_t              getTangentsTotal() const { return m_tangents.size(); }
    const std::vector<glm::vec4>& getTangents() const { return m_tangents; }
    void                clearTangets() { m_tangents.clear(); }
    bool                computeTangents();


    // TEXTURE COORDINATES
    void                addTexCoord(const glm::vec2 &_uv);
    void                addTexCoords(const std::vector<glm::vec2> &_uvs);

    const bool          haveTexCoords() const { return !m_texCoords.empty(); }
    size_t              getTexCoordsTotal() const { return m_texCoords.size(); }
    const glm::vec2&    getTexCoord(size_t _index) const { return m_texCoords[_index]; }
    const std::vector<glm::vec2>& getTexCoords() const { return m_texCoords; }
    void                clearTexCoords() { m_texCoords.clear(); }

    // INDICES
    void                addIndex(INDEX_TYPE _i);
    void                addIndices(const std::vector<INDEX_TYPE> &_inds);
    void                addIndices(const INDEX_TYPE* _inds, int _amt);

    const bool          haveIndices() const { return !m_indices.empty(); }
    INDEX_TYPE          getIndex(size_t _index) const { return m_indices[_index]; }
    size_t              getIndicesTotal() const { return m_indices.size(); }
    const std::vector<INDEX_TYPE>& getIndices() const { return m_indices; }
    void                clearIndices() { m_indices.clear(); }
    void                invertWindingOrder();

    // TRIANGLES
    void                addTriangle(const Triangle &_tri);
    void                addTriangles(const std::vector<Triangle> &_triangles);
    void                addTriangleIndices(INDEX_TYPE index1, INDEX_TYPE index2, INDEX_TYPE index3);
    std::vector<Triangle>   getTriangles() const;
    std::vector<glm::ivec3> getTrianglesIndices() const;

private:
    std::vector<glm::vec4>  m_colors;
    std::vector<glm::vec4>  m_tangents;
    std::vector<glm::vec3>  m_vertices;
    std::vector<glm::vec3>  m_normals;
    std::vector<glm::vec2>  m_texCoords;
    std::vector<INDEX_TYPE> m_indices;

    DrawMode                m_drawMode;

    friend void transform(Mesh&, const glm::quat&);
    friend void transform(Mesh&, const glm::mat3&);
    friend void transform(Mesh&, const glm::mat4&);

    friend void scale(Mesh&, float );
    friend void scaleX(Mesh&, float );
    friend void scaleY(Mesh&, float );
    friend void scaleZ(Mesh&, float );
    friend void scale(Mesh&, const glm::vec3& );
    friend void scale(Mesh&, float , float , float );

    friend void translateX(Mesh&, float );
    friend void translateY(Mesh&, float );
    friend void translateZ(Mesh&, float );
    friend void translate(Mesh&, const glm::vec3& );
    friend void translate(Mesh&, float , float , float );

    friend void rotateX(Mesh&, float );
    friend void rotateY(Mesh&, float );
    friend void rotateZ(Mesh&, float );
    friend void rotate(Mesh&, float, const glm::vec3& );
    friend void rotate(Mesh&, float , float, float, float );

    friend void center(Mesh&);
};

}