#ifndef _BYTE_FLOW_GL_SHADER_H_
#define _BYTE_FLOW_GL_SHADER_H_

static const char kVertexShader[] =
		"#version 100\n"
		"varying vec2 v_texcoord;\n"
		"attribute vec4 position;\n"
		"attribute vec2 texcoord;\n"
		"uniform mat4 MVP;\n"
		"void main() {\n"
		"    v_texcoord = texcoord;\n"
		"    gl_Position = MVP*position;\n"
		"}";

// Pixel shader, YUV420 to RGB conversion.
static const char kFragmentShader0[] =
    "#version 100\n \
    precision highp float; \
    varying vec2 v_texcoord;\
    uniform lowp sampler2D s_textureY;\
    uniform lowp sampler2D s_textureU;\
    uniform lowp sampler2D s_textureV;\
    void main() {\
        float y, u, v, r, g, b;\
        y = texture2D(s_textureY, v_texcoord).r;\
        u = texture2D(s_textureU, v_texcoord).r;\
        v = texture2D(s_textureV, v_texcoord).r;\
        u = u - 0.5;\
        v = v - 0.5;\
        r = y + 1.403 * v;\
        g = y - 0.344 * u - 0.714 * v;\
        b = y + 1.770 * u;\
        gl_FragColor = vec4(r, g, b, 1.0);\
    }";

// Blur Filter
static const char kFragmentShader1[] =
    "#version 100\n \
    precision highp float; \
    varying vec2 v_texcoord;\
    uniform lowp sampler2D s_textureY;\
    uniform lowp sampler2D s_textureU;\
    uniform lowp sampler2D s_textureV;\
    vec4 YuvToRgb(vec2 uv) {\
        float y, u, v, r, g, b;\
        y = texture2D(s_textureY, uv).r;\
        u = texture2D(s_textureU, uv).r;\
        v = texture2D(s_textureV, uv).r;\
        u = u - 0.5;\
        v = v - 0.5;\
        r = y + 1.403 * v;\
        g = y - 0.344 * u - 0.714 * v;\
        b = y + 1.770 * u;\
        return vec4(r, g, b, 1.0);\
    }\
    void main() {\
    	vec4 sample0, sample1, sample2, sample3;\
    	float blurStep = 0.5;\
    	float step = blurStep / 100.0;\
    	sample0 = YuvToRgb(vec2(v_texcoord.x - step, v_texcoord.y - step));\
    	sample1 = YuvToRgb(vec2(v_texcoord.x + step, v_texcoord.y + step));\
    	sample2 = YuvToRgb(vec2(v_texcoord.x + step, v_texcoord.y - step));\
    	sample3 = YuvToRgb(vec2(v_texcoord.x - step, v_texcoord.y + step));\
    	gl_FragColor = (sample0 + sample1 + sample2 + sample3) / 4.0;\
    }";

// Swirl Filter
static const char kFragmentShader2[] =
    "#version 100\n\
    precision highp float; \
    varying vec2 v_texcoord;\
    uniform lowp sampler2D s_textureY;\
    uniform lowp sampler2D s_textureU;\
    uniform lowp sampler2D s_textureV;\
    uniform vec2 texSize;\
    void main() {\
        float radius = 200.0;\
        float angle = 0.8;\
        vec2 center = vec2(texSize.x / 2.0, texSize.y / 2.0);\
        vec2 tc = v_texcoord * texSize;\
        tc -= center;\
        float dist = length(tc);\
        if (dist < radius) {\
            float percent = (radius - dist) / radius;\
            float theta = percent * percent * angle * 8.0;\
            float s = sin(theta);\
            float c = cos(theta);\
            tc = vec2(dot(tc, vec2(c, -s)), dot(tc, vec2(s, c)));\
        }\
        tc += center;\
        float y, u, v, r, g, b;\
        y = texture2D(s_textureY, tc / texSize).r;\
        u = texture2D(s_textureU, tc / texSize).r;\
        v = texture2D(s_textureV, tc / texSize).r;\
        u = u - 0.5;\
        v = v - 0.5;\
        r = y + 1.403 * v;\
        g = y - 0.344 * u - 0.714 * v;\
        b = y + 1.770 * u;\
        gl_FragColor = vec4(r, g, b, 1.0);\
    }";

// Magnifying Glass Filter
static const char kFragmentShader3[] =
    "#version 100\n \
    precision highp float; \
    varying vec2 v_texcoord;\
    uniform lowp sampler2D s_textureY;\
    uniform lowp sampler2D s_textureU;\
    uniform lowp sampler2D s_textureV;\
    uniform vec2 texSize;\
    vec4 YuvToRgb(vec2 uv) {\
        float y, u, v, r, g, b;\
        y = texture2D(s_textureY, uv).r;\
        u = texture2D(s_textureU, uv).r;\
        v = texture2D(s_textureV, uv).r;\
        u = u - 0.5;\
        v = v - 0.5;\
        r = y + 1.403 * v;\
        g = y - 0.344 * u - 0.714 * v;\
        b = y + 1.770 * u;\
        return vec4(r, g, b, 1.0);\
    }\
    void main() {\
        float circleRadius = float(0.5);\
        float minZoom = 0.4;\
        float maxZoom = 0.6;\
        vec2 center = vec2(texSize.x / 2.0, texSize.y / 2.0);\
        vec2 uv = v_texcoord;\
        uv.x *= (texSize.x / texSize.y);\
        vec2 realCenter = vec2(0.0, 0.0);\
        realCenter.x = (center.x / texSize.x) * (texSize.x / texSize.y);\
        realCenter.y = center.y / texSize.y;\
        float maxX = realCenter.x + circleRadius;\
        float minX = realCenter.x - circleRadius;\
        float maxY = realCenter.y + circleRadius;\
        float minY = realCenter.y - circleRadius;\
        if (uv.x > minX && uv.x < maxX && uv.y > minY && uv.y < maxY) {\
            float relX = uv.x - realCenter.x;\
            float relY = uv.y - realCenter.y;\
            float ang =  atan(relY, relX);\
            float dist = sqrt(relX * relX + relY * relY);\
            if (dist <= circleRadius) {\
                float newRad = dist * ((maxZoom * dist / circleRadius) + minZoom);\
                float newX = realCenter.x + cos(ang) * newRad;\
                newX *= (texSize.y / texSize.x);\
                float newY = realCenter.y + sin(ang) * newRad;\
                gl_FragColor = YuvToRgb(vec2(newX, newY));\
            } else {\
                gl_FragColor = YuvToRgb(v_texcoord);\
            }\
        } else {\
            gl_FragColor = YuvToRgb(v_texcoord);\
        }\
    }";

// Fish Eye Filter
static const char kFragmentShader4[] =
    "#version 100\n\
    precision highp float;\
    const float PI = 3.1415926535;\
    varying vec2 v_texcoord;\
    uniform lowp sampler2D s_textureY;\
    uniform lowp sampler2D s_textureU;\
    uniform lowp sampler2D s_textureV;\
    void main() {\
        float aperture = 158.0;\
        float apertureHalf = 0.5 * aperture * (PI / 180.0);\
        float maxFactor = sin(apertureHalf);\
        vec2 uv;\
        vec2 xy = 2.0 * v_texcoord.xy - 1.0;\
        float d = length(xy);\
        if (d < (2.0 - maxFactor)) {\
            d = length(xy * maxFactor);\
            float z = sqrt(1.0 - d * d);\
            float r = atan(d, z) / PI;\
            float phi = atan(xy.y, xy.x);\
            uv.x = r * cos(phi) + 0.5;\
            uv.y = r * sin(phi) + 0.5;\
        } else {\
            uv = v_texcoord.xy;\
        }\
        float y, u, v, r, g, b;\
        y = texture2D(s_textureY, uv).r;\
        u = texture2D(s_textureU, uv).r;\
        v = texture2D(s_textureV, uv).r;\
        u = u - 0.5;\
        v = v - 0.5;\
        r = y + 1.403 * v;\
        g = y - 0.344 * u - 0.714 * v;\
        b = y + 1.770 * u;\
        gl_FragColor = vec4(r, g, b, 1.0);\
    }";

// Lichtenstein-esque Filter
static const char kFragmentShader5[] =
    "#version 100\n\
    precision highp float;\
    varying vec2 v_texcoord;\
    uniform lowp sampler2D s_textureY;\
    uniform lowp sampler2D s_textureU;\
    uniform lowp sampler2D s_textureV;\
    uniform vec2 texSize;\
    void main() {\
        float size = texSize.x / 75.0;\
        float radius = size * 0.5;\
        vec2 fragCoord = v_texcoord * texSize.xy;\
        vec2 quadPos = floor(fragCoord.xy / size) * size;\
        vec2 quad = quadPos/texSize.xy;\
        vec2 quadCenter = (quadPos + size/2.0);\
        float dist = length(quadCenter - fragCoord.xy);\
        float y, u, v, r, g, b;\
        y = texture2D(s_textureY, quad).r;\
        u = texture2D(s_textureU, quad).r;\
        v = texture2D(s_textureV, quad).r;\
        u = u - 0.5;\
        v = v - 0.5;\
        r = y + 1.403 * v;\
        g = y - 0.344 * u - 0.714 * v;\
        b = y + 1.770 * u;\
        if (dist > radius) {\
            gl_FragColor = vec4(0.25);\
        } else {\
             gl_FragColor = vec4(r, g, b, 1.0);\
        }\
    }";

// Triangles mosaic Filter
static const char kFragmentShader6[] =
    "#version 100\n\
    precision highp float;\
    varying vec2 v_texcoord;\
    uniform lowp sampler2D s_textureY;\
    uniform lowp sampler2D s_textureU;\
    uniform lowp sampler2D s_textureV;\
    uniform vec2 texSize;\
    vec4 YuvToRgb(vec2 uv) {\
        float y, u, v, r, g, b;\
        y = texture2D(s_textureY, uv).r;\
        u = texture2D(s_textureU, uv).r;\
        v = texture2D(s_textureV, uv).r;\
        u = u - 0.5;\
        v = v - 0.5;\
        r = y + 1.403 * v;\
        g = y - 0.344 * u - 0.714 * v;\
        b = y + 1.770 * u;\
        return vec4(r, g, b, 1.0);\
    }\
    void main() {\
        vec2 tileNum = vec2(40.0, 20.0);\
        vec2 uv = v_texcoord;\
        vec2 uv2 = floor(uv * tileNum) / tileNum;\
        uv -= uv2;\
        uv *= tileNum;\
        vec3 color = YuvToRgb(uv2 + vec2(step(1.0 - uv.y, uv.x) / (2.0 * tileNum.x), \
            step(uv.x, uv.y) / (2.0 * tileNum.y))).rgb;\
        gl_FragColor = vec4(color, 1.0);\
    }";

// Pixelation Filter
static const char kFragmentShader7[] =
    "#version 100\n\
    precision highp float;\
    varying vec2 v_texcoord;\
    uniform lowp sampler2D s_textureY;\
    uniform lowp sampler2D s_textureU;\
    uniform lowp sampler2D s_textureV;\
    uniform vec2 texSize;\
    void main() {\
        vec2 pixelSize = vec2(texSize.x/100.0, texSize.y/100.0);\
        vec2 uv = v_texcoord.xy;\
        float dx = pixelSize.x*(1./texSize.x);\
        float dy = pixelSize.y*(1./texSize.y);\
        vec2 coord = vec2(dx*floor(uv.x/dx),\
        dy*floor(uv.y/dy));\
        float y, u, v, r, g, b;\
        y = texture2D(s_textureY, coord).r;\
        u = texture2D(s_textureU, coord).r;\
        v = texture2D(s_textureV, coord).r;\
        u = u - 0.5;\
        v = v - 0.5;\
        r = y + 1.403 * v;\
        g = y - 0.344 * u - 0.714 * v;\
        b = y + 1.770 * u;\
        gl_FragColor = vec4(r, g, b, 1.0);\
    }";

// Cross Stitching Filter
static const char kFragmentShader8[] =
    "#version 100\n\
    precision highp float;\
    varying vec2 v_texcoord;\
    uniform lowp sampler2D s_textureY;\
    uniform lowp sampler2D s_textureU;\
    uniform lowp sampler2D s_textureV;\
    uniform vec2 texSize;\
    vec4 YuvToRgb(vec2 uv) {\
        float y, u, v, r, g, b;\
        y = texture2D(s_textureY, uv).r;\
        u = texture2D(s_textureU, uv).r;\
        v = texture2D(s_textureV, uv).r;\
        u = u - 0.5;\
        v = v - 0.5;\
        r = y + 1.403 * v;\
        g = y - 0.344 * u - 0.714 * v;\
        b = y + 1.770 * u;\
        return vec4(r, g, b, 1.0);\
    }\
    vec4 CrossStitching(vec2 uv) {\
        float stitchSize = texSize.x / 35.0;\
        int invert = 0;\
        vec4 color = vec4(0.0);\
        float size = stitchSize;\
        vec2 cPos = uv * texSize.xy;\
        vec2 tlPos = floor(cPos / vec2(size, size));\
        tlPos *= size;\
        int remX = int(mod(cPos.x, size));\
        int remY = int(mod(cPos.y, size));\
        if (remX == 0 && remY == 0)\
            tlPos = cPos;\
        vec2 blPos = tlPos;\
        blPos.y += (size - 1.0);\
        if ((remX == remY) || (((int(cPos.x) - int(blPos.x)) == (int(blPos.y) - int(cPos.y))))) {\
            if (invert == 1)\
                color = vec4(0.2, 0.15, 0.05, 1.0);\
            else\
                color = YuvToRgb(tlPos * vec2(1.0 / texSize.x, 1.0 / texSize.y)) * 1.4;\
        } else {\
            if (invert == 1)\
                color = YuvToRgb(tlPos * vec2(1.0 / texSize.x, 1.0 / texSize.y)) * 1.4;\
            else\
                color = vec4(0.0, 0.0, 0.0, 1.0);\
        }\
        return color;\
    }\
    void main() {\
        gl_FragColor = CrossStitching(v_texcoord);\
    }";

// Toonify Filter
static const char kFragmentShader9[] =
    "#version 100\n \
    precision highp float; \
    varying vec2 v_texcoord;\
    uniform lowp sampler2D s_textureY;\
    uniform lowp sampler2D s_textureU;\
    uniform lowp sampler2D s_textureV;\
    uniform vec2 texSize;\
    const int kHueLevCount = 6;\
    const int kSatLevCount = 7;\
    const int kValLevCount = 4;\
    float hueLevels[kHueLevCount];\
    float satLevels[kSatLevCount];\
    float valLevels[kValLevCount];\
    float edge_thres = 0.2;\
    float edge_thres2 = 5.0;\
    vec4 YuvToRgb(vec2 uv) {\
        float y, u, v, r, g, b;\
        y = texture2D(s_textureY, uv).r;\
        u = texture2D(s_textureU, uv).r;\
        v = texture2D(s_textureV, uv).r;\
        u = u - 0.5;\
        v = v - 0.5;\
        r = y + 1.403 * v;\
        g = y - 0.344 * u - 0.714 * v;\
        b = y + 1.770 * u;\
        return vec4(r, g, b, 1.0);\
    }\
    vec3 RGBtoHSV(float r, float g, float b) {\
        float minv, maxv, delta;\
        vec3 res;\
        minv = min(min(r, g), b);\
        maxv = max(max(r, g), b);\
        res.z = maxv;\
        delta = maxv - minv;\
        if (maxv != 0.0)\
            res.y = delta / maxv;\
        else {\
            res.y = 0.0;\
            res.x = -1.0;\
            return res;\
        }\
        if (r == maxv)\
            res.x = ( g - b ) / delta;\
        else if (g == maxv)\
            res.x = 2.0 + ( b - r ) / delta;\
        else\
            res.x = 4.0 + ( r - g ) / delta;\
        res.x = res.x * 60.0;\
        if(res.x < 0.0)\
            res.x = res.x + 360.0;\
        return res;\
    }\
    vec3 HSVtoRGB(float h, float s, float v ) {\
        int i;\
        float f, p, q, t;\
        vec3 res;\
        if(s == 0.0) {\
            res.x = v;\
            res.y = v;\
            res.z = v;\
            return res;\
        }\
        h /= 60.0;\
        i = int(floor( h ));\
        f = h - float(i);\
        p = v * ( 1.0 - s );\
        q = v * ( 1.0 - s * f );\
        t = v * ( 1.0 - s * ( 1.0 - f ) );\
        if (i == 0) {\
                res.x = v;\
                res.y = t;\
                res.z = p;\
        } else if (i == 1) {\
                res.x = q;\
                res.y = v;\
                res.z = p;\
        } else if (i == 2) {\
                res.x = p;\
                res.y = v;\
                res.z = t;\
        } else if (i == 3) {\
                res.x = p;\
                res.y = q;\
                res.z = v;\
        } else if (i == 4) {\
                res.x = t;\
                res.y = p;\
                res.z = v;\
        } else if (i == 5) {\
                res.x = v;\
                res.y = p;\
                res.z = q;\
        }\
        return res;\
    }\
    float nearestLevel(float col, int mode) {\
        int levCount;\
        if (mode==0) levCount = kHueLevCount;\
        if (mode==1) levCount = kSatLevCount;\
        if (mode==2) levCount = kValLevCount;\
        for (int i=0; i<levCount-1; i++ ) {\
            if (mode==0) {\
                if (col >= hueLevels[i] && col <= hueLevels[i+1]) {\
                    return hueLevels[i+1];\
                }\
            }\
            if (mode==1) {\
                if (col >= satLevels[i] && col <= satLevels[i+1]) {\
                    return satLevels[i+1];\
                }\
            }\
            if (mode==2) {\
                if (col >= valLevels[i] && col <= valLevels[i+1]) {\
                    return valLevels[i+1];\
                }\
            }\
        }\
    }\
    float avgIntensity(vec4 pix) {\
        return (pix.r + pix.g + pix.b)/3.;\
    }\
    vec4 getPixel(vec2 coords, float dx, float dy) {\
        return YuvToRgb(coords + vec2(dx, dy));\
    }\
    float IsEdge(in vec2 coords) {\
        float dxtex = 1.0 / float(texSize.x);\
        float dytex = 1.0 / float(texSize.y);\
        float pix[9];\
        int k = -1;\
        float delta;\
        for (int i=-1; i<2; i++) {\
            for(int j=-1; j<2; j++) {\
                k++;\
                pix[k] = avgIntensity(getPixel(coords,float(i)*dxtex, float(j)*dytex));\
            }\
        }\
        delta = (abs(pix[1]-pix[7]) + abs(pix[5]-pix[3]) + abs(pix[0]-pix[8])+ abs(pix[2]-pix[6]))/4.;\
        return clamp(edge_thres2*delta,0.0,1.0);\
    }\
    void main() {\
        hueLevels[0] = 0.0;\
        hueLevels[1] = 140.0;\
        hueLevels[2] = 160.0;\
        hueLevels[3] = 240.0;\
        hueLevels[4] = 240.0;\
        hueLevels[5] = 360.0;\
        satLevels[0] = 0.0;\
        satLevels[1] = 0.15;\
        satLevels[2] = 0.3;\
        satLevels[3] = 0.45;\
        satLevels[4] = 0.6;\
        satLevels[5] = 0.8;\
        satLevels[6] = 1.0;\
        valLevels[0] = 0.0;\
        valLevels[1] = 0.3;\
        valLevels[2] = 0.6;\
        valLevels[3] = 1.0;\
        vec2 uv = v_texcoord;\
        vec3 color = YuvToRgb(uv).rgb;\
        vec3 vHSV =  RGBtoHSV(color.r, color.g, color.b);\
        vHSV.x = nearestLevel(vHSV.x, 0);\
        vHSV.y = nearestLevel(vHSV.y, 1);\
        vHSV.z = nearestLevel(vHSV.z, 2);\
        float edg = IsEdge(uv);\
        vec3 vRGB = (edg >= edge_thres) ? vec3(0.0,0.0,0.0) : HSVtoRGB(vHSV.x,vHSV.y,vHSV.z);\
        gl_FragColor = vec4(vRGB.x, vRGB.y, vRGB.z, 1.0);\
    }";

// Predator Thermal Vision Filter
static const char kFragmentShader10[] =
    "#version 100\n\
    precision highp float;\
    varying vec2 v_texcoord;\
    uniform lowp sampler2D s_textureY;\
    uniform lowp sampler2D s_textureU;\
    uniform lowp sampler2D s_textureV;\
    void main() {\
        float y, u, v, r, g, b;\
        y = texture2D(s_textureY, v_texcoord).r;\
        u = texture2D(s_textureU, v_texcoord).r;\
        v = texture2D(s_textureV, v_texcoord).r;\
        u = u - 0.5;\
        v = v - 0.5;\
        r = y + 1.403 * v;\
        g = y - 0.344 * u - 0.714 * v;\
        b = y + 1.770 * u;\
        vec3 color = vec3(r, g, b);\
        vec2 uv = v_texcoord.xy;\
        vec3 colors[3];\
        colors[0] = vec3(0.,0.,1.);\
        colors[1] = vec3(1.,1.,0.);\
        colors[2] = vec3(1.,0.,0.);\
        float lum = (color.r + color.g + color.b)/3.;\
        int idx = (lum < 0.5) ? 0 : 1;\
        vec3 rgb = mix(colors[idx],colors[idx+1],(lum-float(idx)*0.5)/0.5);\
        gl_FragColor = vec4(rgb, 1.0);\
    }";

// Emboss Filter
static const char kFragmentShader11[] =
    "#version 100\n \
    precision highp float; \
    varying vec2 v_texcoord;\
    uniform lowp sampler2D s_textureY;\
    uniform lowp sampler2D s_textureU;\
    uniform lowp sampler2D s_textureV;\
    uniform vec2 texSize;\
    vec4 YuvToRgb(vec2 uv) {\
        float y, u, v, r, g, b;\
        y = texture2D(s_textureY, uv).r;\
        u = texture2D(s_textureU, uv).r;\
        v = texture2D(s_textureV, uv).r;\
        u = u - 0.5;\
        v = v - 0.5;\
        r = y + 1.403 * v;\
        g = y - 0.344 * u - 0.714 * v;\
        b = y + 1.770 * u;\
        return vec4(r, g, b, 1.0);\
    }\
    void main() {\
        vec4 color;\
        color.rgb = vec3(0.5);\
        vec2 onePixel = vec2(1.0 / texSize.x, 1.0 / texSize.y);\
        color -= YuvToRgb(v_texcoord - onePixel) * 5.0;\
        color += YuvToRgb(v_texcoord + onePixel) * 5.0;\
        color.rgb = vec3((color.r + color.g + color.b) / 3.0);\
        gl_FragColor = vec4(color.rgb, 1.0);\
    }";

// Edge Detection Filter
static const char kFragmentShader12[] =
    "#version 100\n \
    precision highp float; \
    varying vec2 v_texcoord;\
    uniform lowp sampler2D s_textureY;\
    uniform lowp sampler2D s_textureU;\
    uniform lowp sampler2D s_textureV;\
    uniform vec2 texSize;\
    vec4 YuvToRgb(vec2 uv) {\
        float y, u, v, r, g, b;\
        y = texture2D(s_textureY, uv).r;\
        u = texture2D(s_textureU, uv).r;\
        v = texture2D(s_textureV, uv).r;\
        u = u - 0.5;\
        v = v - 0.5;\
        r = y + 1.403 * v;\
        g = y - 0.344 * u - 0.714 * v;\
        b = y + 1.770 * u;\
        return vec4(r, g, b, 1.0);\
    }\
    void main() {\
        vec2 pos = v_texcoord.xy;\
        vec2 onePixel = vec2(1, 1) / texSize;\
        vec4 color = vec4(0);\
        mat3 edgeDetectionKernel = mat3(\
            -1, -1, -1,\
            -1, 8, -1,\
            -1, -1, -1\
        );\
        for(int i = 0; i < 3; i++) {\
            for(int j = 0; j < 3; j++) {\
                vec2 samplePos = pos + vec2(i - 1 , j - 1) * onePixel;\
                vec4 sampleColor = YuvToRgb(samplePos);\
                sampleColor *= edgeDetectionKernel[i][j];\
                color += sampleColor;\
            }\
        }\
        gl_FragColor = vec4(color.rgb, 1.0);\
    }";

//split screen
static const char kFragmentShader13[] =
		"#version 100\n"
		"precision highp float;\n"
		"varying vec2 v_texcoord;\n"
		"uniform lowp sampler2D s_textureY;\n"
		"uniform lowp sampler2D s_textureU;\n"
		"uniform lowp sampler2D s_textureV;\n"
		"uniform vec2 texSize;\n"
		"vec4 YuvToRgb(vec2 uv) {\n"
		"    float y, u, v, r, g, b;\n"
		"    y = texture2D(s_textureY, uv).r;\n"
		"    u = texture2D(s_textureU, uv).r;\n"
		"    v = texture2D(s_textureV, uv).r;\n"
		"    u = u - 0.5;\n"
		"    v = v - 0.5;\n"
		"    r = y + 1.403 * v;\n"
		"    g = y - 0.344 * u - 0.714 * v;\n"
		"    b = y + 1.770 * u;\n"
		"    return vec4(r, g, b, 1.0);\n"
		"}\n"
		"void main() {\n"
		"    float newY, newX;\n"
		"    if(v_texcoord.y <= 1.0/3.0)\n"
		"    {\n"
		"        newY = v_texcoord.y + 1.0/3.0;\n"
		"    }\n"
		"    else if(1.0/3.0 <= v_texcoord.y && v_texcoord.y <= 2.0/3.0)\n"
		"    {\n"
		"        newY = v_texcoord.y;\n"
		"    }\n"
		"    else\n"
		"    {\n"
		"        newY = v_texcoord.y - 1.0/3.0;\n"
		"    }\n"
		"\n"
		"    if(v_texcoord.x <= 1.0/3.0)\n"
		"    {\n"
		"        newX = v_texcoord.x + 1.0/3.0;\n"
		"    }\n"
		"    else if(1.0/3.0 <= v_texcoord.x && v_texcoord.x <= 2.0/3.0)\n"
		"    {\n"
		"        newX = v_texcoord.x;\n"
		"    }\n"
		"    else\n"
		"    {\n"
		"        newX = v_texcoord.x - 1.0/3.0;\n"
		"    }\n"
		"\n"
		"    gl_FragColor = YuvToRgb(vec2(newX, newY));\n"
		"}";

//color shift
static const char kFragmentShader14[] =
		"#version 100\n"
		"precision highp float;\n"
		"varying vec2 v_texcoord;\n"
		"uniform lowp sampler2D s_textureY;\n"
		"uniform lowp sampler2D s_textureU;\n"
		"uniform lowp sampler2D s_textureV;\n"
		"uniform float u_offset;\n"
		"vec4 YuvToRgb(vec2 uv) {\n"
		"    float y, u, v, r, g, b;\n"
		"    y = texture2D(s_textureY, uv).r;\n"
		"    u = texture2D(s_textureU, uv).r;\n"
		"    v = texture2D(s_textureV, uv).r;\n"
		"    u = u - 0.5;\n"
		"    v = v - 0.5;\n"
		"    r = y + 1.403 * v;\n"
		"    g = y - 0.344 * u - 0.714 * v;\n"
		"    b = y + 1.770 * u;\n"
		"    return vec4(r, g, b, 1.0);\n"
		"}\n"
		"void main()\n"
		"{\n"
		"    vec4 originColor = YuvToRgb(v_texcoord);\n"
		"    vec4 offsetColor0 = YuvToRgb(vec2(v_texcoord.x + u_offset, v_texcoord.y + u_offset));\n"
		"    vec4 offsetColor1 = YuvToRgb(vec2(v_texcoord.x - u_offset, v_texcoord.y - u_offset));\n"
		"    gl_FragColor = vec4(originColor.r, offsetColor1.g, offsetColor0.b, originColor.a);\n"
		"}";

//anti-white
static const char kFragmentShader15[] =
		"#version 100\n"
		"precision highp float;\n"
		"varying vec2 v_texcoord;\n"
		"uniform lowp sampler2D s_textureY;\n"
		"uniform lowp sampler2D s_textureU;\n"
		"uniform lowp sampler2D s_textureV;\n"
		"uniform float u_offset;\n"
		"vec4 YuvToRgb(vec2 uv) {\n"
		"    float y, u, v, r, g, b;\n"
		"    y = texture2D(s_textureY, uv).r;\n"
		"    u = texture2D(s_textureU, uv).r;\n"
		"    v = texture2D(s_textureV, uv).r;\n"
		"    u = u - 0.5;\n"
		"    v = v - 0.5;\n"
		"    r = y + 1.403 * v;\n"
		"    g = y - 0.344 * u - 0.714 * v;\n"
		"    b = y + 1.770 * u;\n"
		"    return vec4(r, g, b, 1.0);\n"
		"}\n"
		"void main()\n"
		"{\n"
		"    vec4 originColor = YuvToRgb(v_texcoord);\n"
		"    gl_FragColor = vec4(originColor.r + u_offset, originColor.g , originColor.b, originColor.a);\n"
		"}";

//scale circle
static const char kFragmentShader16[] =
		"#version 100\n"
		"precision highp float;\n"
		"varying vec2 v_texcoord;\n"
		"uniform lowp sampler2D s_textureY;\n"
		"uniform lowp sampler2D s_textureU;\n"
		"uniform lowp sampler2D s_textureV;\n"
		"uniform float u_offset;\n"
		"uniform vec2 texSize;\n"
		"vec4 YuvToRgb(vec2 uv) {\n"
		"    float y, u, v, r, g, b;\n"
		"    y = texture2D(s_textureY, uv).r;\n"
		"    u = texture2D(s_textureU, uv).r;\n"
		"    v = texture2D(s_textureV, uv).r;\n"
		"    u = u - 0.5;\n"
		"    v = v - 0.5;\n"
		"    r = y + 1.403 * v;\n"
		"    g = y - 0.344 * u - 0.714 * v;\n"
		"    b = y + 1.770 * u;\n"
		"    return vec4(r, g, b, 1.0);\n"
		"}\n"
		"void main()\n"
		"{\n"
		"    vec2 imgTex = v_texcoord * texSize;\n"
		"    float r = (u_offset + 0.208 ) * texSize.x;\n"
		"    if(distance(imgTex, vec2(texSize.x / 2.0, texSize.y / 2.0)) < r)\n"
		"    {\n"
		"        gl_FragColor = YuvToRgb(v_texcoord);\n"
		"    }\n"
		"    else\n"
		"    {\n"
		"        gl_FragColor = vec4(1.0, 1.0, 1.0, 0.0);\n"
		"    }\n"
		"}";

//rotate circle
static const char kFragmentShader17[]  =
		"#version 100\n"
		"precision highp float;\n"
		"varying vec2 v_texcoord;\n"
		"uniform lowp sampler2D s_textureY;\n"
		"uniform lowp sampler2D s_textureU;\n"
		"uniform lowp sampler2D s_textureV;\n"
		"uniform float u_offset;\n"
		"uniform vec2 texSize;\n"
		"vec4 YuvToRgb(vec2 uv) {\n"
		"    float y, u, v, r, g, b;\n"
		"    y = texture2D(s_textureY, uv).r;\n"
		"    u = texture2D(s_textureU, uv).r;\n"
		"    v = texture2D(s_textureV, uv).r;\n"
		"    u = u - 0.5;\n"
		"    v = v - 0.5;\n"
		"    r = y + 1.403 * v;\n"
		"    g = y - 0.344 * u - 0.714 * v;\n"
		"    b = y + 1.770 * u;\n"
		"    return vec4(r, g, b, 1.0);\n"
		"}\n"
		"const float PI = 3.141592653;\n"
		"void main()\n"
		"{\n"
		"    vec2 imgTex = v_texcoord * texSize;\n"
		"    float r = 0.3 * texSize.x;\n"
		"    if(distance(imgTex, vec2(texSize.x / 2.0, texSize.y / 2.0)) < r)\n"
		"    {\n"
		"        vec2 tranTex = v_texcoord - 0.5;\n"
		"        vec2 imgTranTex = tranTex * texSize;\n"
		"        float len = length(imgTranTex);\n"
		"        float angle = 0.0;\n"
		"        \n"
		"        angle = acos(imgTranTex.x / len);\n"
		"\n"
		"        if(tranTex.y < 0.0)\n"
		"        {\n"
		"            angle *= -1.0;\n"
		"        }\n"
		"\n"
		"        angle -= u_offset;\n"
		"\n"
		"        imgTranTex.x = len * cos(angle);\n"
		"        imgTranTex.y = len * sin(angle);\n"
		"\n"
		"        vec2 newTexCoors = imgTranTex / texSize + 0.5;\n"
		"\n"
		"        gl_FragColor = YuvToRgb(newTexCoors);\n"
		"    }\n"
		"    else\n"
		"    {\n"
		"        gl_FragColor = vec4(1.0, 1.0, 1.0, 0.0);\n"
		"    }\n"
		"}";

//dynamic glitch
static const char kFragmentShader18[] =
		"#version 100\n"
		"precision highp float;\n"
		"varying vec2 v_texcoord;\n"
		"uniform vec2 u_ScanLineJitter;\n"
		"uniform float u_ColorDrift;\n"
		"uniform lowp sampler2D s_textureY;\n"
		"uniform lowp sampler2D s_textureU;\n"
		"uniform lowp sampler2D s_textureV;\n"
		"\n"
		"vec4 YuvToRgb(vec2 uv) {\n"
		"    float y, u, v, r, g, b;\n"
		"    y = texture2D(s_textureY, uv).r;\n"
		"    u = texture2D(s_textureU, uv).r;\n"
		"    v = texture2D(s_textureV, uv).r;\n"
		"    u = u - 0.5;\n"
		"    v = v - 0.5;\n"
		"    r = y + 1.403 * v;\n"
		"    g = y - 0.344 * u - 0.714 * v;\n"
		"    b = y + 1.770 * u;\n"
		"    return vec4(r, g, b, 1.0);\n"
		"}\n"
		"\n"
		"float nrand(in float x,in float y){\n"
		"    return fract(sin(dot(vec2(x,y) ,vec2(12.9898,78.233))) * 43758.5453);\n"
		"}\n"
		"\n"
		"void main()\n"
		"{\n"
		"    float v = v_texcoord.x;\n"
		"    float u = v_texcoord.y;\n"
		"    float jitter = nrand(v ,0.0) * 2.0 - 1.0;\n"
		"    float drift = u_ColorDrift;\n"
		"    float offsetParam = step(u_ScanLineJitter.y,abs(jitter));\n"
		"    jitter = jitter * offsetParam * u_ScanLineJitter.x;\n"
		"    vec4 color1 = YuvToRgb(fract(vec2(v, u + jitter)));\n"
		"    vec4 color2 = YuvToRgb(fract(vec2(v, u + jitter + v * drift)));\n"
		"    gl_FragColor = vec4(color1.r ,color2.g ,color1.b ,1.0);\n"
		"}";

// Lut filter
static const char kFragmentShader19[] =
		"#version 100\n"
		"precision highp float;\n"
		"varying vec2 v_texcoord;\n"
		"uniform sampler2D s_LutTexture;\n"
		"uniform lowp sampler2D s_textureY;\n"
		"uniform lowp sampler2D s_textureU;\n"
		"uniform lowp sampler2D s_textureV;\n"
		"\n"
		"vec4 YuvToRgb(vec2 uv) {\n"
		"    float y, u, v, r, g, b;\n"
		"    y = texture2D(s_textureY, uv).r;\n"
		"    u = texture2D(s_textureU, uv).r;\n"
		"    v = texture2D(s_textureV, uv).r;\n"
		"    u = u - 0.5;\n"
		"    v = v - 0.5;\n"
		"    r = y + 1.403 * v;\n"
		"    g = y - 0.344 * u - 0.714 * v;\n"
		"    b = y + 1.770 * u;\n"
		"    return vec4(r, g, b, 1.0);\n"
		"}\n"
		"\n"
		"void main()\n"
		"{\n"
		"    vec4 textureColor = YuvToRgb(v_texcoord);\n"
		"\n"
		"    float blueColor = textureColor.b * 63.0;\n"
		"\n"
		"    vec2 quad1;\n"
		"    quad1.y = floor(floor(blueColor) / 8.0);\n"
		"    quad1.x = floor(blueColor) - (quad1.y * 8.0);\n"
		"\n"
		"    vec2 quad2;\n"
		"    quad2.y = floor(ceil(blueColor) / 8.0);\n"
		"    quad2.x = ceil(blueColor) - (quad2.y * 8.0);\n"
		"\n"
		"    vec2 texPos1;\n"
		"    texPos1.x = (quad1.x * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.r);\n"
		"    texPos1.y = (quad1.y * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.g);\n"
		"\n"
		"    vec2 texPos2;\n"
		"    texPos2.x = (quad2.x * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.r);\n"
		"    texPos2.y = (quad2.y * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.g);\n"
		"\n"
		"    vec4 newColor1 = texture2D(s_LutTexture, texPos1);\n"
		"    vec4 newColor2 = texture2D(s_LutTexture, texPos2);\n"
		"\n"
		"    vec4 newColor = mix(newColor1, newColor2, fract(blueColor));\n"
		"    gl_FragColor = mix(textureColor, vec4(newColor.rgb, textureColor.w), 1.0);\n"
		"}";

static const char kFragmentShader23[] =
		"#version 100\n"
		"precision highp float;\n"
		"varying vec2 v_texcoord;\n"
		"uniform lowp sampler2D s_textureY;\n"
		"uniform lowp sampler2D s_textureU;\n"
		"uniform lowp sampler2D s_textureV;\n"
		"uniform float u_offset;\n"
		"uniform vec2 texSize;\n"
		"vec4 YuvToRgb(vec2 uv) {\n"
		"    float y, u, v, r, g, b;\n"
		"    y = texture2D(s_textureY, uv).r;\n"
		"    u = texture2D(s_textureU, uv).r;\n"
		"    v = texture2D(s_textureV, uv).r;\n"
		"    u = u - 0.5;\n"
		"    v = v - 0.5;\n"
		"    r = y + 1.403 * v;\n"
		"    g = y - 0.344 * u - 0.714 * v;\n"
		"    b = y + 1.770 * u;\n"
		"    return vec4(r, g, b, 1.0);\n"
		"}\n"
		"void main()\n"
		"{\n"
		"    vec2 imgTexCoord = v_texcoord * texSize;\n"
		"    float sideLength = texSize.y / 6.0;\n"
		"    float maxOffset = 0.15 * sideLength;\n"
		"    float x = mod(imgTexCoord.x, floor(sideLength));\n"
		"    float y = mod(imgTexCoord.y, floor(sideLength));\n"
		"\n"
		"    float offset = u_offset * maxOffset;\n"
		"\n"
		"    if(offset <= x\n"
		"    && x <= sideLength - offset\n"
		"    && offset <= y\n"
		"    && y <= sideLength - offset)\n"
		"    {\n"
		"        gl_FragColor = YuvToRgb(v_texcoord);\n"
		"    }\n"
		"    else\n"
		"    {\n"
		"        gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
		"    }\n"
		"}";
#endif //_BYTE_FLOW_GL_SHADER_H_
