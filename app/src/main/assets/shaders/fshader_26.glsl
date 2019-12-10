//灵魂出窍
#version 100
precision highp float;
varying vec2 v_texcoord;
uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureU;
uniform lowp sampler2D s_textureV;
uniform float u_offset;
uniform vec2 texSize;

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
const float MAX_ALPHA = 0.5;
const float MAX_SCALE = 0.8;
void main()
{
    float alpha = MAX_ALPHA * (1.0 - u_offset);
    float scale = 1.0 + u_offset * MAX_SCALE;

    float scale_x = 0.5 + (v_texcoord.x - 0.5) / scale;
    float scale_y = 0.5 + (v_texcoord.y - 0.5) / scale;

    vec2 scaleCoord = vec2(scale_x, scale_y);
    vec4 maskColor = YuvToRgb(scaleCoord);

    vec4 originColor = YuvToRgb(v_texcoord);

    gl_FragColor = originColor * (1.0 - alpha) + maskColor * alpha;
}