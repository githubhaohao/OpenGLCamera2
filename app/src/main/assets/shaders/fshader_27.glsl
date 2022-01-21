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
    //      YUV转RGB
    //      R = Y + 1.402 * (V - 128)
    //      G = Y - 0.34414 * (U - 128) - 0.71414 * (V - 128)
    //      B = Y + 1.772 * (U - 128)

    vec3 yuv;
    // 只赋值x 则会出现黑白照片效果
    yuv.x = texture2D(s_textureY, uv).r;
    // 因为YUV转RGB用的是Y、U-128和V-128，texture函数返回向量值的范围是0-1.0，128表示0.5, 所以要减去0.5
    yuv.y = texture2D(s_textureU, uv).r - 0.5;
    yuv.z = texture2D(s_textureV, uv).r - 0.5;

    //  YUV 和 RGB 的转换矩阵
    highp vec3 rgb =  mat3(
    1.0, 1.0, 1.0, //第一列
    0.0, -0.34414, 1.772, //第二列
    1.403, -0.71414, 0.0//第三列
    ) * yuv;
    return vec4(rgb, 1.0);
}

const int MAX_COUNT = 5;

void main()
{
    float MAX_LEN = (texSize.x < texSize.y ? texSize.y : texSize.x) / 10.0;
    vec2 points[MAX_COUNT];
    points[0].x = rand(u_time);
    points[0].y = rand(points[0].x);

    for (int i=1; i<MAX_COUNT; i++)
    {
        points[i].x = rand(points[i-1].y);
        points[i].y = rand(points[i].x);
    }

    bool doMosic = false;
    vec2 imgtexcoord = v_texcoord * texSize;
    int index = 0;

    for (int i = 0; i<MAX_COUNT; i++)
    {
        vec2 imgMosicPoint = points[i] * texSize;
        float len = rand(imgMosicPoint.x) * MAX_LEN;

        if ((imgMosicPoint.x - len)< imgtexcoord.x && imgtexcoord.x < (imgMosicPoint.x + len)
        && (imgMosicPoint.y - len)< imgtexcoord.y && imgtexcoord.y < (imgMosicPoint.y + len))
        {
            index = i;
            doMosic = true;
            break;
        }
    }

    if (doMosic){
        gl_FragColor = YuvToRgb(points[index]);
    } else {
        gl_FragColor = YuvToRgb(v_texcoord);
    }
}