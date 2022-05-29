
#include <iostream>

#include "ada/gl/vertexLayout.h"

#include "ada/geom/mesh.h"
#include "ada/string.h"

namespace ada {

Mesh::Mesh():m_drawMode(TRIANGLES) {
}

Mesh::Mesh(const Mesh &_mother) : m_drawMode(_mother.getDrawMode()) {
    append(_mother);
}

Mesh::~Mesh() {
}

void Mesh::setColor(const glm::vec4 &_color) {
    m_colors.clear();
    for (uint32_t i = 0; i < m_vertices.size(); i++) {
        m_colors.push_back(_color);
    }
}

void Mesh::addColor(const glm::vec4 &_color) {
    m_colors.push_back(_color);
}

void Mesh::addColors(const std::vector<glm::vec4> &_colors) {
    m_colors.insert(m_colors.end(), _colors.begin(), _colors.end());
}

void Mesh::addVertex(const glm::vec3 &_point) {
   m_vertices.push_back(_point);
}

void Mesh::addVertices(const std::vector<glm::vec3>& _verts) {
   m_vertices.insert(m_vertices.end(),_verts.begin(),_verts.end());
}

void Mesh::addVertices(const glm::vec3* verts, int amt) {
   m_vertices.insert(m_vertices.end(),verts,verts+amt);
}

void Mesh::addNormal(const glm::vec3 &_normal) {
    m_normals.push_back(_normal);
}

void Mesh::addNormals(const std::vector<glm::vec3> &_normals ) {
    m_normals.insert(m_normals.end(), _normals.begin(), _normals.end());
}

void  Mesh::addTangent(const glm::vec4 &_tangent) {
    m_tangents.push_back(_tangent);
}

void Mesh::addTexCoord(const glm::vec2 &_uv) {
    m_texCoords.push_back(_uv);
}

void Mesh::addTexCoords(const std::vector<glm::vec2> &_uvs) {
    m_texCoords.insert(m_texCoords.end(), _uvs.begin(), _uvs.end());
}

void Mesh::addIndex(INDEX_TYPE _i) {
    m_indices.push_back(_i);
}

void Mesh::addIndices(const std::vector<INDEX_TYPE>& inds) {
    m_indices.insert(m_indices.end(),inds.begin(),inds.end());
}

void Mesh::addIndices(const INDEX_TYPE* inds, int amt) {
    m_indices.insert(m_indices.end(),inds,inds+amt);
}

void Mesh::addTriangleIndices(INDEX_TYPE index1, INDEX_TYPE index2, INDEX_TYPE index3) {
    addIndex(index1);
    addIndex(index2);
    addIndex(index3);
}

void Mesh::append(const Mesh &_mesh) {
    if (_mesh.getDrawMode() != m_drawMode) {
        std::cout << "INCOMPATIBLE DRAW MODES" << std::endl;
        return;
    }

    INDEX_TYPE indexOffset = (INDEX_TYPE)getVertices().size();

    addColors(_mesh.getColors());
    addVertices(_mesh.getVertices());
    addNormals(_mesh.getNormals());
    addTexCoords(_mesh.getTexCoords());

    for (uint32_t i = 0; i < _mesh.m_indices.size(); i++)
        addIndex(indexOffset + _mesh.m_indices[i]);
}

void Mesh::invertWindingOrder() {
    if ( getDrawMode() == TRIANGLES) {
        int tmp;
        for (size_t i = 0; i < m_indices.size(); i += 3) {
            tmp = m_indices[i+1];
            m_indices[i+1] = m_indices[i+2];
            m_indices[i+2] = tmp;
        }
    }
}

std::vector<glm::ivec3> Mesh::getTrianglesIndices() const {
    std::vector<glm::ivec3> faces;

    if (getDrawMode() == TRIANGLES) {
        if (haveIndices()) {
            for(unsigned int j = 0; j < m_indices.size(); j += 3) {
                glm::ivec3 tri;
                for(int k = 0; k < 3; k++) {
                    tri[k] = m_indices[j+k];
                }
                faces.push_back(tri);
            }
        }
        else {
            for( unsigned int j = 0; j < m_vertices.size(); j += 3) {
                glm::ivec3 tri;
                for(int k = 0; k < 3; k++) {
                    tri[k] = j+k;
                }
                faces.push_back(tri);
            }
        }
    }
    else {
        //  TODO
        //
        std::cout << "ERROR: getTriangles(): Mesh only add TRIANGLES for NOW !!" << std::endl;
    }

    return faces;
}

void Mesh::clear() {
    if (!m_vertices.empty()) m_vertices.clear();
    if (haveColors()) m_colors.clear();
    if (haveNormals()) m_normals.clear();
    if (haveTexCoords()) m_texCoords.clear();
    if (haveTangents()) m_tangents.clear();
    if (haveIndices()) m_indices.clear();
}

bool Mesh::computeNormals() {
    if (getDrawMode() != TRIANGLES) 
        return false;

    //The number of the vertices
    int nV = m_vertices.size();

    //The number of the triangles
    int nT = m_indices.size() / 3;

    std::vector<glm::vec3> norm( nV ); //Array for the normals

    //Scan all the triangles. For each triangle add its
    //normal to norm's vectors of triangle's vertices
    for (int t=0; t<nT; t++) {

        //Get indices of the triangle t
        int i1 = m_indices[ 3 * t ];
        int i2 = m_indices[ 3 * t + 1 ];
        int i3 = m_indices[ 3 * t + 2 ];

        //Get vertices of the triangle
        const glm::vec3 &v1 = m_vertices[ i1 ];
        const glm::vec3 &v2 = m_vertices[ i2 ];
        const glm::vec3 &v3 = m_vertices[ i3 ];

        //Compute the triangle's normal
        glm::vec3 dir = glm::normalize(glm::cross(v2-v1,v3-v1));

        //Accumulate it to norm array for i1, i2, i3
        norm[ i1 ] += dir;
        norm[ i2 ] += dir;
        norm[ i3 ] += dir;
    }

    //Normalize the normal's length and add it.
    m_normals.clear();
    for (int i=0; i<nV; i++)
        addNormal( glm::normalize(norm[i]) );

    return true;
}


void Mesh::invertNormals() {
    for (std::vector<glm::vec3>::iterator it = m_normals.begin(); it != m_normals.end(); ++it)
        *it *= -1.0f;
}

void Mesh::flatNormals() {
    if ( getDrawMode() == TRIANGLES) {
        
        // get copy original mesh data
        size_t numIndices = m_indices.size();
        std::vector<INDEX_TYPE> indices = m_indices;
        std::vector<glm::vec3> verts = m_vertices;
        std::vector<glm::vec4> colors = m_colors;
        std::vector<glm::vec2> texCoords = m_texCoords;
        
        // remove all data to start from scratch
        clear();
        
        // add mesh data back, duplicating vertices and recalculating normals
        glm::vec3 normal;
        for (size_t i = 0; i < numIndices; i++) {
            size_t indexCurr = indices[i];
    
            if (i % 3 == 0) {
                INDEX_TYPE indexNext1 = indices[i + 1];
                INDEX_TYPE indexNext2 = indices[i + 2];
                glm::vec3 e1 = verts[indexCurr] - verts[indexNext1];
                glm::vec3 e2 = verts[indexNext2] - verts[indexNext1];
                normal = glm::normalize(glm::cross(e1, e2));
            }
    
            addIndex(i);
            addNormal(normal);
            if (indexCurr < texCoords.size()) addTexCoord(texCoords[indexCurr]);
            if (indexCurr < verts.size()) addVertex(verts[indexCurr]);
            if (indexCurr < colors.size()) addColor(colors[indexCurr]);
        }
    }
}

void Mesh::smoothNormals(float _angle) {

    std::vector<Triangle> triangles = getTriangles();
    std::vector<glm::vec3> verts;
    for (size_t i = 0; i < triangles.size(); i++)
        for (size_t j = 0; j < 3; j++) 
            verts.push_back( triangles[i][j] );

    std::map<int, int> removeIds;
    float epsilon = .01f;
    for (size_t i = 0; i < verts.size()-1; i++) {
        for (size_t j = i+1; j < verts.size(); j++) {
            if (i != j) {
                const glm::vec3& v1 = verts[i];
                const glm::vec3& v2 = verts[j];
                if ( glm::distance(v1, v2) <= epsilon ) {
                    // average the location //
                    verts[i] = (v1+v2)/2.f;
                    verts[j] = verts[i];
                    removeIds[j] = 1;
                }
            }
        }
    }

    std::map<std::string, std::vector<int> > vertHash;
    std::string xStr, yStr, zStr;

    for (size_t i = 0; i < verts.size(); i++ ) {
        xStr = "x" + toString(verts[i].x == -0 ? 0: verts[i].x);
        yStr = "y" + toString(verts[i].y == -0 ? 0: verts[i].y);
        zStr = "z" + toString(verts[i].z == -0 ? 0: verts[i].z);
        std::string vstring = xStr+yStr+zStr;

        if (vertHash.find(vstring) == vertHash.end())
            for (size_t j = 0; j < triangles.size(); j++) 
                for (size_t k = 0; k < 3; k++) 
                    if (verts[i].x == triangles[j][k].x)
                        if (verts[i].y == triangles[j][k].y) 
                            if (verts[i].z == triangles[j][k].z)
                                vertHash[vstring].push_back( j );
    }

    glm::vec3 vert;
    glm::vec3 normal;
    float angleCos = cos(glm::radians(_angle));
    float numNormals = 0.0f;

    for (size_t j = 0; j < triangles.size(); j++) {
        for (size_t k = 0; k < 3; k++) {
            vert = triangles[j][k];
            xStr = "x" + toString(vert.x==-0?0:vert.x);
            yStr = "y" + toString(vert.y==-0?0:vert.y);
            zStr = "z" + toString(vert.z==-0?0:vert.z);

            std::string vstring = xStr+yStr+zStr;
            numNormals=0;
            normal = {0.f,0.f,0.f};
            if (vertHash.find(vstring) != vertHash.end()) {
                for (size_t i = 0; i < vertHash[vstring].size(); i++) {
                    glm::vec3 f1 = triangles[j].getNormal();
                    glm::vec3 f2 = triangles[vertHash[vstring][i]].getNormal();
                    if (glm::dot(f1, f2) >= angleCos ) {
                        normal += f2;
                        numNormals+=1.f;
                    }
                }
                //normal /= (float)vertHash[vstring].size();
                normal /= numNormals;

                triangles[j].setNormal(k, normal);
            }
        }
    }

    clear();
    addTriangles( triangles );
}


// http://www.terathon.com/code/tangent.html
bool Mesh::computeTangents() {
    //The number of the vertices
    size_t nV = m_vertices.size();

    if (m_texCoords.size() != nV || 
        m_normals.size() != nV || 
        getDrawMode() != TRIANGLES)
        return false;

    //The number of the triangles
    size_t nT = m_indices.size() / 3;

    std::vector<glm::vec3> tan1( nV );
    std::vector<glm::vec3> tan2( nV );

    //Scan all the triangles. For each triangle add its
    //normal to norm's vectors of triangle's vertices
    for (size_t t = 0; t < nT; t++) {

        //Get indices of the triangle t
        int i1 = m_indices[ 3 * t ];
        int i2 = m_indices[ 3 * t + 1 ];
        int i3 = m_indices[ 3 * t + 2 ];

        //Get vertices of the triangle
        const glm::vec3 &v1 = m_vertices[ i1 ];
        const glm::vec3 &v2 = m_vertices[ i2 ];
        const glm::vec3 &v3 = m_vertices[ i3 ];

        const glm::vec2 &w1 = m_texCoords[i1];
        const glm::vec2 &w2 = m_texCoords[i2];
        const glm::vec2 &w3 = m_texCoords[i3];

        float x1 = v2.x - v1.x;
        float x2 = v3.x - v1.x;
        float y1 = v2.y - v1.y;
        float y2 = v3.y - v1.y;
        float z1 = v2.z - v1.z;
        float z2 = v3.z - v1.z;
        
        float s1 = w2.x - w1.x;
        float s2 = w3.x - w1.x;
        float t1 = w2.y - w1.y;
        float t2 = w3.y - w1.y;
        
        float r = 1.0f / (s1 * t2 - s2 * t1);
        glm::vec3 sdir( (t2 * x1 - t1 * x2) * r, 
                        (t2 * y1 - t1 * y2) * r, 
                        (t2 * z1 - t1 * z2) * r);
        glm::vec3 tdir( (s1 * x2 - s2 * x1) * r, 
                        (s1 * y2 - s2 * y1) * r, 
                        (s1 * z2 - s2 * z1) * r);
        
        tan1[i1] += sdir;
        tan1[i2] += sdir;
        tan1[i3] += sdir;
        
        tan2[i1] += tdir;
        tan2[i2] += tdir;
        tan2[i3] += tdir;
    }

    //Normalize the normal's length and add it.
    m_tangents.clear();
    for (size_t i = 0; i < nV; i++) {
        const glm::vec3 &n = m_normals[i];
        const glm::vec3 &t = tan1[i];
        
        // Gram-Schmidt orthogonalize
        glm::vec3 tangent = t - n * glm::dot(n, t);

        // Calculate handedness
        float hardedness = (glm::dot( glm::cross(n, t), tan2[i]) < 0.0f) ? -1.0f : 1.0f;

        addTangent(glm::vec4(tangent, hardedness));
    }

    return true;
}

void Mesh::addTriangle(const Triangle& _tri) {
    INDEX_TYPE index = m_vertices.size();

    addVertex(_tri[0]);
    addVertex(_tri[1]);
    addVertex(_tri[2]);

    if (_tri.haveColors()) {
        addColor(_tri.getColor(0));
        addColor(_tri.getColor(1));
        addColor(_tri.getColor(2));
    }

    if (_tri.haveNormals()) {
        addNormal(_tri.getNormal(0));
        addNormal(_tri.getNormal(1));
        addNormal(_tri.getNormal(2));
    }
    else {
        addNormal(_tri.getNormal());
        addNormal(_tri.getNormal());
        addNormal(_tri.getNormal());
    }

    if (_tri.haveTexCoords()) {
        addTexCoord(_tri.getTexCoord(0));
        addTexCoord(_tri.getTexCoord(1));
        addTexCoord(_tri.getTexCoord(2));
    }

    addTriangleIndices(index, index+1, index+2);
}

void Mesh::addTriangles(const std::vector<Triangle>& _triangles) {
    for (size_t i = 0; i < _triangles.size(); i++)
        addTriangle(_triangles[i]);
}

std::vector<Triangle> Mesh::getTriangles() const {
    std::vector<glm::ivec3> triIndices = getTrianglesIndices();
    std::vector<Triangle> triangles;

    int t = 0;
    for (std::vector<glm::ivec3>::iterator it = triIndices.begin(); it != triIndices.end(); ++it) {
        Triangle tri = Triangle(m_vertices[it->x], m_vertices[it->y], m_vertices[it->z]);
        if (haveColors()) tri.setColors(m_colors[it->x], m_colors[it->y], m_colors[it->z]);
        if (haveNormals()) tri.setNormals(m_normals[it->x], m_normals[it->y], m_normals[it->z]);
        if (haveTexCoords()) tri.setTexCoords(m_texCoords[it->x], m_texCoords[it->y], m_texCoords[it->z]);
        if (haveTangents()) tri.setTangents(m_tangents[it->x], m_tangents[it->y], m_tangents[it->z]);
        // if (haveMaterials()) tri.material = getMaterialForFaceIndex(it->x);
        
        triangles.push_back( tri );
        t++;
    }

    return triangles;
}


}