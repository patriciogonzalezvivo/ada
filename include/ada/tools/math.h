#pragma once

#include <math.h>

namespace ada {

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif 

#ifndef M_2_SQRTPI
#define M_2_SQRTPI 1.12837916709551257390
#endif

// #ifndef PI
// #define PI 3.14159265358979323846
// #endif 

// #ifndef TWO_PI
// #define TWO_PI   6.28318530717958647693
// #endif

// #ifndef FOUR_PI
// #define FOUR_PI 12.56637061435917295385
// #endif

// #ifndef HALF_PI
// #define HALF_PI  1.57079632679489661923
// #endif

// #ifndef QUARTER_PI
// #define QUARTER_PI 0.785398163
// #endif

// #ifndef FLT_EPSILON
// #define FLT_EPSILON 1.19209290E-07F
// #endif

// #ifndef MIN
// #define MIN(x,y) (((x) < (y)) ? (x) : (y))
// #endif

// #ifndef MAX
// #define MAX(x,y) (((x) > (y)) ? (x) : (y))
// #endif

// #ifndef CLAMP
// #define CLAMP(val,min,max) ((val) < (min) ? (min) : ((val > max) ? (max) : (val)))
// #endif

// #ifndef ABS
// #define ABS(x) (((x) < 0) ? -(x) : (x))
// #endif

#ifndef M_MIN
#define M_MIN(_a, _b) ((_a)<(_b)?(_a):(_b))
#endif


static float angleBetween(float thetav, float phiv, float theta, float phi) {
    float cosGamma = sinf(thetav) * sinf(theta) * cosf(phi - phiv) + cosf(thetav) * cosf(theta);
    return acosf(cosGamma);
}


float clamp ( float value , float min , float max ) {
    if (value < min)
        return min;
    if (value > max)
        return max;

    return value;
}

}