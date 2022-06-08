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
#ifdef GL_OES_standard_derivatives
#extension GL_OES_standard_derivatives : enable
#endif
    
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

float contour(in sampler2D tex, in vec2 uv, float w, in float off) {
    return contour(texture2D(tex, uv).a, w, off);
}

float contourAlpha(in sampler2D tex, in vec2 uv, float distance, in float off) {
    float alpha = contour(distance, distance, off);

#if defined(GL_OES_standard_derivatives) || !defined(GL_ES)
    float dscale = 0.354; // 1 / sqrt(2)
    vec2 duv = dscale * (dFdx(uv) + dFdy(uv));
    vec4 box = vec4(uv - duv, uv + duv);
    float asum = contour(tex, box.xy, distance, off)
               + contour(tex, box.zw, distance, off)
               + contour(tex, box.xw, distance, off)
               + contour(tex, box.zy, distance, off);

    alpha = (alpha + 0.5 * asum) / 2.0;
#endif

    return alpha;
}

void main(void) {
    if (v_alpha == 0.0) {
        discard;
    }

    float distance = texture2D(u_tex, v_uv).a;
    float alpha = contourAlpha(u_tex, v_uv, distance, sdf) * tint;
    alpha = pow(alpha, 1.0 / gamma);

    gl_FragColor = vec4(u_color, v_alpha * alpha);
}
)";

static const GLchar* vertexShaderSrc_300 = R"(#version 300 es

#ifdef GL_ES
precision mediump float;
#define LOWP lowp
#else
#define LOWP
#endif

uniform mat4 u_proj;

in vec2     a_position;
in vec2     a_screenPosition;
in vec2     a_uvs;
in float    a_alpha;
in float    a_rotation;

out vec2    v_uv;
out float   v_alpha;

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

static const GLchar* sdfFragShaderSrc_300 = R"(#version 300 es

#ifdef GL_ES
precision mediump float;
#define LOWP lowp
#else
#define LOWP
#endif

uniform sampler2D u_tex;
uniform LOWP vec3 u_color;

in vec2     v_uv;
in float    v_alpha;
out vec4    fragColor;

const float gamma = 2.2;
const float tint = 1.8;
const float sdf = 0.8;

float contour(in float d, in float w, in float off) {
    return smoothstep(off - w, off + w, d);
}

float contour(in sampler2D tex, in vec2 uv, float w, in float off) {
    return contour(texture(tex, uv).a, w, off);
}

float contourAlpha(in sampler2D tex, in vec2 uv, float distance, in float off) {
    float alpha = contour(distance, distance, off);

    float dscale = 0.354; // 1 / sqrt(2)
    vec2 duv = dscale * (dFdx(uv) + dFdy(uv));
    vec4 box = vec4(uv - duv, uv + duv);
    float asum = contour(tex, box.xy, distance, off)
               + contour(tex, box.zw, distance, off)
               + contour(tex, box.xw, distance, off)
               + contour(tex, box.zy, distance, off);

    alpha = (alpha + 0.5 * asum) / 2.0;

    return alpha;
}

void main(void) {
    if (v_alpha == 0.0) {
        discard;
    }

    float distance = texture(u_tex, v_uv).a;
    float alpha = contourAlpha(u_tex, v_uv, distance, sdf) * tint;
    alpha = pow(alpha, 1.0 / gamma);

    fragColor = vec4(u_color, v_alpha * alpha);
}
)";

}

#endif