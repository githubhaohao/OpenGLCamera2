#version 100

precision highp float;
varying vec2 v_texcoord;
uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureU;
uniform lowp sampler2D s_textureV;
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

vec4 CrossStitching(vec2 uv) {
    float stitchSize = texSize.x / 35.0;
    int invert = 0;
    vec4 color = vec4(0.0);
    float size = stitchSize;
    vec2 cPos = uv * texSize.xy;
    vec2 tlPos = floor(cPos / vec2(size, size));
    tlPos *= size;
    int remX = int(mod(cPos.x, size));
    int remY = int(mod(cPos.y, size));
    if (remX == 0 && remY == 0)  tlPos = cPos;
    vec2 blPos = tlPos;
    blPos.y += (size - 1.0);
    if ((remX == remY) || (((int(cPos.x) - int(blPos.x)) == (int(blPos.y) - int(cPos.y))))) {
        if (invert == 1) color = vec4(0.2, 0.15, 0.05, 1.0);
        else color = YuvToRgb(tlPos * vec2(1.0 / texSize.x, 1.0 / texSize.y)) * 1.4;
    } else {
        if (invert == 1) color = YuvToRgb(tlPos * vec2(1.0 / texSize.x, 1.0 / texSize.y)) * 1.4;
        else color = vec4(0.0, 0.0, 0.0, 1.0);
    }
    return color;
}
void main() {
    gl_FragColor = CrossStitching(v_texcoord);
}