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

void main() {
    float circleRadius = float(0.5);
    float minZoom = 0.4;
    float maxZoom = 0.6;
    vec2 center = vec2(texSize.x / 2.0, texSize.y / 2.0);
    vec2 uv = v_texcoord;
    uv.x *= (texSize.x / texSize.y);
    vec2 realCenter = vec2(0.0, 0.0);
    realCenter.x = (center.x / texSize.x) * (texSize.x / texSize.y);
    realCenter.y = center.y / texSize.y;
    float maxX = realCenter.x + circleRadius;
    float minX = realCenter.x - circleRadius;
    float maxY = realCenter.y + circleRadius;
    float minY = realCenter.y - circleRadius;
    if (uv.x > minX && uv.x < maxX && uv.y > minY && uv.y < maxY) {
        float relX = uv.x - realCenter.x;
        float relY = uv.y - realCenter.y;
        float ang =  atan(relY, relX);
        float dist = sqrt(relX * relX + relY * relY);
        if (dist <= circleRadius) {
            float newRad = dist * ((maxZoom * dist / circleRadius) + minZoom);
            float newX = realCenter.x + cos(ang) * newRad;
            newX *= (texSize.y / texSize.x);
            float newY = realCenter.y + sin(ang) * newRad;
            gl_FragColor = YuvToRgb(vec2(newX, newY));
        } else {
            gl_FragColor = YuvToRgb(v_texcoord);
        }
    } else {
        gl_FragColor = YuvToRgb(v_texcoord);
    }
}