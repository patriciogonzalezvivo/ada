#include "ada/gl/vbo.h"
#include <iostream>

namespace ada {

Vbo::Vbo() : 
    m_vertexLayout(NULL),
    m_glVertexBuffer(0),
    m_nVertices(0),
    m_glIndexBuffer(0),
    m_nIndices(0),
    m_drawType(GL_STATIC_DRAW),
    m_drawMode(GL_TRIANGLES),
    m_isUploaded(false) {
}

Vbo::Vbo(VertexLayout* _vertexLayout, GLenum _drawMode) : 
    m_vertexLayout(_vertexLayout), 
    m_glVertexBuffer(0), 
    m_nVertices(0), 
    m_glIndexBuffer(0), 
    m_nIndices(0), 
    m_drawType(GL_STATIC_DRAW), 
    m_isUploaded(false) {
    setDrawMode(_drawMode);
}

Vbo::~Vbo() {
    m_vertexData.clear();
    m_indices.clear();

    if (m_vertexLayout != NULL)
        delete m_vertexLayout;

    glDeleteBuffers(1, &m_glVertexBuffer);
    glDeleteBuffers(1, &m_glIndexBuffer);
}

void Vbo::setVertexLayout(VertexLayout* _vertexLayout) {
    if (m_vertexLayout != NULL){
        delete m_vertexLayout;
    }
    m_vertexLayout = _vertexLayout;
}

void Vbo::setDrawType(GLenum _drawType) {
    switch (_drawType) {
        case GL_STREAM_DRAW:
        case GL_STATIC_DRAW:
        case GL_DYNAMIC_DRAW:
            m_drawType = _drawType;
            break;
        default:
            std::cout << "Invalid draw type for mesh! Defaulting to GL_STATIC_DRAW" << std::endl;
            m_drawType = GL_STATIC_DRAW;
    }
}

void Vbo::setDrawMode(GLenum _drawMode) {
    switch (_drawMode) {
        case GL_POINTS:
        case GL_LINE_STRIP:
        case GL_LINE_LOOP:
        case GL_LINES:
        case GL_TRIANGLE_STRIP:
        case GL_TRIANGLE_FAN:
        case GL_TRIANGLES:
            m_drawMode = _drawMode;
            break;
        default:
            std::cout << "Invalid draw mode for mesh! Defaulting to GL_TRIANGLES" << std::endl;
            m_drawMode = GL_TRIANGLES;
    }
}

void Vbo::addVertex(GLbyte* _vertex) {
    addVertices(_vertex, 1);
}

void Vbo::addVertices(GLbyte* _vertices, int _nVertices) {
    if (m_isUploaded) {
        std::cout << "Vbo cannot add vertices after upload!" << std::endl;
        return;
    }

    // // Only add up to 65535 vertices, any more will overflow our 16-bit indices
    // int indexSpace = MAX_INDEX_VALUE - m_nVertices;
    // if (_nVertices > indexSpace) {
    //     _nVertices = indexSpace;
    //     std::cout << "WARNING: Tried to add more vertices than available in index space" << std::endl;
    // }

    int vertexBytes = m_vertexLayout->getStride() * _nVertices;
    m_vertexData.insert(m_vertexData.end(), _vertices, _vertices + vertexBytes);
    m_nVertices += _nVertices;
}

void Vbo::addIndex(INDEX_TYPE_GL* _index) {
    addIndices(_index, 1);
}

void Vbo::addIndices(INDEX_TYPE_GL* _indices, int _nIndices) {
    if (m_isUploaded) {
        std::cout << "Vbo cannot add indices after upload!" << std::endl;
        return;
    }

    // if (m_nVertices >= MAX_INDEX_VALUE) {
    //     std::cout << "WARNING: Vertex buffer full, not adding indices" << std::endl;
    //     return;
    // }

    m_indices.insert(m_indices.end(), _indices, _indices + _nIndices);
    m_nIndices += _nIndices;
}

void Vbo::upload() {
    if (m_nVertices > 0) {
        // Generate vertex buffer, if needed
        if (m_glVertexBuffer == 0) {
            glGenBuffers(1, &m_glVertexBuffer);
        }

        // Buffer vertex data
        glBindBuffer(GL_ARRAY_BUFFER, m_glVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, m_vertexData.size(), m_vertexData.data(), m_drawType);
    }

    if (m_nIndices > 0) {
        // Generate index buffer, if needed
        if (m_glIndexBuffer == 0) {
            glGenBuffers(1, &m_glIndexBuffer);
        }

        // Buffer element index data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glIndexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(INDEX_TYPE_GL), m_indices.data(), m_drawType);
    }

    if (m_drawType == GL_STATIC_DRAW)
        m_vertexData.clear();

    m_indices.clear();

    m_isUploaded = true;
}

void Vbo::printInfo() {
    std::cout << "Vertices  = " << m_nVertices << std::endl;
    std::cout << "Indices   = " << m_nIndices << std::endl;
    if (m_vertexLayout) {
        std::cout << "Vertex Layout:" << std::endl;
        m_vertexLayout->printAttrib();
    }
}

void Vbo::render(Shader* _shader) {

    // Ensure that geometry is buffered into GPU
    if (!m_isUploaded) {
        upload();
    }

    // Bind buffers for drawing
    if (m_nVertices > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, m_glVertexBuffer);
    }

    if (m_nIndices > 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glIndexBuffer);
    }

    // Enable shader program
    _shader->use();

    // Enable vertex attribs via vertex layout object
    m_vertexLayout->enable(_shader);

#if !defined(PLATFORM_RPI)&& !defined(_WIN32)
    if (m_drawMode == GL_POINTS) {
        glEnable(GL_POINT_SPRITE);
        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    }
#endif

    // Draw as elements or arrays
    if (m_nIndices > 0) {
        #if defined(PLATFORM_RPI) || defined(__EMSCRIPTEN__)
        glDrawElements(m_drawMode, m_nIndices, GL_UNSIGNED_SHORT, 0);
        #else
        glDrawElements(m_drawMode, m_nIndices, GL_UNSIGNED_INT, 0);
        #endif
    } else if (m_nVertices > 0) {
        glDrawArrays(m_drawMode, 0, m_nVertices);
    }
}

}