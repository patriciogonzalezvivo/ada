#pragma once

#include "vbo.h"
#include "fbo.h"
#include "shader.h"

#include <functional>

// Reference:
// - Convolution Pyramids, Farbman et al., 2011 (https://www.cse.huji.ac.il/labs/cglab/projects/convpyr/data/convpyr-small.pdf)
// - Rendu (https://github.com/kosua20/Rendu)
// - ofxPoissonFill (https://github.com/LingDong-/ofxPoissonFill)
//

#define PYRAMID_MAX_LAYERS 12

namespace ada {

class Pyramid {
public:
    Pyramid();
    virtual ~Pyramid();

    void            allocate(int _width, int _height);
    bool            isAllocated() const {return m_depth != 0; }
    unsigned int    getDepth() const { return m_depth; }
    const Fbo*      getResult(unsigned int index = 0) const;
    virtual int     getWidth() const { return m_width; };
    virtual int     getHeight() const { return m_height; };

    void            process(const Fbo *_fbo);
    std::function<void(Fbo*,const Fbo*,const Fbo*, int)> pass;

    bool            fixed;
private:
    Fbo             m_downs[PYRAMID_MAX_LAYERS];
    Fbo             m_ups[PYRAMID_MAX_LAYERS];

    int             m_width;
    int             m_height;

    unsigned int    m_depth;
};

}
