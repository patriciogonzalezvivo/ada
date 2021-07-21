#include <iostream>

#include "ada/tools/pixels.h"
#include "ada/tools/fs.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

namespace ada {

unsigned char* loadPixels(const std::string& _path, int *_width, int *_height, Channels _channels, bool _vFlip) {
    stbi_set_flip_vertically_on_load(_vFlip);
    int comp;
    unsigned char* pixels = stbi_load(_path.c_str(), _width, _height, &comp, (_channels == RGB)? STBI_rgb : STBI_rgb_alpha);
    return pixels;
}

uint16_t* loadPixels16(const std::string& _path, int *_width, int *_height, Channels _channels, bool _vFlip) {
    stbi_set_flip_vertically_on_load(_vFlip);
    int comp;
    uint16_t *pixels = stbi_load_16(_path.c_str(), _width, _height, &comp, _channels);
    return pixels;
}

float* loadPixelsHDR(const std::string& _path, int *_width, int *_height, bool _vFlip) {
    stbi_set_flip_vertically_on_load(_vFlip);
    int comp;
    float* pixels = stbi_loadf(_path.c_str(), _width, _height, &comp, 0);
    return pixels;
}

bool savePixels(const std::string& _path, unsigned char* _pixels, int _width, int _height) {
    int saved = 0;
    int channels = 4;
    std::string ext = getExt(_path);

    // Flip the image on Y
    flipPixelsVertically<unsigned char>(_pixels, _width, _height, channels);

    if ( ext == "png") 
        saved = stbi_write_png(_path.c_str(), _width, _height, channels, _pixels, _width * channels);
    else if ( ext == "jpg")
        saved = stbi_write_jpg(_path.c_str(), _width, _height, channels, _pixels, 92);
    else if ( ext == "bmp")
        saved = stbi_write_bmp(_path.c_str(), _width, _height, channels, _pixels);
    else if ( ext == "tga")
        saved = stbi_write_tga(_path.c_str(), _width, _height, channels, _pixels);
    else if ( ext == "hdr") {
        size_t total = _width * _height * channels;
        const float m = 1.f / 255.f;
        float *float_pixels = new float[total];
        for (size_t i = 0; i < total; i++)
            float_pixels[i] = _pixels[i] * m;
        saved = stbi_write_hdr(_path.c_str(), _width, _height, channels, float_pixels);
        delete [] float_pixels;
    }

    if (0 == saved) {
        std::cout << "Can't create file " << _path << std::endl;
        return false;
    }
    return true;
}

bool savePixels16(const std::string& _path, unsigned short* _pixels, int _width, int _height) {
    int saved = 0;
    int channels = 4;
    std::string ext = getExt(_path);

    // Flip the image on Y
    flipPixelsVertically<unsigned short>(_pixels, _width, _height, channels);

    if ( ext == "png") 
        saved = stbi_write_png(_path.c_str(), _width, _height, channels, _pixels, _width * channels);
    else if ( ext == "jpg")
        saved = stbi_write_jpg(_path.c_str(), _width, _height, channels, _pixels, 92);
    else if ( ext == "bmp")
        saved = stbi_write_bmp(_path.c_str(), _width, _height, channels, _pixels);
    else if ( ext == "tga")
        saved = stbi_write_tga(_path.c_str(), _width, _height, channels, _pixels);
    else if ( ext == "hdr") {
        size_t total = _width * _height * channels;
        const float m = 1.f / 65535.f;
        float *float_pixels = new float[total];
        for (size_t i = 0; i < total; i++)
            float_pixels[i] = _pixels[i] * m;
        saved = stbi_write_hdr(_path.c_str(), _width, _height, channels, float_pixels);
        delete [] float_pixels;
    }

    if (0 == saved) {
        std::cout << "Can't create file " << _path << std::endl;
        return false;
    }
    return true;
}

bool savePixelsHDR(const std::string& _path, float* _pixels, int _width, int _height) {
    int channels = 4;

    // Flip the image on Y
    flipPixelsVertically<float>(_pixels, _width, _height, channels);
    
    if (0 == stbi_write_hdr(_path.c_str(), _width, _height, channels, _pixels)) {
        std::cout << "Can't create file " << _path << std::endl;
        return false;
    }
    return true;
}

void freePixels(void *pixels) {
    stbi_image_free(pixels);
}

}