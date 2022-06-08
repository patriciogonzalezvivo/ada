//
// Copyright (c) 2009-2013 Mikko Mononen memon@inside.org
// Copyright (c) 2014 Mapzen karim@mapzen.com
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//
#ifndef GLFONTSTASH_H
#define GLFONTSTASH_H

#include <unordered_map>
#include <vector>
#include <climits>
#include <string>
#include <algorithm>
#include <cfloat>

#include "fontstash.h"
#include "shaders.h"

// #define USE_ADA_SHADER
#if defined(USE_ADA_SHADER)
#include "ada/gl/shader.h"
#endif
#include "ada/window.h"

typedef unsigned int fsuint;

typedef struct GLFONScontext GLFonscontext;
typedef struct GLFONSparams GLFONSparams;

struct GLFONSparams {
    bool useGLBackend;
    void (*updateBuffer)(void* usrPtr, GLintptr offset, GLsizei size, float* newData, void* owner);
    void (*updateAtlas)(void* usrPtr, unsigned int xoff, unsigned int yoff, unsigned int width, unsigned int height, const unsigned int* pixels);
};

enum {
    GLFONS_VALID,
    GLFONS_INVALID
};

// GLFONTSTASH API
FONScontext* glfonsCreate(int width, int height, int flags, GLFONSparams glParams, void* userPtr);
void glfonsDelete(FONScontext* ctx);
void glfonsSetAlpha(FONScontext* ctx, fsuint id, float a);
void glfonsRotate(FONScontext* ctx, fsuint id, float r);
void glfonsTranslate(FONScontext* ctx, fsuint id, float tx, float ty);
void glfonsTransform(FONScontext* ctx, fsuint id, float tx, float ty, float r, float a);
void glfonsGenText(FONScontext* ctx, unsigned int nb, fsuint* textId);
void glfonsBufferCreate(FONScontext* ctx, fsuint* id);
void glfonsBufferDelete(FONScontext* ctx, fsuint id);
void glfonsBindBuffer(FONScontext* ctx, fsuint id);
int glfonsRasterize(FONScontext* ctx, fsuint textId, const char* s, unsigned int color = 0x000000);
bool glfonsVertices(FONScontext* ctx, float* data);
int glfonsVerticesSize(FONScontext* ctx);
void glfonsGetBBox(FONScontext* ctx, fsuint id, float* x0, float* y0, float* x1, float* y1);
float glfonsGetLength(FONScontext* ctx, fsuint id);
int glfonsGetGlyphCount(FONScontext* ctx, fsuint id);
void glfonsScreenSize(FONScontext* ctx, int screenWidth, int screenHeight);
void glfonsProjection(FONScontext* ctx, float* projectionMatrix);
void glfonsUpdateBuffer(FONScontext* ctx, void* owner = nullptr);
void glfonsDraw(FONScontext* ctx);
void glfonsSetColor(FONScontext* ctx, unsigned int color);
unsigned int glfonsRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

#endif

#ifdef GLFONTSTASH_IMPLEMENTATION

#define FONTSTASH_IMPLEMENTATION
#include "fontstash.h"

#ifdef GLFONS_DEBUG
#define GLFONS_GL_CHECK(stmt) do { stmt; glfons__OGLError(#stmt, __FILE__, __LINE__); } while (0)
#else
#define GLFONS_GL_CHECK(stmt) stmt
#endif

#define ATLAS_TEXTURE_SLOT  0
#define GLYPH_VERTS         6

#define GLFONS_LOAD_BUFFER \
    GLFONScontext* gl = (GLFONScontext*) ctx->params.userPtr; \
    GLFONSbuffer* buffer = glfons__bufferBound(gl); \

#define GLFONS_LOAD_STASH \
    GLFONS_LOAD_BUFFER \
    GLFONSstash* stash = buffer->stashes[id]; \

struct GLFONSstash {
    short nbGlyph;
    short padding;
    float bbox[4];
    float length;
    size_t offset;
};

struct GLFONSvertexAttrib {
    std::string name;
    GLint size;
    GLboolean normalized;
    GLvoid* offset;
    GLint location;
};

struct GLFONSVertexLayout {
    size_t nbComponents;
    GLsizei stride;
    std::vector<GLFONSvertexAttrib> attributes;
};

struct GLFONSbuffer {
    fsuint id;
    fsuint textIdCount;
    GLuint vbo;
    unsigned int nVerts, color = 0xffffff;
    std::vector<float> interleavedArray;
    std::unordered_map<fsuint, GLFONSstash*> stashes;
    GLintptr dirtyOffset;
    GLsizei dirtySize;
    bool vboInitialized;
    bool dirty;
};

struct GLFONScontext {
    GLFONSVertexLayout layout;
    GLFONSparams params;
    int atlasRes[2];
    GLuint atlas;
    GLuint program;
    fsuint bufferCount;
    fsuint boundBuffer;
    float screenSize[2];
    float projectionMatrix[16];
    bool resolutionDirty;
    void* userPtr;
    std::unordered_map<fsuint, GLFONSbuffer*> buffers;
};

static int glfons__renderCreate(void* userPtr, int width, int height) {
    GLFONScontext* gl = (GLFONScontext*)userPtr;

    gl->bufferCount = 0;
    gl->atlasRes[0] = width;
    gl->atlasRes[1] = height;

    return 1;
}

static int glfons__renderResize(void* userPtr, int width, int height) {
    (void) height;
    (void) width;
    (void) userPtr;
    return 1;
}

static void glfons__renderUpdate(void* userPtr, int* rect, const unsigned char* data) {
    GLFONScontext* gl = (GLFONScontext*)userPtr;

    int h = rect[3] - rect[1];
    const unsigned char* subdata = data + rect[1] * gl->atlasRes[0];
    gl->params.updateAtlas(gl->userPtr, 0, rect[1], gl->atlasRes[0], h, reinterpret_cast<const unsigned int*>(subdata));
}

static void glfons__renderDraw(void* userPtr, const float* verts, const float* tcoords, const unsigned int* colors, int nverts) {
    (void) userPtr;
    (void) verts;
    (void) tcoords;
    (void) colors;
    (void) nverts;
    // called by fontstash, but has nothing to do
}

void glfons__createVertexLayout(GLFONScontext* gl) {
    GLFONSVertexLayout layout;

    layout.attributes.push_back({"a_position", 2, false, 0, -1});
    layout.attributes.push_back({"a_uvs", 2, false, 0, -1});
    layout.attributes.push_back({"a_screenPosition", 2, false, 0, -1});
    layout.attributes.push_back({"a_alpha", 1, false, 0, -1});
    layout.attributes.push_back({"a_rotation", 1, false, 0, -1});
    layout.nbComponents = 0;
    layout.stride = 0;

    for(auto& attribute : layout.attributes) {
        attribute.offset = (GLvoid *) (layout.nbComponents * sizeof(float));
        layout.nbComponents += attribute.size;
        layout.stride += attribute.size * sizeof(float);
    }

    gl->layout = layout;
}

void glfons__initVertexLayout(GLFONScontext* gl, GLuint shaderProgram) {
    glfons__createVertexLayout(gl);

    for(auto& attribute : gl->layout.attributes) {
        attribute.location = glGetAttribLocation(shaderProgram, attribute.name.c_str());
    }
}

int glfons__layoutIndex(GLFONScontext* gl, std::string attributeName) {
    for(unsigned int i = 0, index = 0; i < gl->layout.attributes.size(); index += gl->layout.attributes[i].size, ++i) {
        if(gl->layout.attributes[i].name == attributeName) {
            return index;
        }
    }
    return -1;
}

void glfons__OGLError(const char* stmt, const char* fname, int line) {
    GLenum err = glGetError();
    if(err != GL_NO_ERROR) {
        printf("OpenGL error %08x, at %s:%i - for %s\n", err, fname, line, stmt);
        exit(-1);
    }
}

GLFONSbuffer* glfons__bufferBound(GLFONScontext* gl) {
    if(gl->boundBuffer == 0) {
        return nullptr;
    }

    return gl->buffers.at(gl->boundBuffer);
}

GLuint glfons__compileShader(const GLchar* src, GLenum type) {
    GLuint shader = glCreateShader(type);

    GLFONS_GL_CHECK(glShaderSource(shader, 1, &src, NULL));
    GLFONS_GL_CHECK(glCompileShader(shader));

    GLint isCompiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

    if (isCompiled == GL_FALSE) {
        glDeleteShader(shader);
        return -1;
    }

    return shader;
}

void glfons__enableVertexLayout(GLFONScontext* gl) {
    for(auto& attribute : gl->layout.attributes) {
        GLFONS_GL_CHECK(glVertexAttribPointer(attribute.location, attribute.size, GL_FLOAT, attribute.normalized, gl->layout.stride, (const GLvoid *) attribute.offset));
        GLFONS_GL_CHECK(glEnableVertexAttribArray(attribute.location));
    }
}

void glfons__disableVertexLayout(GLFONScontext* gl) {
    for(auto& attribute : gl->layout.attributes) {
        glDisableVertexAttribArray(attribute.location);
    }
}

#if defined(USE_ADA_SHADER)
ada::Shader glfons__shader;
#endif

void glfons__initShaders(GLFONScontext* gl) {

#if defined(USE_ADA_SHADER)

    #if defined(__EMSCRIPTEN__)
    if (ada::getWebGLVersionNumber() == 2)
        glfons__shader.load(glfs::sdfFragShaderSrc_300, glfs::vertexShaderSrc_300);
    else 
    #endif
    glfons__shader.load(glfs::sdfFragShaderSrc, glfs::vertexShaderSrc);

    GLuint program = glfons__shader.getProgram();
#else
    GLuint program = glCreateProgram();
    GLuint vertex, fragment;

    #if defined(__EMSCRIPTEN__)
    if (ada::getWebGLVersionNumber() == 2) {
        vertex = glfons__compileShader(glfs::vertexShaderSrc_300, GL_VERTEX_SHADER);
        fragment = glfons__compileShader(glfs::sdfFragShaderSrc_300, GL_FRAGMENT_SHADER);
    }
    else 
    #endif
    {
        vertex = glfons__compileShader(glfs::vertexShaderSrc, GL_VERTEX_SHADER);
        fragment = glfons__compileShader(glfs::sdfFragShaderSrc, GL_FRAGMENT_SHADER);
    }

    GLFONS_GL_CHECK(glAttachShader(program, vertex));
    GLFONS_GL_CHECK(glAttachShader(program, fragment));
    GLFONS_GL_CHECK(glLinkProgram(program));

    glDeleteShader(vertex);
    glDeleteShader(fragment);
#endif
    gl->program = program;

    GLuint boundProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &boundProgram);
    GLFONS_GL_CHECK(glUseProgram(program));
    GLFONS_GL_CHECK(glUniform1i(glGetUniformLocation(gl->program, "u_tex"), ATLAS_TEXTURE_SLOT));
    glfons__initVertexLayout(gl, program);
    glUseProgram(boundProgram);
}

void glfons__bindUniforms(GLFONScontext* gl, GLFONSbuffer* buffer) {
    float r = (buffer->color & 0xff) / 255.0;
    float g = (buffer->color >> 8 & 0xff) / 255.0;
    float b = (buffer->color >> 16 & 0xff) / 255.0;

    GLFONS_GL_CHECK(glUniform3f(glGetUniformLocation(gl->program, "u_color"), r, g, b));
}

void glfons__updateProjection(GLFONScontext* gl) {
    if(gl->resolutionDirty) {
        float width = gl->screenSize[0];
        float height = gl->screenSize[1];

        float r = width;
        float l = 0.0;
        float b = height;
        float t = 0.0;
        float n = -1.0;
        float f = 1.0;

        // ensure array is 0-filled
        std::memset(gl->projectionMatrix, 0, 16 * sizeof(float));

        // could be simplified, exposing it like this for comprehension
        gl->projectionMatrix[0]  = 2.0 / (r - l);
        gl->projectionMatrix[5]  = 2.0 / (t - b);
        gl->projectionMatrix[10] = 2.0 / (f - n);
        gl->projectionMatrix[12] = -(r + l) / (r - l);
        gl->projectionMatrix[13] = -(t + b) / (t - b);
        gl->projectionMatrix[14] = -(f + n) / (f - n);
        gl->projectionMatrix[15] = 1.0;
        gl->resolutionDirty = false;
    }
}

void glfons__projection(GLFONScontext* gl, float* projectionMatrix) {
    glfons__updateProjection(gl);
    std::memcpy(projectionMatrix, gl->projectionMatrix, 16 * sizeof(float));
}

void glfons__draw(GLFONScontext* gl, bool bindAtlas) {
    GLFONS_GL_CHECK(glUseProgram(gl->program));

    if(gl->resolutionDirty) {
        glfons__updateProjection(gl);
        GLFONS_GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(gl->program, "u_proj"), 1, GL_FALSE, gl->projectionMatrix));
    }

    if(bindAtlas) {
        glActiveTexture(GL_TEXTURE0 + ATLAS_TEXTURE_SLOT);
        GLFONS_GL_CHECK(glBindTexture(GL_TEXTURE_2D, gl->atlas));
    }

    for(auto& pair : gl->buffers) {
        GLFONSbuffer* buffer = pair.second;
        GLFONS_GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo));
        glfons__enableVertexLayout(gl);
        glfons__bindUniforms(gl, buffer);
        GLFONS_GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, buffer->nVerts));
        glfons__disableVertexLayout(gl);
    }
}

void glfons__setDirty(GLFONSbuffer* buffer, GLintptr start, GLsizei size) {
    GLintptr byteOffset = start * sizeof(float);
    GLsizei byteSize = size * sizeof(float);

    if(!buffer->dirty) {
        buffer->dirtySize = byteSize;
        buffer->dirtyOffset = byteOffset;
        buffer->dirty = true;
    } else {
        long dBytes = std::abs((long double)byteOffset - buffer->dirtyOffset);
        long newEnd = byteOffset + byteSize;
        long oldEnd = buffer->dirtySize + buffer->dirtyOffset;

        if(byteOffset < buffer->dirtyOffset) {
            buffer->dirtyOffset = byteOffset;
            if(newEnd > oldEnd) {
                buffer->dirtySize = byteSize;
            } else {
                buffer->dirtySize += dBytes;
            }
            buffer->dirty = true;
        } else if(newEnd > oldEnd) {
            buffer->dirtySize = dBytes + byteSize;
            buffer->dirty = true;
        }
    }
}

void glfons__updateBuffer(void* usrPtr, GLintptr offset, GLsizei size, float* newData, void* owner) {
    (void) owner;
    GLFONScontext* gl = (GLFONScontext*) usrPtr;
    GLFONSbuffer* buffer = glfons__bufferBound(gl);

    glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
    GLFONS_GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, offset, size, newData));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void glfons__udpateAtas(void* usrPtr, unsigned int xoff, unsigned int yoff,
                        unsigned int width, unsigned int height, const unsigned int* pixels) {
    GLFONScontext* gl = (GLFONScontext*) usrPtr;

    glActiveTexture(GL_TEXTURE0 + ATLAS_TEXTURE_SLOT);
    GLFONS_GL_CHECK(glBindTexture(GL_TEXTURE_2D, gl->atlas));
    glTexSubImage2D(GL_TEXTURE_2D, 0, xoff, yoff, width, height, GL_ALPHA, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void glfonsSetColor(FONScontext* ctx, unsigned int color) {
    GLFONScontext* gl = (GLFONScontext*) ctx->params.userPtr;
    GLFONSbuffer* buffer = glfons__bufferBound(gl);

    if(!gl->params.useGLBackend) {
        // should be directly sent to shader from own gl-backend
        return;
    }

    buffer->color = color;
}

void glfonsGenText(FONScontext* ctx, unsigned int nb, fsuint* textId) {
    GLFONScontext* gl = (GLFONScontext*) ctx->params.userPtr;
    GLFONSbuffer* buffer = glfons__bufferBound(gl);

    for(unsigned int i = 0; i < nb; ++i) {
        textId[i] = buffer->textIdCount++;
    }
}

int glfonsRasterize(FONScontext* ctx, fsuint textId, const char* s, unsigned int color) {
    (void) color;
    GLFONScontext* gl = (GLFONScontext*) ctx->params.userPtr;
    GLFONSbuffer* buffer = glfons__bufferBound(gl);

    int length = fonsDrawText(ctx, 0, 0, s, NULL, 0);

    if (length == -1.f) {
        return GLFONS_INVALID;
    }

    GLFONSstash* stash = new GLFONSstash;

    stash->offset = buffer->interleavedArray.size();

    // pre-allocate data for this text id
    int oldSize = buffer->interleavedArray.size();
    buffer->interleavedArray.resize(oldSize + gl->layout.nbComponents * ctx->nverts, 0);

    float inf = std::numeric_limits<float>::infinity();
    float x0 = inf, x1 = -inf, y0 = inf, y1 = -inf;

    for(int i = 0, index = oldSize; i < ctx->nverts * 2; i += 2) {
        float x, y;

        x = ctx->verts[i];
        y = ctx->verts[i + 1];

        x0 = x < x0 ? x : x0;
        x1 = x > x1 ? x : x1;
        y0 = y < y0 ? y : y0;
        y1 = y > y1 ? y : y1;

        buffer->interleavedArray[index++] = x;
        buffer->interleavedArray[index++] = y;
        buffer->interleavedArray[index++] = ctx->tcoords[i];
        buffer->interleavedArray[index++] = ctx->tcoords[i + 1];
        index += 4; // skip screenPos / alpha / rotation
    }

    // remove extra-offset used for interpolation in fontstash
    stash->bbox[0] = x0 + 3;
    stash->bbox[1] = y0 - 3;
    stash->bbox[2] = x1 + 3;
    stash->bbox[3] = y1 - 3;

    stash->length = length - 6;

    if(ctx->shaping != NULL && fons__getState(ctx)->useShaping) {
        FONSshapingRes* res = ctx->shaping->result;
        stash->nbGlyph = res->glyphCount;
        fons__clearShaping(ctx);
    } else {
        const char* str = s;
        const char* end = s + strlen(s);
        int i = 0;
        unsigned int utf8state = 0;
        unsigned int codepoint = 0;

        // count glyphs
        for (; str != end; ++str) {
            if (fons__decutf8(&utf8state, &codepoint, *(const unsigned char*)str))
                continue;
            i++;
        }
        stash->nbGlyph = i;
    }

    stash->padding = stash->nbGlyph * GLYPH_VERTS * gl->layout.nbComponents;

    // hack : reset fontstash state
    buffer->nVerts += ctx->nverts;
    ctx->nverts = 0;

    buffer->stashes[textId] = stash;

    return GLFONS_VALID;
}

void glfonsBufferCreate(FONScontext* ctx, fsuint* id) {
    GLFONScontext* gl = (GLFONScontext*) ctx->params.userPtr;
    GLFONSbuffer* buffer = new GLFONSbuffer;

    *id = ++gl->bufferCount;

    buffer->textIdCount = 0;
    buffer->id = *id;
    buffer->nVerts = 0;
    buffer->dirtyOffset = 0;
    buffer->dirtySize = 0;
    buffer->dirty = false;

    if(gl->params.useGLBackend) {
        glGenBuffers(1, &buffer->vbo);
        // data not initialized
        buffer->vboInitialized = false;
    }

    // bind the buffer
    gl->boundBuffer = *id;
    gl->buffers[*id] = buffer;
}

void glfonsBufferDelete(GLFONScontext* gl, fsuint id) {
    if (gl->buffers.find(id) == gl->buffers.end()) {
        return;
    }

    GLFONSbuffer* buffer = gl->buffers.at(id);

    if(gl->params.useGLBackend && buffer->vbo != 0) {
        glDeleteBuffers(1, &buffer->vbo);
    }

    for(auto& pair : buffer->stashes) {
        delete pair.second;
    }

    buffer->interleavedArray.clear();
    buffer->stashes.clear();
    delete buffer;
    gl->buffers.erase(id);
}

void glfonsBufferDelete(FONScontext* ctx, fsuint id) {
    GLFONScontext* gl = (GLFONScontext*) ctx->params.userPtr;

    if(gl->buffers.size() == 0) {
        return;
    }

    if(gl->buffers.find(id) != gl->buffers.end()) {
        glfonsBufferDelete(gl, id);
    }
}

void glfonsBindBuffer(FONScontext* ctx, fsuint id) {
    GLFONScontext* gl = (GLFONScontext*) ctx->params.userPtr;
    gl->boundBuffer = id;
}

void glfonsUpdateBuffer(FONScontext* ctx, void* owner) {
    GLFONS_LOAD_BUFFER

    float* data = reinterpret_cast<float*>(buffer->interleavedArray.data());

    if(gl->params.useGLBackend) {
        if(!buffer->vboInitialized) {
            glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
            GLFONS_GL_CHECK(glBufferData(GL_ARRAY_BUFFER,
                                         sizeof(float) * buffer->interleavedArray.size(),
                                         data, GL_DYNAMIC_DRAW));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            buffer->vboInitialized = true;
            return;
        }
    }

    if(!buffer->dirty) {
        return;
    }

    data += buffer->dirtyOffset / sizeof(float);

    gl->params.updateBuffer(gl->userPtr, buffer->dirtyOffset, buffer->dirtySize, data, owner);
    
    buffer->dirty = false;
    buffer->dirtySize = 0;
    buffer->dirtyOffset = 0;
}

void glfonsDraw(FONScontext* ctx) {
    GLFONScontext* gl = (GLFONScontext*)ctx->params.userPtr;

    if(!gl->params.useGLBackend) {
        return;
    }

    GLboolean blending;
    glGetBooleanv(GL_BLEND, &blending);
    GLboolean depthTest;
    glGetBooleanv(GL_DEPTH_TEST, &depthTest);

    GLuint textureUnit0 = 0;
    GLuint boundProgram = 0;
    GLuint boundBuffer = 0;

    // save states
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLint*) &boundBuffer);
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &boundProgram);
    glActiveTexture(GL_TEXTURE0 + ATLAS_TEXTURE_SLOT);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*) &textureUnit0);

    if(depthTest) {
        glDisable(GL_DEPTH_TEST);
    }

    if(!blending) {
        glEnable(GL_BLEND);
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfons__draw(gl, gl->atlas != textureUnit0);
    glBindBuffer(GL_ARRAY_BUFFER, boundBuffer);

    if(!blending) {
        glDisable(GL_BLEND);
    }

    if(depthTest) {
        glEnable(GL_DEPTH_TEST);
    }

    glActiveTexture(GL_TEXTURE0 + ATLAS_TEXTURE_SLOT);
    glBindTexture(GL_TEXTURE_2D, textureUnit0);
    glUseProgram(boundProgram);
}

static void glfons__renderDelete(void* userPtr) {
    GLFONScontext* gl = (GLFONScontext*)userPtr;

    while (!gl->buffers.empty()) {
        auto it = gl->buffers.begin();
        glfonsBufferDelete(gl, it->first);
    }

    if(gl->params.useGLBackend) {
        if(gl->atlas != 0) {
            glDeleteTextures(1, &gl->atlas);
        }
        if (gl->program) {
            glDeleteProgram(gl->program);
        }
    }
    delete gl;
}

void glfonsScreenSize(FONScontext* ctx, int screenWidth, int screenHeight) {
    GLFONScontext* gl = (GLFONScontext*) ctx->params.userPtr;
    gl->screenSize[0] = screenWidth;
    gl->screenSize[1] = screenHeight;
    gl->resolutionDirty = true;
}

void glfons__createAtlas(void* usrPtr, unsigned int width, unsigned int height) {
    GLFONScontext* gl = (GLFONScontext*) usrPtr;

    glActiveTexture(GL_TEXTURE0 + ATLAS_TEXTURE_SLOT);
    glGenTextures(1, &gl->atlas);
    GLFONS_GL_CHECK(glBindTexture(GL_TEXTURE_2D, gl->atlas));
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

FONScontext* glfonsCreate(int width, int height, int flags, GLFONSparams glParams, void* userPtr) {
    FONSparams params;
    GLFONScontext* gl = new GLFONScontext;

    if(glParams.useGLBackend) {
        glParams.updateAtlas = glfons__udpateAtas;
        glParams.updateBuffer = glfons__updateBuffer;
        gl->userPtr = gl;
        glfons__initShaders(gl);
        glfons__createAtlas(gl, width, height);
    } else {
        glfons__createVertexLayout(gl);
        gl->userPtr = userPtr;
    }

    gl->params = glParams;

    params.width = width;
    params.height = height;
    params.flags = (unsigned char)flags;

    params.renderCreate = glfons__renderCreate;
    params.renderResize = glfons__renderResize;
    params.renderUpdate = glfons__renderUpdate;
    params.renderDraw = glfons__renderDraw;
    params.renderDelete = glfons__renderDelete;
    params.pushQuad = NULL;

    params.userPtr = gl;

    return fonsCreateInternal(&params);
}

void glfonsProjection(FONScontext* ctx, float* projectionMatrix) {
    GLFONScontext* gl = (GLFONScontext*) ctx->params.userPtr;

    glfons__projection(gl, projectionMatrix);
}

void glfonsDelete(FONScontext* ctx) {
    fonsDeleteInternal(ctx);
}

unsigned int glfonsRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    return (r) | (g << 8) | (b << 16) | (a << 24);
}

bool glfons__compareFlt(float a, float b) {
    return fabs(b - a) < 0.00001;
}

int glfonsVerticesSize(FONScontext* ctx) {
    GLFONS_LOAD_BUFFER
    return buffer->nVerts;
}

bool glfonsVertices(FONScontext* ctx, float* data) {
    GLFONS_LOAD_BUFFER
    memcpy(data, buffer->interleavedArray.data(), buffer->interleavedArray.size() * sizeof(float));
    return true;
}

void glfons__updateInterleavedArray(GLFONScontext* gl, GLFONSbuffer* buffer, GLFONSstash* stash, const char* attribute, int offset, float value) {
    int index = glfons__layoutIndex(gl, attribute);

    float* start = &buffer->interleavedArray[0] + stash->offset;
    int stride = gl->layout.nbComponents;

    if (stash->nbGlyph == 0) {
        return;
    }

    glfons__setDirty(buffer, stash->offset + index, stash->padding - gl->layout.nbComponents + index);

    for(int i = 0; i < stash->nbGlyph * GLYPH_VERTS; i++) {
        start[i * stride + index + offset] = value;
    }
}

void glfonsSetAlpha(FONScontext* ctx, fsuint id, float a) {
    GLFONS_LOAD_STASH
    glfons__updateInterleavedArray(gl, buffer, stash, "a_alpha", 0, a);
}

void glfonsRotate(FONScontext* ctx, fsuint id, float r) {
    GLFONS_LOAD_STASH
    glfons__updateInterleavedArray(gl, buffer, stash, "a_rotation", 0, r);
}

void glfonsTranslate(FONScontext* ctx, fsuint id, float tx, float ty) {
    GLFONS_LOAD_STASH
    glfons__updateInterleavedArray(gl, buffer, stash, "a_screenPosition", 0, tx);
    glfons__updateInterleavedArray(gl, buffer, stash, "a_screenPosition", 1, ty);
}

void glfonsTransform(FONScontext* ctx, fsuint id, float tx, float ty, float r, float a) {
    GLFONS_LOAD_STASH

    if (stash->nbGlyph == 0) {
        return;
    }

    // cache those indexes
    static int i0 = glfons__layoutIndex(gl, "a_screenPosition");
    static int i1 = glfons__layoutIndex(gl, "a_alpha");
    static int i2 = glfons__layoutIndex(gl, "a_rotation");

    float* start = &buffer->interleavedArray[0] + stash->offset;
    int stride = gl->layout.nbComponents;
    int min = std::min(i0, std::min(i1, i2));

    // set the buffer dirty
    glfons__setDirty(buffer, stash->offset + min, stash->padding - stride + min);

    for(int i = 0; i < stash->nbGlyph * GLYPH_VERTS; i++) {
        start[i * stride + i0] = tx;
        start[i * stride + i0 + 1] = ty;
        start[i * stride + i1] = a;
        start[i * stride + i2] = r;
    }
}

int glfonsGetGlyphCount(FONScontext* ctx, fsuint id) {
    GLFONS_LOAD_STASH
    return stash->nbGlyph;
}

void glfonsGetBBox(FONScontext* ctx, fsuint id, float* x0, float* y0, float* x1, float* y1) {
    GLFONS_LOAD_STASH
    *x0 = stash->bbox[0];
    *y0 = stash->bbox[1];
    *x1 = stash->bbox[2];
    *y1 = stash->bbox[3];
}

float glfonsGetLength(FONScontext* ctx, fsuint id) {
    GLFONS_LOAD_STASH
    return stash->length;
}

#endif