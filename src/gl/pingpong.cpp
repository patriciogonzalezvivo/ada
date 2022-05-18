#include <iostream>

#include "ada/gl/pingpong.h"

namespace ada {

PingPong::PingPong(): 
    src(nullptr), dst(nullptr), 
    m_flag(0) {
}

PingPong::~PingPong() {
}

void PingPong::allocate(int _width, int _height, FboType _type, TextureFilter _filter, TextureWrap _wrap, bool _autoclear) {
    for(int i = 0; i < 2; i++)
        m_fbos[i].allocate(_width, _height, _type, _filter, _wrap, _autoclear);

    clear();

    // Set everything to 0
    m_flag = 0;
    swap();
}

void PingPong::swap(){
    src = &(m_fbos[(m_flag)%2]);
    dst = &(m_fbos[++(m_flag)%2]);
}

void PingPong::clear(float _alpha) {
    for(int i = 0; i < 2; i++){
        m_fbos[i].bind();
        glClearColor(0.0f, 0.0f, 0.0f, _alpha);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_fbos[i].unbind();
    }
}

}