#include "ada/tools/font.h"

#define GLFONTSTASH_IMPLEMENTATION
#include "glfontstash.h"

#include "ada/window.h"
#include "ada/tools/text.h"

#include "glm/gtc/type_ptr.hpp"

namespace ada { 

static FONScontext* fs = nullptr;
static size_t fn = 0;

Font::Font() : m_size(18.f), m_id(-1) {
    m_color = glfonsRGBA(255,255,255,255);
}

Font::Font(const std::string &_filepath, const std::string &_name) : m_size(10.f) {
    m_color = glfonsRGBA(255,255,255,255);
    load(_filepath, _name);
}

Font::~Font() {
    glfonsDelete(fs);
    fs = nullptr;
}

bool Font::load(const std::string &_filepath, std::string _name) {
    if (_name.size() == 0)
        _name = "font" + ada::toString(fn, 0, 3);

    if (fs == nullptr) {
        GLFONSparams params;
        params.useGLBackend = true; // if not set to true, you must provide your own gl backend
        // fs = glfonsCreate(512, 512, FONS_ZERO_TOPLEFT);
        fs = glfonsCreate(512, 512, FONS_ZERO_TOPLEFT | FONS_NORMALIZE_TEX_COORDS, params, nullptr);
    }

    m_id = fonsAddFont(fs, _name.c_str(), _filepath.c_str());

    return m_id >= 0;
}

void Font::setColor(const glm::vec4 & _color) {
    m_color = glfonsRGBA((char)(_color.r * 255), (char)(_color.g * 255), (char)(_color.b * 255), (char)(_color.a * 255));
}

// GLint Font::getAtlasTexture() {
// 	GLFONScontext* gl = (GLFONScontext*)(fs->params.userPtr);
// 	return Texture::create(gl->tex, gl->width, gl->height);
// }

glm::vec4 Font::getBoundingBox(const std::string &_text, float _x, float _y) {
    glm::vec4 bbox;

    fsuint textID = 0;
    fsuint buffer;

    fonsSetFont(fs, m_id);
    fonsSetSize(fs, m_size);
    fonsSetAlign(fs, m_align);

    // glfonsBufferCreate(fs, &buffer);
    // glfonsBindBuffer(fs, buffer);
    // glfonsGenText(fs, 1, &textID);
    // glfonsRasterize(fs, textID, _text.c_str());
    // glfonsGetBBox(fs, textID, &bbox[0], &bbox[1], &bbox[2], &bbox[3]);
    // glfonsBufferDelete(fs, buffer);
    // return bbox;

	fonsTextBounds(fs, _x, _y, _text.c_str(), NULL, glm::value_ptr( bbox ) );
    return bbox;
}

void Font::render(const std::string &_text, float _x, float _y) {
    if (m_id < 0)
        return;
    
    fonsSetFont(fs, m_id);
    fonsSetSize(fs, m_size * ada::getPixelDensity() );
    fonsSetColor(fs, m_color);
    fonsSetAlign(fs, m_align);

    glfonsScreenSize(fs, ada::getWindowWidth(), ada::getWindowHeight());

    fsuint textID = 0;
    fsuint buffer;
    glfonsBufferCreate(fs, &buffer);
    glfonsBindBuffer(fs, buffer);

    glfonsGenText(fs, 1, &textID);
    glfonsSetColor(fs, m_color);

    glfonsRasterize(fs, textID, _text.c_str());
    glfonsTransform(fs, textID, _x, _y, 0.0, 1.0);
    glfonsUpdateBuffer(fs);
    glfonsDraw(fs);
    glfonsBufferDelete(fs, buffer);

}


}

