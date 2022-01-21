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