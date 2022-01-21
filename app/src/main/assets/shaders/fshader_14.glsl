//分色偏移
#version 100
precision highp float;
varying vec2 v_texcoord;
uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureU;
uniform lowp sampler2D s_textureV;
uniform float u_offset;

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

void main(){
    vec4 originColor = YuvToRgb(v_texcoord);
    vec4 offsetColor0 = YuvToRgb(vec2(v_texcoord.x + u_offset, v_texcoord.y + u_offset));
    vec4 offsetColor1 = YuvToRgb(vec2(v_texcoord.x - u_offset, v_texcoord.y - u_offset));

    gl_FragColor = vec4(originColor.r, offsetColor1.g, offsetColor0.b, originColor.a);
}