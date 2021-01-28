//字符画
#version 100
precision highp float;
varying vec2 v_texcoord;
uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureU;
uniform lowp sampler2D s_textureV;
uniform lowp sampler2D s_textureMapping;
uniform float u_offset;
uniform vec2 texSize;
uniform vec2 asciiTexSize;

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

const vec3  RGB2GRAY_VEC3 = vec3(0.299, 0.587, 0.114);
const float MESH_WIDTH = 16.0;//一个字符的宽
const float MESH_HEIGHT= 23.0;//一个字符的高
const float GARY_LEVEL = 24.0;//字符表图上有 24 个字符
const float ASCIIS_WIDTH = 8.0;//字符表列数
const float ASCIIS_HEIGHT = 3.0;//字符表行数
const float MESH_ROW_NUM = 100.0;//固定小格子的行数
void main()
{
    float imageMeshWidth = texSize.x / MESH_ROW_NUM;
    float imageMeshHeight = imageMeshWidth * MESH_HEIGHT / MESH_WIDTH;

    vec2 imageTexCoord = v_texcoord * texSize;//归一化坐标转像素坐标
    vec2 midTexCoord;
    midTexCoord.x = floor(imageTexCoord.x / imageMeshWidth) * imageMeshWidth + imageMeshWidth * 0.5;//小格子中心
    midTexCoord.y = floor(imageTexCoord.y / imageMeshHeight) * imageMeshHeight + imageMeshHeight * 0.5;//小格子中心

    vec2 normalizedTexCoord = midTexCoord / texSize;//归一化
    vec4 rgbColor = YuvToRgb(normalizedTexCoord);//采样
    float grayValue = dot(rgbColor.rgb, RGB2GRAY_VEC3);//rgb分量转灰度值
    //gl_FragColor = vec4(vec3(grayValue), rgbColor.a);

    float offsetX = mod(imageTexCoord.x, imageMeshWidth) * MESH_WIDTH / imageMeshWidth;
    float offsetY = mod(imageTexCoord.y, imageMeshHeight) * MESH_HEIGHT / imageMeshHeight;

    float asciiIndex = floor((1.0 - grayValue) * GARY_LEVEL);//第几个字符
    float asciiIndexX = mod(asciiIndex, ASCIIS_WIDTH);
    float asciiIndexY = floor(asciiIndex / ASCIIS_WIDTH);

    vec2 grayTexCoord;
    grayTexCoord.x = (asciiIndexX * MESH_WIDTH + offsetX) / asciiTexSize.x;
    grayTexCoord.y = (asciiIndexY * MESH_HEIGHT + offsetY) / asciiTexSize.y;

    vec4 originColor = YuvToRgb(v_texcoord);//采样原始纹理
    vec4 mappingColor = vec4(texture2D(s_textureMapping, grayTexCoord).rgb, rgbColor.a);

    gl_FragColor = mix(originColor, mappingColor, u_offset);
}