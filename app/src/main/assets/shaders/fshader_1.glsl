#version 100
precision highp float;
varying vec2 v_texcoord;
uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureU;
uniform lowp sampler2D s_textureV;
void main() {
    float y, u, v, r, g, b;
    y = texture2D(s_textureY, v_texcoord).r;
    u = texture2D(s_textureU, v_texcoord).r;
    v = texture2D(s_textureV, v_texcoord).r;
    u = u - 0.5;
    v = v - 0.5;
    r = y + 1.403 * v;
    g = y - 0.344 * u - 0.714 * v;
    b = y + 1.770 * u;
    gl_FragColor = vec4(r, g, b, 1.0);
}