//
//  shaders.h
//  FontstashiOS
//
//  Created by Karim Naaji on 12/16/14.
//  Copyright (c) 2014 Mapzen. All rights reserved.
//

#ifndef SHADERS_H
#define SHADERS_H

#include "ada/gl/gl.h"

namespace glfs {

static const GLchar* vertexShaderSrc = R"(
#ifdef GL_ES
precision mediump float;
#define LOWP lowp
#else
#define LOWP
#endif

attribute vec2 a_position;
attribute vec2 a_screenPosition;
attribute vec2 a_uvs;
attribute float a_alpha;
attribute float a_rotation;

uniform mat4 u_proj;

varying vec2 v_uv;
varying float v_alpha;

void main() {
    if (a_alpha != 0.0) {
        float st = sin(a_rotation);
        float ct = cos(a_rotation);

        // rotates first around +z-axis (0,0,1) and then translates by (tx,ty,0)
        vec4 p = vec4(
            a_position.x * ct - a_position.y * st + a_screenPosition.x,
            a_position.x * st + a_position.y * ct + a_screenPosition.y,
            0.0, 1.0
        );

        gl_Position = u_proj * p;
    } else {
        gl_Position = vec4(0.0);
    }
    
    v_uv = a_uvs;
    v_alpha = a_alpha;
}

)";

static const GLchar* sdfFragShaderSrc = R"(
#extension GL_OES_standard_derivatives : enable
    
#ifdef GL_ES
precision mediump float;
#define LOWP lowp
#else
#define LOWP
#endif

uniform sampler2D u_tex;
uniform LOWP vec3 u_color;

varying vec2 v_uv;
varying float v_alpha;

const float gamma = 2.2;
const float tint = 1.8;
const float sdf = 0.8;

float contour(in float d, in float w, in float off) {
    return smoothstep(off - w, off + w, d);
}

float sample(in vec2 uv, float w, in float off) {
    return contour(texture2D(u_tex, uv).a, w, off);
}

float sampleAlpha(in vec2 uv, float distance, in float off) {
    float alpha = contour(distance, distance, off);

#ifdef GL_OES_standard_derivatives
    float dscale = 0.354; // 1 / sqrt(2)
    vec2 duv = dscale * (dFdx(uv) + dFdy(uv));
    vec4 box = vec4(uv - duv, uv + duv);

    float asum = sample(box.xy, distance, off)
               + sample(box.zw, distance, off)
               + sample(box.xw, distance, off)
               + sample(box.zy, distance, off);

    alpha = (alpha + 0.5 * asum) / 2.0;
#endif

    return alpha;
}

void main(void) {
    if (v_alpha == 0.0) {
        discard;
    }

    float distance = texture2D(u_tex, v_uv).a;
    float alpha = sampleAlpha(v_uv, distance, sdf) * tint;
    alpha = pow(alpha, 1.0 / gamma);

    gl_FragColor = vec4(u_color, v_alpha * alpha);
}

)";

static const GLchar* defaultFragShaderSrc = R"(

#ifdef GL_ES
precision mediump float;
#define LOWP lowp
#else
#define LOWP 
#endif

uniform sampler2D u_tex;
uniform LOWP vec3 u_color;

varying vec2 v_uv;
varying float v_alpha;

void main(void) {
    if (v_alpha == 0.0) {
        discard;
    }
    
    vec4 texColor = texture2D(u_tex, v_uv);
    gl_FragColor = vec4(u_color.rgb, texColor.a * v_alpha);
}

)";
    
}

#endif