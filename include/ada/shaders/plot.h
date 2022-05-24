#pragma once

#include <string>

const std::string plot_frag = R"(
#ifdef GL_ES
precision mediump float;
#endif

#ifndef PLOT_DATA_TEXTURE
#define PLOT_DATA_TEXTURE u_plotData
#endif

#ifndef PLOT_DATA_PIXEL
#define PLOT_DATA_PIXEL 0.00390625
#endif

uniform sampler2D PLOT_DATA_TEXTURE;
uniform vec2 u_viewport;

varying vec2 v_texcoord;

/*
author: Patricio Gonzalez Vivo
description: Fix the aspect ratio of a space keeping things squared for you.
use: ratio(vec2 st, vec2 st_size)
license: |
  Copyright (c) 2017 Patricio Gonzalez Vivo.
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.    
*/

#ifndef FNC_RATIO
#define FNC_RATIO
vec2 ratio(in vec2 st, in vec2 s) {
    return mix( vec2((st.x*s.x/s.y)-(s.x*.5-s.y*.5)/s.y,st.y),
                vec2(st.x,st.y*(s.y/s.x)-(s.y*.5-s.x*.5)/s.x),
                step(s.x,s.y));
}
#endif

/*
author: Patricio Gonzalez Vivo
description: scale a 2D space variable
use: scale(<vec2> st, <vec2|float> scale_factor [, <vec2> center])
options:
    - CENTER
    - CENTER_2D
    - CENTER_3D
license: |
  Copyright (c) 2017 Patricio Gonzalez Vivo.
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.    
*/

#ifndef FNC_SCALE
#define FNC_SCALE
float scale(in float st, in float s, in float center) {
  return (st - center) * s + center;
}

float scale(in float st, in float s) {
  #ifdef CENTER_2D
  return scale(st,  s, CENTER);
  #else
  return scale(st,  s, .5);
  #endif
}


vec2 scale(in vec2 st, in vec2 s, in vec2 center) {
  return (st - center) * s + center;
}

vec2 scale(in vec2 st, in float value, in vec2 center) {
  return scale(st, vec2(value), center);
}

vec2 scale(in vec2 st, in vec2 s) {
  #ifdef CENTER_2D
  return scale(st,  s, CENTER_2D);
  #else
  return scale(st,  s, vec2(.5));
  #endif
}

vec2 scale(in vec2 st, in float value) {
  return scale(st, vec2(value));
}

vec3 scale(in vec3 st, in vec3 s, in vec3 center) {
  return (st - center) * s + center;
}

vec3 scale(in vec3 st, in float value, in vec3 center) {
  return scale(st, vec3(value), center);
}

vec3 scale(in vec3 st, in vec3 s) {
  #ifdef CENTER_3D
  return scale(st,  s, CENTER_3D);
  #else
  return scale(st,  s, vec3(.5));
  #endif
}

vec3 scale(in vec3 st, in float value) {
  return scale(st, vec3(value));
}
#endif


/*
author: Patricio Gonzalez Vivo
description: fill a stroke in a SDF. From PixelSpiritDeck https://github.com/patriciogonzalezvivo/PixelSpiritDeck
use: stroke(<float> sdf, <float> size, <float> width [, <float> edge])
license: |
  Copyright (c) 2017 Patricio Gonzalez Vivo. All rights reserved.
  Distributed under BSD 3-clause "New" or "Revised" License. See LICENSE file at
  https://github.com/patriciogonzalezvivo/PixelSpiritDeck
*/

#ifndef FNC_STROKE
#define FNC_STROKE
float stroke(float x, float size, float w) {
    float d = step(size, x+w*.5) - step(size, x-w*.5);
    return clamp(d, 0., 1.);
}
#endif

/*
author: Patricio Gonzalez Vivo
description: Flips the float passed in, 0 becomes 1 and 1 becomes 0
use: flip(<float> v, <float> pct)
license: |
  Copyright (c) 2017 Patricio Gonzalez Vivo. All rights reserved.
  Distributed under BSD 3-clause "New" or "Revised" License. See LICENSE file at
  https://github.com/patriciogonzalezvivo/PixelSpiritDeck
*/

#ifndef FNC_FLIP
#define FNC_FLIP
float flip(in float v, in float pct) {
    return mix(v, 1.0 - v, pct);
}

vec3 flip(in vec3 v, in float pct) {
    return mix(v, 1.0 - v, pct);
}

vec4 flip(in vec4 v, in float pct) {
    return mix(v, 1.0 - v, pct);
}
#endif


/*
author: Patricio Gonzalez Vivo
description: |
  Draws all the digits of a floating point number, useful for debugging.
  Requires high precision to work properly.
use: digits(<vec2> st, <float> value [, <float> nDecDigit])
options:
  DIGITS_DECIMALS: number of decimals after the point, defaults to 2
  DIGITS_SIZE: size of the font, defaults to vec2(.025)
*/

#ifndef DIGITS_SIZE
#define DIGITS_SIZE vec2(.025)
#endif

#ifndef DIGITS_DECIMALS
#define DIGITS_DECIMALS 2.0
#endif

#ifndef FNC_DIGITS
#define FNC_DIGITS
float digits(in vec2 st, in float value, in float nDecDigit) {
    st /= DIGITS_SIZE;

    float absValue = abs(value);
    float biggestDigitIndex = max(floor(log2(absValue) / log2(10.)), 0.);

    float counter = floor(value);
    float nIntDigits = 0.;
    for (int i = 0; i < 9; i++) {
        counter = floor(counter*.1);
        nIntDigits++;
        if (counter == 0.)
            break;
    }
    float digit = 12.;
    float digitIndex = (nIntDigits-1.) - floor(st.x);
    if (digitIndex > (-nDecDigit - 1.5)) {
        if (digitIndex > biggestDigitIndex) {
            if (value < 0.) {
                if (digitIndex < (biggestDigitIndex+1.5)) {
                    digit = 11.;
                }
            }
        } 
        else {
            if (digitIndex == -1.) {
                if (nDecDigit > 0.) {
                    digit = 10.;
                }
            } 
            else {
                if (digitIndex < 0.) {
                    digitIndex += 1.;
                }
                float digitValue = (absValue / (pow(10., digitIndex)));
                digit = mod(floor(0.0001+digitValue), 10.);
            }
        }
    }
    vec2 pos = vec2(fract(st.x), st.y);

    if (pos.x < 0.) return 0.;
    if (pos.y < 0.) return 0.;
    if (pos.x >= 1.) return 0.;
    if (pos.y >= 1.) return 0.;

    // make a 4x5 array of bits
    float bin = 0.;
    if (digit < 0.5) // 0
        bin = 7. + 5. * 16. + 5. * 256. + 5. * 4096. + 7. * 65536.; 
    else if (digit < 1.5) // 1
        bin = 2. + 2. * 16. + 2. * 256. + 2. * 4096. + 2. * 65536.;
    else if (digit < 2.5) // 2
        bin = 7. + 1. * 16. + 7. * 256. + 4. * 4096. + 7. * 65536.;
    else if (digit < 3.5) // 3
        bin = 7. + 4. * 16. + 7. * 256. + 4. * 4096. + 7. * 65536.;
    else if (digit < 4.5) // 4
        bin = 4. + 7. * 16. + 5. * 256. + 1. * 4096. + 1. * 65536.;
    else if (digit < 5.5) // 5
        bin = 7. + 4. * 16. + 7. * 256. + 1. * 4096. + 7. * 65536.;
    else if (digit < 6.5) // 6
        bin = 7. + 5. * 16. + 7. * 256. + 1. * 4096. + 7. * 65536.;
    else if (digit < 7.5) // 7
        bin = 4. + 4. * 16. + 4. * 256. + 4. * 4096. + 7. * 65536.;
    else if (digit < 8.5) // 8
        bin = 7. + 5. * 16. + 7. * 256. + 5. * 4096. + 7. * 65536.;
    else if (digit < 9.5) // 9
        bin = 7. + 4. * 16. + 7. * 256. + 5. * 4096. + 7. * 65536.;
    else if (digit < 10.5) // '.'
        bin = 2. + 0. * 16. + 0. * 256. + 0. * 4096. + 0. * 65536.;
    else if (digit < 11.5) // '-'
        bin = 0. + 0. * 16. + 7. * 256. + 0. * 4096. + 0. * 65536.;

    vec2 pixel = floor(pos * vec2(4., 5.));
    return mod(floor(bin / pow(2., (pixel.x + (pixel.y * 4.)))), 2.);
}

float digits(in vec2 st, in float value, in float nDecDigit, in float nIntDigits) {
    vec2 st2 = st;
    float result = 0.0;
    float dig = nDecDigit;

    #ifndef DIGITS_LEADING_INT
    #define DIGITS_LEADING_INT nIntDigits
    #endif

    for (float i = DIGITS_LEADING_INT - 1.0; i > 0.0 ; i--) {
        if (i * 10.0 > value) {
            result += digits(st2, 0.0, 0.0);
            st2.x -= DIGITS_SIZE.x;
        }
    }
    result += digits(st2, value, nDecDigit);
    return result; 
}

float digits(in vec2 st, in float value) {
    return digits(st, value, (DIGITS_DECIMALS));
}
#endif

void main() {
    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
    vec2 st = v_texcoord;

    vec4 data = texture2D(PLOT_DATA_TEXTURE, vec2(st.x, 0.5));

    color.r = step(st.y, data.r);
    color.g = step(st.y, data.g);
    color.b = step(st.y, data.b);

    #ifdef PLOT_VALUE
    vec4 value = texture2D(PLOT_DATA_TEXTURE, vec2(1.0-PLOT_DATA_PIXEL*0.5, 0.5));
    vec2 uv = ratio(st, u_viewport);
    PLOT_VALUE
    #endif

    gl_FragColor = color;
}
)";

const std::string plot_frag_300 = R"(
#ifdef GL_ES
precision mediump float;
#endif

#ifndef PLOT_DATA_TEXTURE
#define PLOT_DATA_TEXTURE u_plotData
#endif

uniform sampler2D   PLOT_DATA_TEXTURE;

in      vec2        v_texcoord;
out     vec4        fragColor;

/*
author: Patricio Gonzalez Vivo
description: fill a stroke in a SDF. From PixelSpiritDeck https://github.com/patriciogonzalezvivo/PixelSpiritDeck
use: stroke(<float> sdf, <float> size, <float> width [, <float> edge])
license: |
  Copyright (c) 2017 Patricio Gonzalez Vivo. All rights reserved.
  Distributed under BSD 3-clause "New" or "Revised" License. See LICENSE file at
  https://github.com/patriciogonzalezvivo/PixelSpiritDeck
*/

float stroke(float x, float size, float w) {
    float d = step(size, x+w*.5) - step(size, x-w*.5);
    return clamp(d, 0., 1.);
}

/*
author: Patricio Gonzalez Vivo
description: |
  Draws all the digits of a floating point number, useful for debugging.
  Requires high precision to work properly.
use: digits(<vec2> st, <float> value [, <float> nDecDigit])
options:
  DIGITS_DECIMALS: number of decimals after the point, defaults to 2
  DIGITS_SIZE: size of the font, defaults to vec2(.025)
*/

#ifndef DIGITS_SIZE
#define DIGITS_SIZE vec2(.025)
#endif

#ifndef DIGITS_DECIMALS
#define DIGITS_DECIMALS 2.0
#endif

#ifndef FNC_DIGITS
#define FNC_DIGITS
float digits(in vec2 st, in float value, in float nDecDigit) {
    st /= DIGITS_SIZE;

    float absValue = abs(value);
    float biggestDigitIndex = max(floor(log2(absValue) / log2(10.)), 0.);

    float counter = floor(value);
    float nIntDigits = 0.;
    for (int i = 0; i < 9; i++) {
        counter = floor(counter*.1);
        nIntDigits++;
        if (counter == 0.)
            break;
    }
    float digit = 12.;
    float digitIndex = (nIntDigits-1.) - floor(st.x);
    if (digitIndex > (-nDecDigit - 1.5)) {
        if (digitIndex > biggestDigitIndex) {
            if (value < 0.) {
                if (digitIndex < (biggestDigitIndex+1.5)) {
                    digit = 11.;
                }
            }
        } 
        else {
            if (digitIndex == -1.) {
                if (nDecDigit > 0.) {
                    digit = 10.;
                }
            } 
            else {
                if (digitIndex < 0.) {
                    digitIndex += 1.;
                }
                float digitValue = (absValue / (pow(10., digitIndex)));
                digit = mod(floor(0.0001+digitValue), 10.);
            }
        }
    }
    vec2 pos = vec2(fract(st.x), st.y);

    if (pos.x < 0.) return 0.;
    if (pos.y < 0.) return 0.;
    if (pos.x >= 1.) return 0.;
    if (pos.y >= 1.) return 0.;

    // make a 4x5 array of bits
    float bin = 0.;
    if (digit < 0.5) // 0
        bin = 7. + 5. * 16. + 5. * 256. + 5. * 4096. + 7. * 65536.; 
    else if (digit < 1.5) // 1
        bin = 2. + 2. * 16. + 2. * 256. + 2. * 4096. + 2. * 65536.;
    else if (digit < 2.5) // 2
        bin = 7. + 1. * 16. + 7. * 256. + 4. * 4096. + 7. * 65536.;
    else if (digit < 3.5) // 3
        bin = 7. + 4. * 16. + 7. * 256. + 4. * 4096. + 7. * 65536.;
    else if (digit < 4.5) // 4
        bin = 4. + 7. * 16. + 5. * 256. + 1. * 4096. + 1. * 65536.;
    else if (digit < 5.5) // 5
        bin = 7. + 4. * 16. + 7. * 256. + 1. * 4096. + 7. * 65536.;
    else if (digit < 6.5) // 6
        bin = 7. + 5. * 16. + 7. * 256. + 1. * 4096. + 7. * 65536.;
    else if (digit < 7.5) // 7
        bin = 4. + 4. * 16. + 4. * 256. + 4. * 4096. + 7. * 65536.;
    else if (digit < 8.5) // 8
        bin = 7. + 5. * 16. + 7. * 256. + 5. * 4096. + 7. * 65536.;
    else if (digit < 9.5) // 9
        bin = 7. + 4. * 16. + 7. * 256. + 5. * 4096. + 7. * 65536.;
    else if (digit < 10.5) // '.'
        bin = 2. + 0. * 16. + 0. * 256. + 0. * 4096. + 0. * 65536.;
    else if (digit < 11.5) // '-'
        bin = 0. + 0. * 16. + 7. * 256. + 0. * 4096. + 0. * 65536.;

    vec2 pixel = floor(pos * vec2(4., 5.));
    return mod(floor(bin / pow(2., (pixel.x + (pixel.y * 4.)))), 2.);
}

float digits(in vec2 st, in float value, in float nDecDigit, in float nIntDigits) {
    vec2 st2 = st;
    float result = 0.0;
    float dig = nDecDigit;

    #ifndef DIGITS_LEADING_INT
    #define DIGITS_LEADING_INT nIntDigits
    #endif

    for (float i = DIGITS_LEADING_INT - 1.0; i > 0.0 ; i--) {
        if (i * 10.0 > value) {
            result += digits(st2, 0.0, 0.0);
            st2.x -= DIGITS_SIZE.x;
        }
    }
    result += digits(st2, value, nDecDigit);
    return result; 
}

float digits(in vec2 st, in float value) {
    return digits(st, value, (DIGITS_DECIMALS));
}
#endif

void main() {
    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
    vec2 st = v_texcoord;

    vec4 data = texture(PLOT_DATA_TEXTURE, vec2(st.x, 0.5));

    color.r = step(st.y, data.r);
    color.g = step(st.y, data.g);
    color.b = step(st.y, data.b);

    #ifdef PLOT_VALUE
    vec4 value = texture(PLOT_DATA_TEXTURE, vec2(1.0-PLOT_DATA_PIXEL*0.5, 0.5));
    vec2 uv = ratio(st, u_viewport);
    PLOT_VALUE
    #endif

    fragColor = color;
}
)";
