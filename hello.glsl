#version 100
varying vec2 v_texcoord;
attribute vec4 position;
attribute vec2 texcoord;
uniform mat4 MVP;
uniform float startAngle;
void main() {
    float angleSpan = 4.0 * 3.141592653;
    float curAngle = startAngle + ((position.x + 1.0)/2.0)*angleSpan;
    float tz = sin(curAngle)* 0.1;
    v_texcoord = texcoord;
    gl_Position = MVP*position;
}