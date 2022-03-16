//shake 抖动
#version 100
precision highp float;
varying vec2 v_texcoord;
uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureU;
uniform lowp sampler2D s_textureV;
uniform float u_offset;
const float SCALE = 8.0;

vec4 textureYUV(vec2 texCoord) {
    float y, u, v, r, g, b;
    y = texture2D(s_textureY, texCoord).r;
    u = texture2D(s_textureU, texCoord).r;
    v = texture2D(s_textureV, texCoord).r;
    u = u - 0.5;
    v = v - 0.5;
    r = y + 1.403 * v;
    g = y - 0.344 * u - 0.714 * v;
    b = y + 1.770 * u;
    return vec4(r, g, b, 1.0);
}

void main()
{
    vec2 id = floor(v_texcoord * SCALE);
    vec2 offs = vec2(sin(v_texcoord.y * SCALE + sin(u_offset * 3.0) * 20.0 + id.y * 0.2),
    sin(v_texcoord.x * SCALE + sin(u_offset * 3.0) * 20.0 + id.x * 0.2));

    offs *= 0.01;
    vec2 texCoord = v_texcoord + offs ;

    gl_FragColor = textureYUV(texCoord);
}