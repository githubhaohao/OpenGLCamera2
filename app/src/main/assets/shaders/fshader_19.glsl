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

void main()
{
    if(v_texcoord.y > 0.5)
    {
        gl_FragColor = LutFilter(v_texcoord);
    }
    else
    {
        gl_FragColor = YuvToRgb(v_texcoord);
    }
}