#version 100
precision highp float;
varying vec2 v_texcoord;
uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureU;
uniform lowp sampler2D s_textureV;
vec4 YuvToRgb(vec2 uv) {
    float y, u, v, r, g, b;
    y = texture2D(s_textureY, uv).r;
    u = texture2D(s_textureU, uv).r;
    v = texture2D(s_textureV, uv).r;
    u = u - 0.5;
    v = v - 0.5;
    r = y + 1.403 * v;
    g = y - 0.344 * u - 0.714 * v;
    b = y + 1.770 * u;
    return vec4(r, g, b, 1.0);
}
void main() {
    vec4 sample0, sample1, sample2, sample3;
    float blurStep = 0.5;
    float step = blurStep / 100.0;
    sample0 = YuvToRgb(vec2(v_texcoord.x - step, v_texcoord.y - step));
    sample1 = YuvToRgb(vec2(v_texcoord.x + step, v_texcoord.y + step));
    sample2 = YuvToRgb(vec2(v_texcoord.x + step, v_texcoord.y - step));
    sample3 = YuvToRgb(vec2(v_texcoord.x - step, v_texcoord.y + step));
    gl_FragColor = (sample0 + sample1 + sample2 + sample3) / 4.0;
}