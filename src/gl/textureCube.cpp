#include <iostream>
#include <cstring>

#include "ada/gl/textureCube.h"

#include "ada/fs.h"
#include "ada/math.h"
#include "ada/pixel.h"

namespace ada {

TextureCube::TextureCube() 
    : SH {  glm::vec3(0.0), glm::vec3(0.0), glm::vec3(0.0),
            glm::vec3(0.0), glm::vec3(0.0), glm::vec3(0.0),
            glm::vec3(0.0), glm::vec3(0.0), glm::vec3(0.0) } {
}

TextureCube::~TextureCube() {
    glDeleteTextures(1, &m_id);
}

bool TextureCube::load(const std::string &_path, bool _vFlip) {
    std::string ext = getExt(_path);

    if (m_id == 0)
        glGenTextures(1, &m_id);
        
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

    int sh_samples = 0;
    if (ext == "png"    || ext == "PNG" ||
        ext == "jpg"    || ext == "JPG" ||
        ext == "jpeg"   || ext == "JPEG" ) {

        unsigned char* data = loadPixels(_path, &m_width, &m_height, RGB, false);

        // LOAD FACES
        CubemapFace<unsigned char> **faces = new CubemapFace<unsigned char>*[6];

        if (m_height > m_width) {

            if (m_width/6 == m_height) {
                // Vertical Row
                splitFacesFromVerticalRow<unsigned char>(data, m_width, m_height, faces);
            }
            else {
                // Vertical Cross
                splitFacesFromVerticalCross<unsigned char>(data, m_width, m_height, faces);

                // adjust NEG_Z face
                if (_vFlip) {
                    faces[5]->flipHorizontal();
                    faces[5]->flipVertical();
                }
            }
            
        }
        else {
            if (m_width/2 == m_height) {
                // Equilateral
                splitFacesFromEquirectangular<unsigned char>(data, m_width, m_height, faces);
            }
            else if (m_width/6 == m_height) {
                // Horizontal Row
                splitFacesFromHorizontalRow<unsigned char>(data, m_width, m_height, faces);
            }
            else {
                // Horizontal Cross
                splitFacesFromHorizontalCross<unsigned char>(data, m_width, m_height, faces);
            }
        }
        
        for (int i = 0; i < 6; i++) {
            faces[i]->upload();
            sh_samples += faces[i]->calculateSH(SH);
        }

        delete[] data;
        for(int i = 0; i < 6; ++i) {
            delete[] faces[i]->data;
            delete faces[i];
        }
        delete[] faces;

    }

    else if (ext == "hdr" || ext == "HDR") {
        float* data = loadPixelsHDR(_path, &m_width, &m_height, false);

        // LOAD FACES
        CubemapFace<float> **faces = new CubemapFace<float>*[6];

        if (m_height > m_width) {
            if (m_width/6 == m_height) {
                // Vertical Row
                splitFacesFromVerticalRow<float>(data, m_width, m_height, faces);
            }
            else {
                // Vertical Cross
                splitFacesFromVerticalCross<float>(data, m_width, m_height, faces);

                // adjust NEG_Z face
                if (_vFlip) {
                    faces[5]->flipHorizontal();
                    faces[5]->flipVertical();
                }
            }
        }
        else {

            if (m_width/2 == m_height)  {
                // Equilatera
                splitFacesFromEquirectangular<float>(data, m_width, m_height, faces);
            }
            else if (m_width/6 == m_height) {
                // Horizontal Row
                splitFacesFromHorizontalRow<float>(data, m_width, m_height, faces);
            }
            else {
                // Horizontal Cross
                splitFacesFromHorizontalCross<float>(data, m_width, m_height, faces);
            }
        }

        for (int i = 0; i < 6; i++) {
            faces[i]->upload();
            sh_samples += faces[i]->calculateSH(SH);
        }

        delete[] data;
        for(int i = 0; i < 6; ++i) {
            delete[] faces[i]->data;
            delete faces[i];
        }
        delete[] faces;

    }

    for (int i = 0; i < 9; i++) {
        SH[i] = SH[i] * (32.0f / (float)sh_samples);
    }

#if defined(PLATFORM_RPI) || defined(DRIVER_GBM)
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#else
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
#endif
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    m_path = _path;
    m_vFlip = _vFlip;
    
    return true;
}

bool TextureCube::load(SkyData* _sky, int _width) {

    if (m_id == 0)
        glGenTextures(1, &m_id);

    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

    int sh_samples = 0;

    m_width = _width;
    m_height = int(_width/2);

    // float *data = equirectangularSkyBox(_sky, m_width, m_height);

    // // LOAD FACES
    // CubemapFace<float> **faces = new CubemapFace<float>*[6];
    // splitFacesFromEquirectangular<float>(data, m_width, m_height, faces);

    CubemapFace<float> **faces = skyCubemap(_sky, m_width);
    
    for (int i = 0; i < 6; i++) {
        faces[i]->upload();
        sh_samples += faces[i]->calculateSH(SH);
    }

    for(int i = 0; i < 6; ++i) {
        delete[] faces[i]->data;
        delete faces[i];
    }
    delete[] faces;
    // delete[] data;

    for (int i = 0; i < 9; i++)
        SH[i] = SH[i] * (32.0f / (float)sh_samples);

#if defined(PLATFORM_RPI) || defined(DRIVER_GBM)
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#else
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
#endif

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return true;
}

void TextureCube::bind() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
}

}