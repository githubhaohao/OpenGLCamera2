//Polar 马赛克
#version 100
precision highp float;
varying vec2 v_texcoord;
uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureU;
uniform lowp sampler2D s_textureV;
uniform float u_offset;
uniform vec2 center;
uniform vec2 pixelSize;

vec4 textureYUV(vec2 texCoord) {
    float y, u, v, r, g, b;
    y = texture2D(s_textureY, texCoord).r;
    u = texture2D(s_textureU, texCoord).r;
    v = texture2D(s_textureV, texCoord).r;
    u = u - 0.5;
    v = v - 0.5;
    r = y + 1.403 * v;
    g = y - 0.344 * u - 0.714 * v;
    b = y + 1.770 * u;
    return vec4(r, g, b, 1.0);
}

void main()
{
    vec2 normCoord = 2.0 * v_texcoord - 1.0;
    vec2 normCenter = 2.0 * center - 1.0;

    normCoord -= normCenter;

    float r = length(normCoord); // to polar coords
    float phi = atan(normCoord.y, normCoord.x); // to polar coords

    r = r - mod(r, pixelSize.x) + 0.03;
    phi = phi - mod(phi, pixelSize.y);

    normCoord.x = r * cos(phi);
    normCoord.y = r * sin(phi);

    normCoord += normCenter;

    vec2 textureCoordinateToUse = normCoord / 2.0 + 0.5;

    gl_FragColor = textureYUV(textureCoordinateToUse);
}