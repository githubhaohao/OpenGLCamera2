//随机马赛克
#version 100
precision highp float;
varying vec2 v_texcoord;
uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureU;
uniform lowp sampler2D s_textureV;
uniform float u_time;
uniform vec2 texSize;

float rand(float n) {
    // fract(x) 返回x的小数部分数值
    return fract(sin(n) * 50000.0);
}

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

const int MAX_COUNT = 5;
void main()
{
    float MAX_LEN = (texSize.x < texSize.y ? texSize.y : texSize.x) / 10.0;
    vec2 points[MAX_COUNT];
    points[0].x = rand(u_time);
    points[0].y = rand(points[0].x);
    for(int i=1; i<MAX_COUNT; i++)
    {
        points[i].x = rand(points[i-1].y);
        points[i].y = rand(points[i].x);
    }

    bool doMosic = false;
    vec2 imgtexcoord = v_texcoord * texSize;
    int index = 0;
    for(int i = 0; i<MAX_COUNT; i++)
    {
        vec2 imgMosicPoint = points[i] * texSize;
        float len = rand(imgMosicPoint.x) * MAX_LEN;

        if((imgMosicPoint.x - len)< imgtexcoord.x && imgtexcoord.x < (imgMosicPoint.x + len)
            && (imgMosicPoint.y - len)< imgtexcoord.y && imgtexcoord.y < (imgMosicPoint.y + len))
        {
            index = i;
            doMosic = true;
            break;
        }
    }

    if(doMosic)
    {
        gl_FragColor = YuvToRgb(points[index]);
    }
    else
    {
        gl_FragColor = YuvToRgb(v_texcoord);
    }
}