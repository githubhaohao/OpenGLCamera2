#version 100
precision highp float;
varying vec2 v_texcoord;
//这是个二阶向量，x是横向偏移的值，y是阈值
uniform vec2 u_ScanLineJitter;
//颜色偏移的值
uniform float u_ColorDrift;
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

float nrand(in float x,in float y){
    //fract(x) = x - floor(x);
    //dot是向量点乘,，sin就是正弦函数
    return fract(sin(dot(vec2(x,y) ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
    float v = v_texcoord.x;
    float u = v_texcoord.y;
    //用y计算0~1的随机值，再取值-1~1的数
    float jitter = nrand(v ,0.0) * 2.0 - 1.0;
    float drift = u_ColorDrift;
    //计算向左或向右偏移
    //意思是，如果第一个参数大于第二个参数，那么返回0，否则返回1
    float offsetParam = step(u_ScanLineJitter.y,abs(jitter));
    //如果offset为0就不偏移，如果为1，就偏移jtter*u_ScanLineJitter.x的位置
    jitter = jitter * offsetParam * u_ScanLineJitter.x;
    //这里计算最终的像素值，纹理坐标是0到1之间的数，如果小于0，那么图像就捅到屏幕右边去，如果超过1，那么就捅到屏幕左边去，形成颜色偏移
    vec4 color1 = YuvToRgb(fract(vec2(v, u + jitter)));
    vec4 color2 = YuvToRgb(fract(vec2(v, u + jitter + v * drift)));
    gl_FragColor = vec4(color1.r ,color2.g ,color1.b ,1.0);
}