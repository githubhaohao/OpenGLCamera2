//旋转的圆
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
const float PI = 3.141592653;
void main()
{
    vec2 imgTex = v_texcoord * texSize;
    float r = 0.3 * texSize.x;
    if(distance(imgTex, vec2(texSize.x / 2.0, texSize.y / 2.0)) < r)
    {
        vec2 tranTex = v_texcoord - 0.5;
        vec2 imgTranTex = tranTex * texSize;
        float len = length(imgTranTex);
        float angle = 0.0;

        angle = acos(imgTranTex.x / len);

        if(tranTex.y < 0.0)
        {
            angle *= -1.0;
        }

        angle -= u_offset;

        imgTranTex.x = len * cos(angle);
        imgTranTex.y = len * sin(angle);

        vec2 newTexCoors = imgTranTex / texSize + 0.5;

        gl_FragColor = YuvToRgb(newTexCoors);
    }
    else
    {
        gl_FragColor = YuvToRgb(v_texcoord);
    }
}