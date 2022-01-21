// Lut 滤镜
#version 100
precision highp float;
varying vec2 v_texcoord;

//Lut 纹理
uniform sampler2D s_LutTexture;
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

vec4 LutFilter(vec2 texCoord)
{
    //原始采样像素的 RGBA 值
    vec4 textureColor = YuvToRgb(texCoord);

    //获取 B 分量值，确定 LUT 小方格的 index, 取值范围转为 0～63
    float blueColor = textureColor.b * 63.0;

    //取与 B 分量值最接近的 2 个小方格的坐标
    vec2 quad1;
    quad1.y = floor(floor(blueColor) / 8.0);
    quad1.x = floor(blueColor) - (quad1.y * 8.0);

    vec2 quad2;
    quad2.y = floor(ceil(blueColor) / 7.9999);
    quad2.x = ceil(blueColor) - (quad2.y * 8.0);

    //通过 R 和 G 分量的值确定小方格内目标映射的 RGB 组合的坐标，然后归一化，转化为纹理坐标。
    vec2 texPos1;
    texPos1.x = (quad1.x * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.r);
    texPos1.y = (quad1.y * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.g);

    vec2 texPos2;
    texPos2.x = (quad2.x * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.r);
    texPos2.y = (quad2.y * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.g);

    //取目标映射对应的像素值
    vec4 newColor1 = texture2D(s_LutTexture, texPos1);
    vec4 newColor2 = texture2D(s_LutTexture, texPos2);

    //使用 Mix 方法对 2 个边界像素值进行混合
    vec4 newColor = mix(newColor1, newColor2, fract(blueColor));
    return mix(textureColor, vec4(newColor.rgb, textureColor.w), 1.0);
}

void main() {
    if (v_texcoord.y > 0.5){
        gl_FragColor = LutFilter(v_texcoord);
    } else {
        gl_FragColor = YuvToRgb(v_texcoord);
    }
}