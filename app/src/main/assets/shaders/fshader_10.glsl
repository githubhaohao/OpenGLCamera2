#version 100

precision highp float;
varying vec2 v_texcoord;
uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureU;
uniform lowp sampler2D s_textureV;
uniform vec2 texSize;
const int kHueLevCount = 6;
const int kSatLevCount = 7;
const int kValLevCount = 4;
float hueLevels[kHueLevCount];
float satLevels[kSatLevCount];
float valLevels[kValLevCount];
float edge_thres = 0.2;
float edge_thres2 = 5.0;
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
vec3 RGBtoHSV(float r, float g, float b) {
    float minv, maxv, delta;
    vec3 res;
    minv = min(min(r, g), b);
    maxv = max(max(r, g), b);
    res.z = maxv;
    delta = maxv - minv;
    if (maxv != 0.0)
    res.y = delta / maxv;
    else {
        res.y = 0.0;
        res.x = -1.0;
        return res;
    }
    if (r == maxv)
    res.x = ( g - b ) / delta;
    else if (g == maxv)
    res.x = 2.0 + ( b - r ) / delta;
    else
    res.x = 4.0 + ( r - g ) / delta;
    res.x = res.x * 60.0;
    if(res.x < 0.0)
    res.x = res.x + 360.0;
    return res;
}
vec3 HSVtoRGB(float h, float s, float v ) {
    int i;
    float f, p, q, t;
    vec3 res;
    if(s == 0.0) {
        res.x = v;
        res.y = v;
        res.z = v;
        return res;
    }
    h /= 60.0;
    i = int(floor( h ));
    f = h - float(i);
    p = v * ( 1.0 - s );
    q = v * ( 1.0 - s * f );
    t = v * ( 1.0 - s * ( 1.0 - f ) );
    if (i == 0) {
        res.x = v;
        res.y = t;
        res.z = p;
    } else if (i == 1) {
        res.x = q;
        res.y = v;
        res.z = p;
    } else if (i == 2) {
        res.x = p;
        res.y = v;
        res.z = t;
    } else if (i == 3) {
        res.x = p;
        res.y = q;
        res.z = v;
    } else if (i == 4) {
        res.x = t;
        res.y = p;
        res.z = v;
    } else if (i == 5) {
        res.x = v;
        res.y = p;
        res.z = q;
    }
    return res;
}
float nearestLevel(float col, int mode) {
    int levCount;
    if (mode==0) levCount = kHueLevCount;
    if (mode==1) levCount = kSatLevCount;
    if (mode==2) levCount = kValLevCount;
    for (int i=0; i<levCount-1; i++ ) {
        if (mode==0) {
            if (col >= hueLevels[i] && col <= hueLevels[i+1]) {
                return hueLevels[i+1];
            }
        }
        if (mode==1) {
            if (col >= satLevels[i] && col <= satLevels[i+1]) {
                return satLevels[i+1];
            }
        }
        if (mode==2) {
            if (col >= valLevels[i] && col <= valLevels[i+1]) {
                return valLevels[i+1];
            }
        }
    }
    return 0.0;
}
float avgIntensity(vec4 pix) {
    return (pix.r + pix.g + pix.b)/3.;
}
vec4 getPixel(vec2 coords, float dx, float dy) {
    return YuvToRgb(coords + vec2(dx, dy));
}
float IsEdge(in vec2 coords) {
    float dxtex = 1.0 / float(texSize.x);
    float dytex = 1.0 / float(texSize.y);
    float pix[9];
    int k = -1;
    float delta;
    for (int i=-1; i<2; i++) {
        for(int j=-1; j<2; j++) {
            k++;
            pix[k] = avgIntensity(getPixel(coords,float(i)*dxtex, float(j)*dytex));
        }
    }
    delta = (abs(pix[1]-pix[7]) + abs(pix[5]-pix[3]) + abs(pix[0]-pix[8])+ abs(pix[2]-pix[6]))/4.;
    return clamp(edge_thres2*delta,0.0,1.0);
}
void main() {
    hueLevels[0] = 0.0;
    hueLevels[1] = 140.0;
    hueLevels[2] = 160.0;
    hueLevels[3] = 240.0;
    hueLevels[4] = 240.0;
    hueLevels[5] = 360.0;
    satLevels[0] = 0.0;
    satLevels[1] = 0.15;
    satLevels[2] = 0.3;
    satLevels[3] = 0.45;
    satLevels[4] = 0.6;
    satLevels[5] = 0.8;
    satLevels[6] = 1.0;
    valLevels[0] = 0.0;
    valLevels[1] = 0.3;
    valLevels[2] = 0.6;
    valLevels[3] = 1.0;
    vec2 uv = v_texcoord;
    vec3 color = YuvToRgb(uv).rgb;
    vec3 vHSV =  RGBtoHSV(color.r, color.g, color.b);
    vHSV.x = nearestLevel(vHSV.x, 0);
    vHSV.y = nearestLevel(vHSV.y, 1);
    vHSV.z = nearestLevel(vHSV.z, 2);
    float edg = IsEdge(uv);
    vec3 vRGB = (edg >= edge_thres) ? vec3(0.0,0.0,0.0) : HSVtoRGB(vHSV.x,vHSV.y,vHSV.z);
    gl_FragColor = vec4(vRGB.x, vRGB.y, vRGB.z, 1.0);
}
