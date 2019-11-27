//分屏
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
void main()
{
    vec2 newTexCoord = v_texcoord;
    if(newTexCoord.x < 0.5)
    {
        newTexCoord.x = newTexCoord.x * 2.0;
    }
    else
    {
        newTexCoord.x = (newTexCoord.x - 0.5) * 2.0;
    }

    if(newTexCoord.y < 0.5)
    {
        newTexCoord.y = newTexCoord.y * 2.0;
    }
    else
    {
        newTexCoord.y = (newTexCoord.y - 0.5) * 2.0;
    }

    gl_FragColor = YuvToRgb(newTexCoord);
}