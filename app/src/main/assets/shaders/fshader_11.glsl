#version 100

precision highp float;
varying vec2 v_texcoord;
uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureU;
uniform lowp sampler2D s_textureV;

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

void main() {
    vec3 color = YuvToRgb(v_texcoord).rgb;
    vec2 uv = v_texcoord.xy;
    vec3 colors[3];
    colors[0] = vec3(0.,0.,1.);
    colors[1] = vec3(1.,1.,0.);
    colors[2] = vec3(1.,0.,0.);
    float lum = (color.r + color.g + color.b)/3.;
    int idx = (lum < 0.5) ? 0 : 1;
    vec3 rgb = mix(colors[idx],colors[idx + 1],(lum - float(idx) * 0.5) / 0.5);
    gl_FragColor = vec4(rgb, 1.0);
}