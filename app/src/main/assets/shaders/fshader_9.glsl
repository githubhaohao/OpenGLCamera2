#version 100

precision highp float;
varying vec2 v_texcoord;
uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureU;
uniform lowp sampler2D s_textureV;
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
    if (remX == 0 && remY == 0)
    tlPos = cPos;
    vec2 blPos = tlPos;
    blPos.y += (size - 1.0);
    if ((remX == remY) || (((int(cPos.x) - int(blPos.x)) == (int(blPos.y) - int(cPos.y))))) {
        if (invert == 1)
        color = vec4(0.2, 0.15, 0.05, 1.0);
        else
        color = YuvToRgb(tlPos * vec2(1.0 / texSize.x, 1.0 / texSize.y)) * 1.4;
    } else {
        if (invert == 1)
        color = YuvToRgb(tlPos * vec2(1.0 / texSize.x, 1.0 / texSize.y)) * 1.4;
        else
        color = vec4(0.0, 0.0, 0.0, 1.0);
    }
    return color;
}
void main() {
    gl_FragColor = CrossStitching(v_texcoord);
}