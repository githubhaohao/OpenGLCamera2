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

vec4 textureYUV(vec2 uv) {
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

const float arg = 0.5;

float random(vec2 seed)
{
    return fract(sin(dot(seed ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
    vec4 centralColor = textureYUV(v_texcoord);
    float lum = dot(centralColor.rgb, vec3(0.299, 0.587, 0.114));
    float factor = (1.0 + arg) / (arg + lum) * factor;

    float gaussianWeightTotal = 1.0;
    vec4 sum = centralColor * gaussianWeightTotal;
    vec2 stepScale = blurSamplerScale * samplerSteps / float(samplerRadius);
    float offset = random(v_texcoord) - 0.5;

    for(int i = 1; i <= samplerRadius; ++i)
    {
        vec2 dis = (float(i) + offset) * stepScale;
        float percent = 1.0 - (float(i) + offset) / float(samplerRadius);

        {
            vec4 sampleColor1 = textureYUV(v_texcoord + dis);
            float distanceFromCentralColor1 = min(distance(centralColor, sampleColor1) * factor, 1.0);
            float gaussianWeight1 = percent * (1.0 - distanceFromCentralColor1);
            gaussianWeightTotal += gaussianWeight1;
            sum += sampleColor1 * gaussianWeight1;
        }

        {
            vec4 sampleColor2 = textureYUV(v_texcoord - dis);
            float distanceFromCentralColor2 = min(distance(centralColor, sampleColor2) * factor, 1.0);
            float gaussianWeight2 = percent * (1.0 - distanceFromCentralColor2);
            gaussianWeightTotal += gaussianWeight2;
            sum += sampleColor2 * gaussianWeight2;
        }
    }

    gl_FragColor = sum / gaussianWeightTotal;
}