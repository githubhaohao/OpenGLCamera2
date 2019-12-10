//分色偏移
#version 100
precision highp float;
varying vec2 v_texcoord;
uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureU;
uniform lowp sampler2D s_textureV;
uniform float u_offset;
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
void main()
{
    vec4 originColor = YuvToRgb(v_texcoord);
    vec4 offsetColor0 = YuvToRgb(vec2(v_texcoord.x + u_offset, v_texcoord.y + u_offset));
    vec4 offsetColor1 = YuvToRgb(vec2(v_texcoord.x - u_offset, v_texcoord.y - u_offset));

    gl_FragColor = vec4(originColor.r, offsetColor1.g, offsetColor0.b, originColor.a);
}