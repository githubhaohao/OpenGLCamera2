#version 100
precision highp float;
varying vec2 v_texcoord;
uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureU;
uniform lowp sampler2D s_textureV;
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
void main() {
    float newY, newX;
    if(v_texcoord.y <= 1.0/3.0)
    {
        newY = v_texcoord.y + 1.0/3.0;
    }
    else if(1.0/3.0 <= v_texcoord.y && v_texcoord.y <= 2.0/3.0)
    {
        newY = v_texcoord.y;
    }
    else
    {
        newY = v_texcoord.y - 1.0/3.0;
    }

    if(v_texcoord.x <= 1.0/3.0)
    {
        newX = v_texcoord.x + 1.0/3.0;
    }
    else if(1.0/3.0 <= v_texcoord.x && v_texcoord.x <= 2.0/3.0)
    {
        newX = v_texcoord.x;
    }
    else
    {
        newX = v_texcoord.x - 1.0/3.0;
    }

    gl_FragColor = YuvToRgb(vec2(newX, newY));
}