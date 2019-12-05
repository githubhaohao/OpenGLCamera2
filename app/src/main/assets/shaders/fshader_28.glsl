//画中画
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

vec2 scale(vec2 uv, float level)
{
    vec2 center = vec2(0.5, 0.5);
    vec2 newTexCoord = uv.xy;
    newTexCoord -= center;
    newTexCoord = newTexCoord / level;
    newTexCoord += center;
    return newTexCoord;
}

const float OFFSET_LEVEL = 0.15;
const float SCALE_LEVEL = 4.0;
void main()
{

    if(OFFSET_LEVEL < v_texcoord.x && v_texcoord.x < (1.0 - OFFSET_LEVEL)
       && OFFSET_LEVEL < v_texcoord.y && v_texcoord.y < (1.0 - OFFSET_LEVEL))
    {
        vec2 newTexCoord = v_texcoord;
        newTexCoord -= OFFSET_LEVEL;
        newTexCoord = newTexCoord / (1.0 - 2.0 * OFFSET_LEVEL);
        gl_FragColor = YuvToRgb(newTexCoord);
    }
    else
    {
        gl_FragColor = YuvToRgb(scale(v_texcoord, SCALE_LEVEL));
    }
}