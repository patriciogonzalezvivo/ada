#pragma once

#include <string>

const std::string histogram_frag = R"(
#ifdef GL_ES
precision mediump float;
#endif

#ifndef PLOT_DATA_TEXTURE
#define PLOT_DATA_TEXTURE u_plotData
#endif

uniform sampler2D PLOT_DATA_TEXTURE;

varying vec2 v_texcoord;

float stroke(float x, float size, float w) {
    float d = step(size, x+w*.5) - step(size, x-w*.5);
    return clamp(d, 0., 1.);
}

void main() {
    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
    vec2 st = v_texcoord;

    vec4 freqs = texture2D(PLOT_DATA_TEXTURE, vec2(st.x, 0.5));

    color.r = step(st.y, freqs.r);
    color.g = step(st.y, freqs.g);
    color.b = step(st.y, freqs.b);
    // color += stroke(freqs.a, st.y, 0.1);

    gl_FragColor = color;
}
)";

const std::string histogram_frag_300 = R"(
#ifdef GL_ES
precision mediump float;
#endif

#ifndef PLOT_DATA_TEXTURE
#define PLOT_DATA_TEXTURE u_plotData
#endif

uniform sampler2D   PLOT_DATA_TEXTURE;

in      vec2        v_texcoord;
out     vec4        fragColor;

float stroke(float x, float size, float w) {
    float d = step(size, x+w*.5) - step(size, x-w*.5);
    return clamp(d, 0., 1.);
}

void main() {
    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
    vec2 st = v_texcoord;

    vec4 freqs = texture(PLOT_DATA_TEXTURE, vec2(st.x, 0.5));

    color.r = step(st.y, freqs.r);
    color.g = step(st.y, freqs.g);
    color.b = step(st.y, freqs.b);
    // color += stroke(freqs.a, st.y, 0.1);

    fragColor = color;
}
)";
