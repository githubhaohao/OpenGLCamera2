//fireworks
#version 100
precision highp float;
varying vec2 v_texcoord;
uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureU;
uniform lowp sampler2D s_textureV;
uniform float u_offset;
uniform vec2 texSize;
uniform int front;

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

//random value
vec2 N22(vec2 p)
{
    vec3 a = fract(p.xyx * vec3(123.34, 234.34, 345.65));
    a += dot(a, a + 34.45);
    return fract(vec2(a.x * a.y, a.y * a.z));
}

vec3 burst(vec2 st, vec2 pos, float r, vec3 col)
{
    st -= pos;
    r = 0.6 * r * r;

    /// 发光效果
    return (r / dot(st, st)) * col * 0.6;
}

/// s = p0 + ut + 0.5at^2
/// 距离加速度公式
vec2 get_pos(vec2 u, vec2 a, vec2 p0, float t, float ang)
{
    /// 根据初始位置、水平和垂直的速度、加速度来更新当前的位置
    vec2 d = p0 + vec2(u.x * cos(ang), u.y * sin(ang)) * t + 0.5 * a * t * t;
    return d;
}

// t 时刻的粒子速度
vec2 get_velocity(vec2 u, vec2 a, float t, float ang)
{
    /// 根据加速度、当前水平和垂直的速度来更新当前的速度
    return vec2(u.x * cos(ang), u.y * sin(ang)) + a * t;
}

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

float np = 60.0;
float snp = 20.0;
float R = 0.032;
float R_RATIO = 0.04;
float ACC_RATIO = 0.03;
float ANG = 90.0;
void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = 2.0 * fragCoord - 1.0;
    float aspect = texSize.x / texSize.y;

    uv = uv * vec2(1.0, aspect);
    vec3 col = vec3(0.0);
    float t = mod(u_offset, 300.0) / 30.0;

    vec2 u = vec2(0.0);                ///< 初速度
    const vec2 acc = vec2(0.0, -9.8); ///< 重力加速度 acc
    float ang = radians(ANG);             ///< 上升粒子的发射角度

    vec3 particles = vec3(0.0); //particle

    for (float i = 0.0; i < np; i++)
    {
        float r = R;
        vec2 rand = N22(vec2(i));

        /// @note 爆炸前的粒子上升

        /// 初始位置
        vec2 ip = vec2(sin(30.*rand.x) * aspect, -1.0 + r);

        /// 真正初始化速度
        u = vec2(sin(5.0*rand.x), 5.0 + sin(4.0*rand.y));

        float t_i = t - i / 5.0; ///< 时间差异化
        vec2 s = get_pos(u, acc, ip, t_i, ang);
        vec2 v = get_velocity(u, acc, t_i, ang);

        /// 计算竖直向上的运动时间
        float t_up = u.y * sin(ang) / abs(acc.y);
        /// 根据时间计算出向上运动的最大高度
        vec2 h_max = get_pos(u, acc, ip, t_up, ang);

        vec3 pcol = vec3(1.0, 1.0, 1.0);


        if (v.y < -0.5) ///< 下落速度超过一定大小则消失
        {
            r = 0.0;    ///< 隐藏
        }

        particles += burst(uv, s, r, pcol); ///< 发射上升的粒子

        float angle = u_offset / 10.0;
        vec4 light = vec4(abs(sin(angle)), abs(cos(angle)), abs(sin(angle)), 1.0);

        /// @note 爆炸后的粒子扩散
        /// 当粒子停止上升，且粒子的当前时间已经达到了上升的时间

        if (v.y > -6.5 && v.y < 0.0 && t_i >= t_up /*&& SPAWN == 1*/)
        {
            /// 把一个圆根据角度分成若干份扇形
            float unit = (360. / snp);
            for (float j = 0.0; j < snp; j++)
            {
                float ang = radians(j * unit);

                float r = 0.035;             ///< 心形粒子的半径
                r -= (t_i - t_up) * R_RATIO; ///< 根据时间差来改变粒子的大小（变小）

                /// --------------------------------------------------
                /// @note 根据（单位圆的）角度计算笛卡尔坐标
                float x = cos(ang); //coords of unit circle
                float y = sin(ang);
                /// 心形公式
                y = y + abs(x) * sqrt( (8.0 - abs(x)) / 50.0 );
                /// 心形速度向量，随着时间而变小
                vec2 heart = vec2(x * x + y * y) * (0.4 / (t_i * sqrt(t_i)));

                /// 根据心形的当前速度和加速度、初始位置等更新粒子的位置
                vec2 S = get_pos(heart, acc * ACC_RATIO, h_max, t_i - (t_up), ang);
                /// --------------------------------------------------

                vec3 pcol = vec3(1.0);
                particles += burst(uv, S, max(0.0, r), pcol);

                particles = particles * light.xyz;
            }
        }

    }

    col = particles;

    fragColor = vec4(col,  1.0);
}

void main()
{
    vec4 imageColor = textureYUV(v_texcoord);
    vec4 bgColor = vec4(0.0);
    mainImage(bgColor, vec2(v_texcoord.y, front == 0 ? 1.0 - v_texcoord.x : v_texcoord.x));
    gl_FragColor = bgColor + imageColor;
}