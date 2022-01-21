//磨砂
#version 100
precision highp float;
varying vec2 v_texcoord;
uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureU;
uniform lowp sampler2D s_textureV;
uniform vec2 texSize;
uniform float u_offset;
uniform float factor;
uniform float blurSamplerScale;
const vec2 samplerSteps = vec2(1.0, 1.0);
uniform int samplerRadius;

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

const float arg = 0.5;

float random(vec2 seed)
{
    return fract(sin(dot(seed, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    vec4 centralColor = YuvToRgb(v_texcoord);
    float lum = dot(centralColor.rgb, vec3(0.299, 0.587, 0.114));
    float factor = (1.0 + arg) / (arg + lum) * factor;

    float gaussianWeightTotal = 1.0;
    vec4 sum = centralColor * gaussianWeightTotal;
    vec2 stepScale = blurSamplerScale * samplerSteps / float(samplerRadius);
    float offset = random(v_texcoord) - 0.5;

    for (int i = 1; i <= samplerRadius; ++i) {
        vec2 dis = (float(i) + offset) * stepScale;
        float percent = 1.0 - (float(i) + offset) / float(samplerRadius);

        vec4 sampleColor1 = YuvToRgb(v_texcoord + dis);
        float distanceFromCentralColor1 = min(distance(centralColor, sampleColor1) * factor, 1.0);
        float gaussianWeight1 = percent * (1.0 - distanceFromCentralColor1);
        gaussianWeightTotal += gaussianWeight1;
        sum += sampleColor1 * gaussianWeight1;

        vec4 sampleColor2 = YuvToRgb(v_texcoord - dis);
        float distanceFromCentralColor2 = min(distance(centralColor, sampleColor2) * factor, 1.0);
        float gaussianWeight2 = percent * (1.0 - distanceFromCentralColor2);
        gaussianWeightTotal += gaussianWeight2;
        sum += sampleColor2 * gaussianWeight2;
    }
    gl_FragColor = sum / gaussianWeightTotal;
}