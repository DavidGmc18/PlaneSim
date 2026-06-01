#version 460 core

layout (location = 0) in vec2 aXZ;
layout (location = 1) in float aY;

uniform vec2 uOffset; // instead of model matrix
uniform mat4 uVP;

out vec2 uv;

void main() {
    vec2 xz = uOffset + aXZ;
    gl_Position = uVP *  vec4(xz.x, aY, xz.y, 1.0);
    
    uv = aXZ;
}