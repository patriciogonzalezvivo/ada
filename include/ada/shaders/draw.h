#pragma once

#include <string>


const std::string points_vert = R"(#version 120
#ifdef GL_ES
precision mediump float;
#endif

uniform mat4    u_modelViewProjectionMatrix;
uniform float   u_size;
attribute vec4  a_position;

void main(void) {
    gl_PointSize = u_size;
    gl_Position = u_modelViewProjectionMatrix * a_position;
}
)";

const std::string points_frag = R"(#version 120

#ifdef GL_OES_standard_derivatives
#extension GL_OES_standard_derivatives : enable
#endif

#ifdef GL_ES
precision mediump float;
#endif

uniform vec4 u_color;
uniform int  u_shape;

float aastep(float threshold, float value) {
    #ifdef GL_OES_standard_derivatives
    float afwidth = 0.7 * length(vec2(dFdx(value), dFdy(value)));
    return smoothstep(threshold-afwidth, threshold+afwidth, value);
    #elif defined(AA_EDGE)
    float afwidth = AA_EDGE;
    return smoothstep(threshold-afwidth, threshold+afwidth, value);
    #else 
    return step(threshold, value);
    #endif
}

#define saturate(x) clamp(x, 0.0, 1.0)

float fill(float x, float size) {
    return 1.0 - aastep(size, x);
}

float fill(float x, float size, float edge) {
    return 1.0 - smoothstep(size - edge, size + edge, x);
}

float stroke(float x, float size, float w) {
    float d = aastep(size, x + w * 0.5) - aastep(size, x - w * 0.5);
    return saturate(d);
}

float rectSDF(in vec2 st, in vec2 s) {
    st = st * 2. - 1.;
    return max( abs(st.x / s.x),
                abs(st.y / s.y) );
}

float crossSDF(in vec2 st, in float s) {
    vec2 size = vec2(.25, s);
    return min(rectSDF(st.xy, size.xy),
               rectSDF(st.xy, size.yx));
}

void main(void) {
    vec4 color = u_color;

#if !defined(GL_ES)

    if (u_shape == 1) {
        vec2 uv = gl_PointCoord.xy;
        float sdf = rectSDF(uv, vec2(1.0));
        color.a = stroke(sdf, 0.75, 0.5);
    }

    else if (u_shape == 2) {
        vec2 uv = gl_PointCoord.xy;
        float sdf = length(uv - 0.5);
        color.a = fill( sdf, 0.5 );
    }

    else if (u_shape == 3) {
        vec2 uv = gl_PointCoord.xy;
        float sdf = length(uv - 0.5) * 2.0;
        color.a = stroke( sdf, 0.75, 0.5 );
    }

    else if (u_shape == 4) {
        vec2 uv = gl_PointCoord.xy;
        color.a = fill( crossSDF(uv, 1.0), 1.0 );
    }

    else if (u_shape == 5) {
        vec2 uv = gl_PointCoord.xy;

        float sdf = .5+(uv.x-uv.y)*.5;
        float sdf_inv = (uv.x+uv.y)*.5;
        color.a =   stroke(sdf,.5,.1) + 
                    stroke(sdf_inv,.5,.1);
    }
    
#endif

    gl_FragColor = color;
}
)";

const std::string points_vert_300 = R"(
#ifdef GL_ES
precision mediump float;
#endif

uniform mat4    u_modelViewProjectionMatrix;
uniform float   u_size;
in      vec4    a_position;

void main(void) {
    gl_PointSize = u_size;
    gl_Position = u_modelViewProjectionMatrix * a_position;
}
)";

const std::string points_frag_300 = R"(
#ifdef GL_ES
precision mediump float;
#endif

uniform vec4    u_color;
out     vec4    fragColor;

#ifndef SHAPE
#define SHAPE 0
#endifs

void main(void) {
    vec4 color = u_color;
    fragColor = color;
}
)";

const std::string fill_vert = R"(
#ifdef GL_ES
precision mediump float;
#endif

uniform mat4    u_modelViewProjectionMatrix;

attribute vec4  a_position;
varying vec4    v_position;

#ifdef MODEL_VERTEX_COLOR
attribute vec4  a_color;
varying vec4    v_color;
#endif

#ifdef MODEL_VERTEX_NORMAL
attribute vec3  a_normal;
varying vec3    v_normal;
#endif

#ifdef MODEL_VERTEX_TEXCOORD
attribute vec2  a_texcoord;
varying vec2    v_texcoord;
#endif

void main(void) {
    v_position = a_position;
    
#ifdef MODEL_VERTEX_COLOR
    v_color = a_color;
#endif
    
#ifdef MODEL_VERTEX_NORMAL
    v_normal = a_normal;
#endif
    
#ifdef MODEL_VERTEX_TEXCOORD
    v_texcoord = a_texcoord;
#endif
    
    gl_Position = u_modelViewProjectionMatrix * v_position;
}
)";

const std::string fill_frag = R"(
#ifdef GL_ES
precision mediump float;
#endif

#ifdef HAVE_TEXTURE
uniform sampler2D u_tex0;
#endif

uniform vec4    u_color;

#ifdef MODEL_VERTEX_COLOR
varying vec4    v_color;
#endif

#ifdef MODEL_VERTEX_NORMAL
varying vec3    v_normal;
#endif

#ifdef MODEL_VERTEX_TEXCOORD
varying vec2    v_texcoord;
#endif

void main(void) {
    vec4 color = u_color;

#ifdef MODEL_VERTEX_COLOR
    color = v_color;
#endif

#if defined(HAVE_TEXTURE) && defined(MODEL_VERTEX_TEXCOORD)
    color = texture2D(u_tex0, v_texcoord);
#endif

    gl_FragColor = color;
}
)";

const std::string fill_vert_300 = R"(
#ifdef GL_ES
precision mediump float;
#endif

uniform mat4    u_modelViewProjectionMatrix;
in      vec4    a_position;

void main(void) {
    gl_Position = u_modelViewProjectionMatrix * a_position;
}
)";

const std::string fill_frag_300 = R"(
#ifdef GL_ES
precision mediump float;
#endif

uniform vec4    u_color;
out     vec4    fragColor;

void main(void) {
    fragColor = u_color;
}
)";

