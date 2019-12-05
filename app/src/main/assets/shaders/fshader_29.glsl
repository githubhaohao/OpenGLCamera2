//画中画
#version 100
precision highp float;
varying vec2 v_texcoord;
uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureU;
uniform lowp sampler2D s_textureV;
uniform float u_time;
uniform vec2 texSize;

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

vec2 boom(vec2 xy, vec2 size, vec2 center, float radius, float min, float max)
{
    vec2 newXy = xy;
    vec2 uv = v_texcoord;
    uv.x *= (size.x / size.y);
    vec2 centerPoint = center * size;
    vec2 realCenter = vec2(0.0, 0.0);
    realCenter.x = (centerPoint.x / size.x) * (size.x / size.y);
    realCenter.y = centerPoint.y / size.y;
    float maxX = realCenter.x + radius;
    float minX = realCenter.x - radius;
    float maxY = realCenter.y + radius;
    float minY = realCenter.y - radius;
    if (uv.x > minX && uv.x < maxX && uv.y > minY && uv.y < maxY) {
        float relX = uv.x - realCenter.x;
        float relY = uv.y - realCenter.y;
        float ang =  atan(relY, relX);
        float dist = sqrt(relX * relX + relY * relY);
        if (dist <= radius) {
            float newRad = dist * ((max * dist / radius) + min);
            float newX = realCenter.x + cos(ang) * newRad;
            newX *= (size.y / size.x);
            float newY = realCenter.y + sin(ang) * newRad;
            newXy = vec2(newX, newY);
        }
    }
    return newXy;
}

float rand(float n) {
    // fract(x) 返回x的小数部分数值
    return fract(sin(n) * 50000.0);
}

void main()
{

    vec2 center;
    center.x = rand(u_time);
    center.y = rand(center.x);
    float radius = rand(center.y) * 0.5;
    gl_FragColor = YuvToRgb(boom(v_texcoord, texSize, center, radius, 0.4, 0.6));
}